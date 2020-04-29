/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include <cpu/cache.h>
#include <cpu/intc.h>

#include <scu/dma.h>

#include <scu-internal.h>

/* Debug: Keep a copy of the SCU-DMA registers for each level */
#define DEBUG_COPY_DMA_HANDLES_ENABLE 0

#if DEBUG_COPY_DMA_HANDLES_ENABLE == 1
static scu_dma_handle_t _dma_handles[SCU_DMA_LEVEL_COUNT];
#endif /* DEBUG_COPY_DMA_HANDLES_ENABLE */

void
_internal_scu_dma_init(void)
{
        scu_dma_stop();

        uint32_t scu_mask;
        scu_mask = SCU_IC_MASK_LEVEL_0_DMA_END |
                   SCU_IC_MASK_LEVEL_1_DMA_END |
                   SCU_IC_MASK_LEVEL_2_DMA_END |
                   SCU_IC_MASK_DMA_ILLEGAL;

        scu_ic_mask_chg(SCU_IC_MASK_ALL, scu_mask);

        scu_dma_level0_end_set(NULL);
        scu_dma_level1_end_set(NULL);
        scu_dma_level2_end_set(NULL);

        scu_dma_illegal_set(NULL);

#if DEBUG_COPY_DMA_HANDLES_ENABLE
        (void)memset(&_dma_handles[0], 0x00, sizeof(scu_dma_handle_t));
        (void)memset(&_dma_handles[1], 0x00, sizeof(scu_dma_handle_t));
        (void)memset(&_dma_handles[2], 0x00, sizeof(scu_dma_handle_t));
#endif /* DEBUG_COPY_DMA_HANDLES_ENABLE */

        scu_ic_mask_chg(~scu_mask, SCU_IC_MASK_NONE);
}

void
scu_dma_config_buffer(scu_dma_handle_t *handle,
    const scu_dma_level_cfg_t *cfg)
{
        assert(cfg != NULL);

        assert(handle != NULL);

        /* Clear mode, starting factor and update bits */
        handle->dnmd &= ~0x01010107;

        switch (cfg->mode & 0x01) {
        case SCU_DMA_MODE_DIRECT:
                assert(cfg->xfer.direct.len != 0x00000000);
                assert(cfg->xfer.direct.dst != 0x00000000);
                assert(cfg->xfer.direct.src != 0x00000000);

                /* The absolute address must not be cached */
                handle->dnw = CPU_CACHE_THROUGH | cfg->xfer.direct.dst;
                /* The absolute address must not be cached */
                handle->dnr = CPU_CACHE_THROUGH | cfg->xfer.direct.src;

                /* Level 0 is able to transfer 1MiB
                 * Level 1 is able to transfer 4KiB
                 * Level 2 is able to transfer 4KiB */
                handle->dnc = cfg->xfer.direct.len;
                break;
        case SCU_DMA_MODE_INDIRECT:
                assert(cfg->xfer.indirect != NULL);

                /* The absolute address must not be cached */
                handle->dnw = CPU_CACHE_THROUGH | (uint32_t)cfg->xfer.indirect;
                handle->dnr = 0x00000000;
                handle->dnc = 0x00000000;
                handle->dnmd |= 0x01000000;
                break;
        }

        /* Since bit 8 being unset is effective only for the CS2 space
         * of the A bus, everything else should set it */
        handle->dnad = 0x00000100 | (cfg->stride & 0x07);

        handle->dnmd |= cfg->update & 0x00010100;
}

void
scu_dma_config_set(uint8_t level, uint8_t start_factor,
    const scu_dma_handle_t *handle, scu_dma_ihr ihr __unused)
{
        assert(handle != NULL);

        assert(level <= 2);

        assert(start_factor <= 7);

#if DEBUG_COPY_DMA_HANDLES_ENABLE == 1
        (void)memcpy(&_dma_handles[level], handle, sizeof(scu_dma_handle_t));
#endif /* DEBUG_COPY_DMA_HANDLES_ENABLE */

        switch (level) {
        case 0:
                scu_dma_level0_wait();
                scu_dma_level0_stop();

                MEMORY_WRITE(32, SCU(D0R), handle->dnr);
                MEMORY_WRITE(32, SCU(D0W), handle->dnw);
                MEMORY_WRITE(32, SCU(D0C), handle->dnc);
                MEMORY_WRITE(32, SCU(D0AD), handle->dnad);
                MEMORY_WRITE(32, SCU(D0MD), handle->dnmd | start_factor);

                if (start_factor != SCU_DMA_START_FACTOR_ENABLE) {
                        MEMORY_WRITE(32, SCU(D0EN), 0x00000100);
                }
                break;
        case 1:
                scu_dma_level1_wait();
                scu_dma_level1_stop();

                MEMORY_WRITE(32, SCU(D1R), handle->dnr);
                MEMORY_WRITE(32, SCU(D1W), handle->dnw);
                MEMORY_WRITE(32, SCU(D1C), handle->dnc);
                MEMORY_WRITE(32, SCU(D1AD), handle->dnad);
                MEMORY_WRITE(32, SCU(D1MD), handle->dnmd | start_factor);

                if (start_factor != SCU_DMA_START_FACTOR_ENABLE) {
                        MEMORY_WRITE(32, SCU(D1EN), 0x00000100);
                }
                break;
        case 2:
                /* To prevent operation errors, do not activate DMA level 2
                 * during DMA level 1 operation. */

                scu_dma_level2_wait();
                scu_dma_level2_stop();

                MEMORY_WRITE(32, SCU(D2R), handle->dnr);
                MEMORY_WRITE(32, SCU(D2W), handle->dnw);
                MEMORY_WRITE(32, SCU(D2C), handle->dnc);
                MEMORY_WRITE(32, SCU(D2AD), handle->dnad);
                MEMORY_WRITE(32, SCU(D2MD), handle->dnmd | start_factor);

                if (start_factor != SCU_DMA_START_FACTOR_ENABLE) {
                        MEMORY_WRITE(32, SCU(D2EN), 0x00000100);
                }
                break;
        }
}

int8_t
scu_dma_level_unused_get(void)
{
        if ((scu_dma_level_busy(0)) != 0x00) {
                return 0;
        }

        if ((scu_dma_level_busy(1)) != 0x00) {
                return 1;
        }

        if ((scu_dma_level_busy(2)) != 0x00) {
                return 2;
        }

        return -1;
}
