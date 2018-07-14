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

        switch (cfg->mode & 0x01) {
        case DMA_MODE_DIRECT:
                /* The absolute address must not be cached. */
                dst = 0x20000000 | cfg->direct.dst;
                /* The absolute address must not be cached. */
                src = 0x20000000 | cfg->direct.src;
                count = cfg->direct.len;
                break;
        case DMA_MODE_INDIRECT:
                src = 0x00000000;
                /* The absolute address must not be cached. */
                dst = 0x20000000 | (uint32_t)cfg->indirect.tbl;
                count = 0x00000000;
                break;
        }

        uint32_t add;

        /* Since this is effective only for the CS2 space of the A bus,
         * everything else should set bit 8 */

        add = 0x00000100 | (cfg->stride & 0x7);

        uint32_t mode;
        mode = ((cfg->mode & 0x01) << 24) | cfg->update | cfg->starting_factor;

        switch (cfg->level & 0x03) {
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

        _dma_ihr_table[cfg->level & 0x03] = _default_ihr;

        if (cfg->ihr != NULL) {
                /* Set interrupt handling routine */
                _dma_ihr_table[cfg->level & 0x03] = cfg->ihr;
        }
}

void
cpu_dma_illegal_set(void (*ihr)(void))
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
