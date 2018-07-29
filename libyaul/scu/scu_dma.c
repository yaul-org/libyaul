/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <math.h>

#include <cpu/intc.h>
#include <scu/dma.h>

#include <scu-internal.h>

static void _dma_level0_ihr_handler(void);
static void _dma_level1_ihr_handler(void);
static void _dma_level2_ihr_handler(void);
static void _dma_illegal_handler(void);

static void _default_ihr(void);

#define DMA_IHR_INDEX_LEVEL0      0
#define DMA_IHR_INDEX_LEVEL1      1
#define DMA_IHR_INDEX_LEVEL2      1

static void (*_dma_ihr_table[])(void) = {
        _default_ihr,
        _default_ihr,
        _default_ihr
};

static void (*_dma_illegal_ihr)(void) = _default_ihr;

void
scu_dma_init(void)
{
        uint32_t mask;
        mask = IC_MASK_LEVEL_0_DMA_END |
               IC_MASK_LEVEL_1_DMA_END |
               IC_MASK_LEVEL_2_DMA_END |
               IC_MASK_DMA_ILLEGAL;

        uint32_t scu_mask;
        scu_mask = scu_ic_mask_get();

        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_NONE);

        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_0_DMA_END, &_dma_level0_ihr_handler);
        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_1_DMA_END, &_dma_level1_ihr_handler);
        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_2_DMA_END, &_dma_level2_ihr_handler);

        scu_ic_mask_chg(scu_mask & ~mask, IC_MASK_NONE);

        /* Writing to DSTP causes a hang */

        scu_dma_level0_stop();
        scu_dma_level1_stop();
        scu_dma_level2_stop();
}

void
scu_dma_level_config_set(const struct dma_level_cfg *cfg)
{
        uint32_t dst;
        uint32_t src;
        uint32_t count;

        if (cfg == NULL) {
                return;
        }
        
        if (cfg->dlc_xfer == NULL) {
                return;
        }

        switch (cfg->dlc_mode & 0x01) {
        case DMA_MODE_DIRECT:
                /* The absolute address must not be cached */
                dst = 0x20000000 | cfg->dlc_xfer->dst;
                /* The absolute address must not be cached */
                src = 0x20000000 | cfg->dlc_xfer->src;
                count = cfg->dlc_xfer->len;
                break;
        case DMA_MODE_INDIRECT:
                /* Transfer count cannot be ignored like in direct
                 * mode */
                if (cfg->dlc_xfer_count == 0) {
                        return;
                }

                /* The transfer table start address must be on a power
                 * of 2 boundary */
                uint32_t boundary;
                boundary = pow2(cfg->dlc_xfer_count * sizeof(struct dma_xfer)) - 1;

                if (((uint32_t)cfg->dlc_xfer & boundary) != 0x00000000) {
                        return;
                }

                /* The absolute address must not be cached */
                dst = 0x20000000 | (uint32_t)cfg->dlc_xfer;
                src = 0x00000000;
                count = 0x00000000;

                /* Force set the transfer end bit */
                cfg->dlc_xfer[cfg->dlc_xfer_count - 1].src |= 0x80000000;
                break;
        }

        /* Since bit 8 being unset is effective only for the CS2 space
         * of the A bus, everything else should set it */

        uint32_t add;
        add = 0x00000100 | (cfg->dlc_stride & 0x07);

        uint32_t mode;
        mode = ((cfg->dlc_mode & 0x01) << 24) |
               ((cfg->dlc_update & 0x0101) << 8) |
                (cfg->dlc_starting_factor & 0x07);

        switch (cfg->dlc_level & 0x03) {
        case 0:
                /* Level 0 is able to transfer 1MiB */
                count &= 0x00100000 - 1;

                scu_dma_level0_wait();
                scu_dma_level0_stop();

                MEMORY_WRITE(32, SCU(D0R), src);
                MEMORY_WRITE(32, SCU(D0W), dst);
                MEMORY_WRITE(32, SCU(D0C), count);
                MEMORY_WRITE(32, SCU(D0AD), add);
                MEMORY_WRITE(32, SCU(D0MD), mode);
                break;
        case 1:
                /* Level 1 is able transfer 4KiB */
                count &= 0x00001000 - 1;

                scu_dma_level0_wait();
                scu_dma_level1_stop();

                MEMORY_WRITE(32, SCU(D1R), src);
                MEMORY_WRITE(32, SCU(D1W), dst);
                MEMORY_WRITE(32, SCU(D1C), count);
                MEMORY_WRITE(32, SCU(D1AD), add);
                MEMORY_WRITE(32, SCU(D1MD), mode);
                break;
        case 2:
                /* Level 2 is able transfer 4KiB */
                count &= 0x00001000 - 1;

                scu_dma_level2_wait();
                scu_dma_level2_stop();

                MEMORY_WRITE(32, SCU(D2R), src);
                MEMORY_WRITE(32, SCU(D2W), dst);
                MEMORY_WRITE(32, SCU(D2C), count);
                MEMORY_WRITE(32, SCU(D2AD), add);
                MEMORY_WRITE(32, SCU(D2MD), mode);
                break;
        }

        _dma_ihr_table[cfg->dlc_level & 0x03] = _default_ihr;

        if (cfg->dlc_ihr != NULL) {
                /* Set interrupt handling routine */
                _dma_ihr_table[cfg->dlc_level & 0x03] = cfg->dlc_ihr;
        }
}

void
scu_dma_illegal_set(void (*ihr)(void))
{
        _dma_illegal_ihr = (ihr != NULL) ? ihr : _default_ihr;
}

static void
_default_ihr(void)
{
}

static void
_dma_level0_ihr_handler(void)
{
        _dma_ihr_table[DMA_IHR_INDEX_LEVEL0]();
}

static void
_dma_level1_ihr_handler(void)
{
        _dma_ihr_table[DMA_IHR_INDEX_LEVEL1]();
}

static void
_dma_level2_ihr_handler(void)
{
        _dma_ihr_table[DMA_IHR_INDEX_LEVEL2]();
}

static void
_dma_illegal_handler(void)
{
        _dma_illegal_ihr();
}
