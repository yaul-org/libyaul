/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/cache.h>
#include <cpu/intc.h>
#include <cpu/instructions.h>

#include <scu/dma.h>

#include <string.h>
#include <sys/callback-list.h>

#include <scu-internal.h>

/* Debug: Keep a copy of the SCU-DMA registers for each level */
#define DEBUG_COPY_DMA_HANDLES_ENABLE 0

/* Each block of SCU-DMA registers is 0x20 bytes, and luckily, each block is
 * contiguous */
#define REGISTER_BLOCK_OFFSET(level) (((level) & SCU_DMA_LEVEL_COUNT) << 5)

#if DEBUG_COPY_DMA_HANDLES_ENABLE == 1
static scu_dma_handle_t _dma_handles[SCU_DMA_LEVEL_COUNT];
#endif /* DEBUG_COPY_DMA_HANDLES_ENABLE */

#define LEVEL_STATE_IDLING      0x00
#define LEVEL_STATE_WORKING     0x01

static struct level_state {
        volatile uint8_t flags;
        callback_t callback;
} _level_state[3];

static void _scu_dma_level0_handler(void);
static void _scu_dma_level1_handler(void);
static void _scu_dma_level2_handler(void);

void
_internal_scu_dma_init(void)
{
        scu_dma_stop();

        const uint32_t scu_mask = SCU_IC_MASK_LEVEL_0_DMA_END |
                                  SCU_IC_MASK_LEVEL_1_DMA_END |
                                  SCU_IC_MASK_LEVEL_2_DMA_END |
                                  SCU_IC_MASK_DMA_ILLEGAL;

        scu_ic_mask_chg(SCU_IC_MASK_ALL, scu_mask);

        scu_ic_ihr_set(SCU_IC_INTERRUPT_LEVEL_0_DMA_END, _scu_dma_level0_handler);
        scu_ic_ihr_set(SCU_IC_INTERRUPT_LEVEL_1_DMA_END, _scu_dma_level1_handler);
        scu_ic_ihr_set(SCU_IC_INTERRUPT_LEVEL_2_DMA_END, _scu_dma_level2_handler);

        scu_dma_illegal_set(NULL);

        for (scu_dma_level_t level = 0; level <= 2; level++) {
                struct level_state * const level_state = &_level_state[level];

                level_state->flags = LEVEL_STATE_IDLING;

                callback_init(&level_state->callback);
        }

#if DEBUG_COPY_DMA_HANDLES_ENABLE
        (void)memset(&_dma_handles[0], 0x00, sizeof(scu_dma_handle_t));
        (void)memset(&_dma_handles[1], 0x00, sizeof(scu_dma_handle_t));
        (void)memset(&_dma_handles[2], 0x00, sizeof(scu_dma_handle_t));
#endif /* DEBUG_COPY_DMA_HANDLES_ENABLE */

        scu_ic_mask_chg(~scu_mask, SCU_IC_MASK_NONE);
}

void
scu_dma_level_fast_start(scu_dma_level_t level)
{
        MEMORY_WRITE(32, SCU(D0EN | REGISTER_BLOCK_OFFSET(level)), 0x00000101);
}

void
scu_dma_level_start(scu_dma_level_t level)
{
        if (level == 2) {
                /* To prevent operation errors, do not activate DMA level 2
                 * during DMA level 1 operation. */

                scu_dma_level_wait(1);
        }

        scu_dma_level_wait(level);
        scu_dma_level_fast_start(level);
}

void
scu_dma_level_stop(scu_dma_level_t level)
{
        MEMORY_WRITE(32, SCU(D0EN | REGISTER_BLOCK_OFFSET(level)), 0x00000000);

        _level_state[level].flags = LEVEL_STATE_IDLING;
}

void
scu_dma_level_wait(scu_dma_level_t level)
{
        while (true) {
                if (_level_state[level].flags != LEVEL_STATE_WORKING) {
                        return;
                }

                if ((scu_dma_level_busy(level)) == 0x00000000) {
                        return;
                }
        }
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
scu_dma_config_set(scu_dma_level_t level, scu_dma_start_factor_t start_factor,
    const scu_dma_handle_t *handle, scu_dma_callback_t callback __unused)
{
        assert(handle != NULL);

        assert(level <= 2);

        assert(start_factor <= 7);

#if DEBUG_COPY_DMA_HANDLES_ENABLE == 1
        (void)memcpy(&_dma_handles[level], handle, sizeof(scu_dma_handle_t));
#endif /* DEBUG_COPY_DMA_HANDLES_ENABLE */

        /* To prevent operation errors, do not activate DMA level 2 during DMA
         * level 1 operation. */
        if (level == 2) {
                scu_dma_level_wait(1);
        }

        scu_dma_level_wait(level);
        scu_dma_level_stop(level);

        _level_state[level].flags = LEVEL_STATE_WORKING;

        volatile uint32_t *reg_ptr =
            (volatile uint32_t *)SCU(REGISTER_BLOCK_OFFSET(level));

        *reg_ptr++ = handle->dnr;
        *reg_ptr++ = handle->dnw;
        *reg_ptr++ = handle->dnc;
        *reg_ptr++ = handle->dnad;

        volatile uint32_t * const reg_dxen = (volatile uint32_t *)reg_ptr++;

        *reg_ptr = handle->dnmd | start_factor;

        if (start_factor != SCU_DMA_START_FACTOR_ENABLE) {
                *reg_dxen = 0x00000100;
        }
}

void
scu_dma_level_end_set(scu_dma_level_t level, scu_dma_callback_t callback,
    void *work)
{
        callback_set(&_level_state[level].callback, callback, work);
}

scu_dma_level_t
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

static void
_scu_dma_level0_handler(void)
{
        _level_state[0].flags = LEVEL_STATE_IDLING;

        callback_call(&_level_state[0].callback);
}

static void
_scu_dma_level1_handler(void)
{
        _level_state[1].flags = LEVEL_STATE_IDLING;

        callback_call(&_level_state[1].callback);
}

static void
_scu_dma_level2_handler(void)
{
        _level_state[2].flags = LEVEL_STATE_IDLING;

        callback_call(&_level_state[2].callback);
}
