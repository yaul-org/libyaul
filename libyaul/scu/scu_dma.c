/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <math.h>

#include <cpu/cache.h>
#include <cpu/intc.h>

#include <scu/dma.h>

#include <scu-internal.h>

void
scu_dma_init(void)
{
        uint32_t scu_mask;
        scu_mask = IC_MASK_LEVEL_0_DMA_END |
                   IC_MASK_LEVEL_1_DMA_END |
                   IC_MASK_LEVEL_2_DMA_END |
                   IC_MASK_DMA_ILLEGAL;

        scu_ic_mask_chg(IC_MASK_ALL, scu_mask);

        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_0_DMA_END, NULL);
        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_1_DMA_END, NULL);
        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_2_DMA_END, NULL);
        scu_ic_ihr_set(IC_INTERRUPT_DMA_ILLEGAL, NULL);

        scu_ic_mask_chg(~scu_mask, IC_MASK_NONE);

        /* Writing to SCU(DSTP) causes a hang */

        scu_dma_stop();
}

void
scu_dma_level_config_set(const struct dma_level_cfg *cfg)
{
        if (cfg == NULL) {
                return;
        }

        if (cfg->dlc_xfer == NULL) {
                return;
        }

        uint8_t level;
        level = cfg->dlc_level & 0x03;

        if (level > 2) {
                return;
        }

        uint32_t dst;
        uint32_t src;
        uint32_t count;

        switch (cfg->dlc_mode & 0x01) {
        case DMA_MODE_DIRECT:
                /* The absolute address must not be cached */
                dst = CPU_CACHE_THROUGH | cfg->dlc_xfer->dst;
                /* The absolute address must not be cached */
                src = CPU_CACHE_THROUGH | cfg->dlc_xfer->src;
                count = cfg->dlc_xfer->len;
                break;
        case DMA_MODE_INDIRECT:
                /* The absolute address must not be cached */
                dst = CPU_CACHE_THROUGH | (uint32_t)cfg->dlc_xfer;
                src = 0x00000000;
                count = 0x00000000;
                break;
        }

        /* Since bit 8 being unset is effective only for the CS2 space
         * of the A bus, everything else should set it */

        uint32_t add;
        add = 0x00000100 | (cfg->dlc_stride & 0x07);

        uint32_t mode;
        mode = ((cfg->dlc_mode & 0x01) << 24) |
                (cfg->dlc_update & 0x00010100) |
                (cfg->dlc_starting_factor & 0x07);

        switch (level) {
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

        uint8_t vector;
        vector = IC_INTERRUPT_LEVEL_2_DMA_END + (2 - level);

        scu_ic_ihr_set(vector, cfg->dlc_ihr);
}

int8_t
scu_dma_level_unused_get(void)
{
        if (!scu_dma_level_busy(0)) {
                return 0;
        }

        if (!scu_dma_level_busy(1)) {
                return 1;
        }

        if (!scu_dma_level_busy(2)) {
                return 2;
        }

        return -1;
}
