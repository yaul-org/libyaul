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

static void _dma_level0_ihr_handler(void);
static void _dma_level1_ihr_handler(void);
static void _dma_level2_ihr_handler(void);

static void _default_ihr(void *);

static void (*_dma_level_ihr_table[])(void *) = {
        _default_ihr,
        _default_ihr,
        _default_ihr
};

static void *_dma_level_work_table[] = {
        NULL,
        NULL,
        NULL
};

void
scu_dma_init(void)
{
        uint32_t scu_mask;
        scu_mask = IC_MASK_LEVEL_0_DMA_END |
                   IC_MASK_LEVEL_1_DMA_END |
                   IC_MASK_LEVEL_2_DMA_END |
                   IC_MASK_DMA_ILLEGAL;

        scu_ic_mask_chg(IC_MASK_ALL, scu_mask);

        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_0_DMA_END, _dma_level0_ihr_handler);
        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_1_DMA_END, _dma_level1_ihr_handler);
        scu_ic_ihr_set(IC_INTERRUPT_LEVEL_2_DMA_END, _dma_level2_ihr_handler);
        scu_ic_ihr_set(IC_INTERRUPT_DMA_ILLEGAL, NULL);

        scu_ic_mask_chg(~scu_mask, IC_MASK_NONE);

        /* Writing to SCU(DSTP) causes a hang */

        scu_dma_stop();
}

void
scu_dma_level_config_buffer(uint8_t level, const struct dma_level_cfg *cfg, void *buffer)
{
        assert(cfg != NULL);

        assert(buffer != NULL);

        assert(level <= 2);

        scu_dma_level_xfer_update(cfg, buffer);

        struct state_scu_dma_level *state;
        state = buffer;

        /* Since bit 8 being unset is effective only for the CS2 space
         * of the A bus, everything else should set it */
        state->buffered_regs.dnad = 0x00000100 | (cfg->dlc_stride & 0x07);

        state->buffered_regs.dnmd = ((cfg->dlc_mode & 0x01) << 24) |
                                     (cfg->dlc_update & 0x00010100) |
                                     (cfg->dlc_starting_factor & 0x07);

        scu_dma_level_ihr_update(cfg, buffer);
}

void
scu_dma_level_config_set(uint8_t level, const struct dma_level_cfg *cfg)
{
        static struct state_scu_dma_level state;

        scu_dma_level_config_buffer(level, cfg, &state);
        scu_dma_level_buffer_set(&state);
}

void
scu_dma_level_xfer_update(const struct dma_level_cfg *cfg, void *buffer)
{
        assert(cfg != NULL);

        assert(buffer != NULL);

        const uint32_t count_mask[] = {
                /* Level 0 is able to transfer 1MiB */
                0x00100000 - 1,
                /* Level 1 is able transfer 4KiB */
                0x00001000 - 1,
                /* Level 2 is able transfer 4KiB */
                0x00001000 - 1
        };

        struct state_scu_dma_level *state;
        state = buffer;

        switch (cfg->dlc_mode & 0x01) {
        case DMA_MODE_DIRECT:
                assert(cfg->dlc_xfer.direct.len != 0x00000000);
                assert(cfg->dlc_xfer.direct.dst != 0x00000000);
                assert(cfg->dlc_xfer.direct.src != 0x00000000);

                /* The absolute address must not be cached */
                state->buffered_regs.dnw = CPU_CACHE_THROUGH | cfg->dlc_xfer.direct.dst;
                /* The absolute address must not be cached */
                state->buffered_regs.dnr = CPU_CACHE_THROUGH | cfg->dlc_xfer.direct.src;
                state->buffered_regs.dnc = cfg->dlc_xfer.direct.len & count_mask[state->level];
                break;
        case DMA_MODE_INDIRECT:
                assert(cfg->dlc_xfer.indirect != NULL);

                /* The absolute address must not be cached */
                state->buffered_regs.dnw = CPU_CACHE_THROUGH | (uint32_t)cfg->dlc_xfer.indirect;
                state->buffered_regs.dnr = 0x00000000;
                state->buffered_regs.dnc = 0x00000000;
                break;
        }
}

void
scu_dma_level_ihr_update(const struct dma_level_cfg *cfg, void *buffer)
{
        assert(cfg != NULL);

        assert(buffer != NULL);

        struct state_scu_dma_level *state;
        state = buffer;

        state->ihr = cfg->dlc_ihr;
        state->work = cfg->dlc_work;
}

void
scu_dma_level_buffer_set(const void *buffer)
{
        assert(buffer != NULL);

        const struct state_scu_dma_level *state;
        state = buffer;

        assert(state->level <= 2);

        /* Wait until the SCU DMA level is idle as the registers might
         * be modified by SCU DMA (think the update bits) */

        switch (state->level) {
        case 0:
                scu_dma_level0_wait();
                scu_dma_level0_stop();

                MEMORY_WRITE(32, SCU(D0R), state->buffered_regs.dnr);
                MEMORY_WRITE(32, SCU(D0W), state->buffered_regs.dnw);
                MEMORY_WRITE(32, SCU(D0C), state->buffered_regs.dnc);
                MEMORY_WRITE(32, SCU(D0AD), state->buffered_regs.dnad);
                MEMORY_WRITE(32, SCU(D0MD), state->buffered_regs.dnmd);
                break;
        case 1:
                scu_dma_level1_wait();
                scu_dma_level1_stop();

                MEMORY_WRITE(32, SCU(D1R), state->buffered_regs.dnr);
                MEMORY_WRITE(32, SCU(D1W), state->buffered_regs.dnw);
                MEMORY_WRITE(32, SCU(D1C), state->buffered_regs.dnc);
                MEMORY_WRITE(32, SCU(D1AD), state->buffered_regs.dnad);
                MEMORY_WRITE(32, SCU(D1MD), state->buffered_regs.dnmd);
        case 2:
                scu_dma_level2_wait();
                scu_dma_level2_stop();

                MEMORY_WRITE(32, SCU(D2R), state->buffered_regs.dnr);
                MEMORY_WRITE(32, SCU(D2W), state->buffered_regs.dnw);
                MEMORY_WRITE(32, SCU(D2C), state->buffered_regs.dnc);
                MEMORY_WRITE(32, SCU(D2AD), state->buffered_regs.dnad);
                MEMORY_WRITE(32, SCU(D2MD), state->buffered_regs.dnmd);
                break;
        }

        _dma_level_ihr_table[state->level] = (state->ihr != NULL) ? state->ihr : _default_ihr;
        _dma_level_work_table[state->level] = state->work;
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

static void
_dma_level0_ihr_handler(void)
{
        void *work;
        work = _dma_level_work_table[0];

        _dma_level_ihr_table[0](work);
}

static void
_dma_level1_ihr_handler(void)
{
        void *work;
        work = _dma_level_work_table[1];

        _dma_level_ihr_table[1](work);
}

static void
_dma_level2_ihr_handler(void)
{
        void *work;
        work = _dma_level_work_table[2];

        _dma_level_ihr_table[2](work);
}

static void
_default_ihr(void *work __unused)
{
}
