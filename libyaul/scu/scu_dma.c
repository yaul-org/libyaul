/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/cache.h>
#include <cpu/intc.h>

#include <scu/dma.h>

#include <scu-internal.h>

struct dma_regs {
        uint32_t dnr;
        uint32_t dnw;
        uint32_t dnc;
        uint32_t dnad;
        uint32_t dnmd;
} __packed __aligned(4);

static_assert(sizeof(struct dma_regs) == sizeof(struct dma_reg_buffer));

void
scu_dma_init(void)
{
        scu_dma_stop();

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
}

void
scu_dma_config_buffer(struct dma_reg_buffer *reg_buffer,
    const struct dma_level_cfg *cfg)
{
        assert(cfg != NULL);

        assert(reg_buffer != NULL);

        struct dma_regs *regs;
        regs = (struct dma_regs *)&reg_buffer->buffer[0];

        /* Clear mode, starting factor and update bits */
        regs->dnmd &= ~0x01010107;

        switch (cfg->dlc_mode & 0x01) {
        case DMA_MODE_DIRECT:
                assert(cfg->dlc_xfer.direct.len != 0x00000000);
                assert(cfg->dlc_xfer.direct.dst != 0x00000000);
                assert(cfg->dlc_xfer.direct.src != 0x00000000);

                /* The absolute address must not be cached */
                regs->dnw = CPU_CACHE_THROUGH | cfg->dlc_xfer.direct.dst;
                /* The absolute address must not be cached */
                regs->dnr = CPU_CACHE_THROUGH | cfg->dlc_xfer.direct.src;

                /* Level 0 is able to transfer 1MiB
                 * Level 1 is able to transfer 4KiB
                 * Level 2 is able to transfer 4KiB */
                regs->dnc = cfg->dlc_xfer.direct.len;
                break;
        case DMA_MODE_INDIRECT:
                assert(cfg->dlc_xfer.indirect != NULL);

                /* The absolute address must not be cached */
                regs->dnw = CPU_CACHE_THROUGH | (uint32_t)cfg->dlc_xfer.indirect;
                regs->dnr = 0x00000000;
                regs->dnc = 0x00000000;
                regs->dnmd |= 0x01000000;
                break;
        }

        /* Since bit 8 being unset is effective only for the CS2 space
         * of the A bus, everything else should set it */
        regs->dnad = 0x00000100 | (cfg->dlc_stride & 0x07);

        regs->dnmd |= cfg->dlc_update & 0x00010100;
}

void
scu_dma_config_set(uint8_t level, uint8_t start_factor,
    const struct dma_reg_buffer *reg_buffer, void (*ihr)(void))
{
        assert(reg_buffer != NULL);

        assert(level <= 2);

        assert(start_factor <= 7);

        const struct dma_regs *regs;
        regs = (struct dma_regs *)&reg_buffer->buffer[0];

        switch (level) {
        case 0:
                scu_dma_level0_wait();
                scu_dma_level0_stop();

                MEMORY_WRITE(32, SCU(D0R), regs->dnr);
                MEMORY_WRITE(32, SCU(D0W), regs->dnw);
                MEMORY_WRITE(32, SCU(D0C), regs->dnc);
                MEMORY_WRITE(32, SCU(D0AD), regs->dnad);
                MEMORY_WRITE(32, SCU(D0MD), regs->dnmd | start_factor);

                scu_ic_ihr_set(IC_INTERRUPT_LEVEL_0_DMA_END, ihr);

                MEMORY_WRITE(32, SCU(D0EN), 0x00000100);
                break;
        case 1:
                scu_dma_level1_wait();
                scu_dma_level1_stop();

                MEMORY_WRITE(32, SCU(D1R), regs->dnr);
                MEMORY_WRITE(32, SCU(D1W), regs->dnw);
                MEMORY_WRITE(32, SCU(D1C), regs->dnc);
                MEMORY_WRITE(32, SCU(D1AD), regs->dnad);
                MEMORY_WRITE(32, SCU(D1MD), regs->dnmd | start_factor);

                scu_ic_ihr_set(IC_INTERRUPT_LEVEL_1_DMA_END, ihr);

                MEMORY_WRITE(32, SCU(D1EN), 0x00000100);
                break;
        case 2:
                scu_dma_level2_wait();
                scu_dma_level2_stop();

                MEMORY_WRITE(32, SCU(D2R), regs->dnr);
                MEMORY_WRITE(32, SCU(D2W), regs->dnw);
                MEMORY_WRITE(32, SCU(D2C), regs->dnc);
                MEMORY_WRITE(32, SCU(D2AD), regs->dnad);
                MEMORY_WRITE(32, SCU(D2MD), regs->dnmd | start_factor);

                scu_ic_ihr_set(IC_INTERRUPT_LEVEL_2_DMA_END, ihr);

                MEMORY_WRITE(32, SCU(D2EN), 0x00000100);
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
