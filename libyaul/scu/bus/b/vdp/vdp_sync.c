/*
 * Copyright (c) 2012-2018 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/cache.h>
#include <scu/ic.h>

#include <vdp.h>

#include <sys/dma-queue.h>

static void _vblank_in_handler(void);
static void _vblank_out_handler(void);
static void _sprite_end_handler(void);

static void _vdp1_dma_handler(void *);

static void _vdp2_commit_handler(void *);

static void _default_handler(void);

/* VDP1 related state */
/* Request for VDP1 to draw */
static volatile bool _state_vdp1_commit = false;
/* VDP1 finished drawing */
static volatile bool _state_vdp1_committed = false;
/* Request to change frame buffer */
static volatile bool _state_vdp1_change = false;
/* Keep track of the current command table operation */
static volatile uint16_t _vdp1_last_command = 0x0000;
static struct dma_level_cfg _vdp1_dma_cfg;
static uint8_t _vdp1_dma_reg_buffer[DMA_REG_BUFFER_BYTE_SIZE];

/* VDP2 relate state */
/* Request for VDP2 to update state */
static volatile bool _state_vdp2_commit = false;
/* VDP2 finished updating state */
static volatile bool _state_vdp2_committed = false;
/* Request is pending */
static volatile bool _state_vdp2_pending = false;

static volatile bool _sync = false;
static volatile int16_t _interval = 0;

static volatile bool _vblank_in = false;
static volatile bool _vblank_out = false;

static void (*_user_vblank_in_handler)(void);
static void (*_user_vblank_out_handler)(void);

void
vdp_sync_init(void)
{
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_VBLANK_IN | IC_MASK_VBLANK_OUT | IC_MASK_SPRITE_END);

        _state_vdp1_commit = false;
        _state_vdp1_committed = false;
        _state_vdp1_change = false;

        _state_vdp2_commit = false;
        _state_vdp2_committed = false;
        _state_vdp2_pending = false;

        _sync = false;
        _interval = 0;

        _vdp1_last_command = 0x0000;

        _vblank_in = false;
        _vblank_out = false;

        vdp_sync_vblank_in_clear();
        vdp_sync_vblank_out_clear();

        scu_ic_ihr_set(IC_INTERRUPT_VBLANK_IN, _vblank_in_handler);
        scu_ic_ihr_set(IC_INTERRUPT_VBLANK_OUT, _vblank_out_handler);

        vdp2_commit_handler_set(_vdp2_commit_handler, NULL);

        scu_ic_ihr_set(IC_INTERRUPT_SPRITE_END, _sprite_end_handler);

        scu_ic_mask_chg(~(IC_MASK_VBLANK_IN | IC_MASK_VBLANK_OUT | IC_MASK_SPRITE_END), IC_MASK_NONE);
}

void
vdp_sync(int16_t interval)
{
        uint32_t scu_mask;
        scu_mask = IC_MASK_VBLANK_IN | IC_MASK_VBLANK_OUT;

        /* Enter critical section */
        scu_ic_mask_chg(IC_MASK_ALL, scu_mask);

        assert(!_sync);
        assert(interval >= 0);

        vdp1_sync_draw_wait();

        _interval = interval;
        _sync = true;
        scu_ic_mask_chg(~scu_mask, IC_MASK_NONE);

        /* Wait until VDP1 changed frame buffers and wait until VDP2
         * state has been committed. */
        while (_state_vdp1_change || (_state_vdp2_commit && !_state_vdp2_committed)) {
        }

        scu_ic_mask_chg(IC_MASK_ALL, scu_mask);
        _sync = false;
        scu_ic_mask_chg(~scu_mask, IC_MASK_NONE);
}

void
vdp1_sync_draw(const struct vdp1_cmdt *cmdts, uint16_t cmdt_count)
{
        if (cmdt_count == 0) {
                return;
        }

        assert(cmdts != NULL);

        /* If we're trying to issue multiple draw calls while the
         * previous draw call hasn't yet been committed, wait */
        vdp1_sync_draw_wait();

        _state_vdp1_commit = true;
        _state_vdp1_committed = false;

        assert(cmdt_count > 0);

        _vdp1_dma_cfg.dlc_mode = DMA_MODE_DIRECT;
        _vdp1_dma_cfg.dlc_xfer.direct.len = cmdt_count * sizeof(struct vdp1_cmdt);
        _vdp1_dma_cfg.dlc_xfer.direct.dst = CMD_TABLE(_vdp1_last_command, 0);
        _vdp1_dma_cfg.dlc_xfer.direct.src = CPU_CACHE_THROUGH | (uint32_t)&cmdts[0];
        _vdp1_dma_cfg.dlc_stride = DMA_STRIDE_2_BYTES;
        _vdp1_dma_cfg.dlc_update = DMA_UPDATE_NONE;

        scu_dma_config_buffer(&_vdp1_dma_reg_buffer[0], &_vdp1_dma_cfg);

        int8_t ret;
        ret = dma_queue_enqueue(&_vdp1_dma_reg_buffer[0], DMA_QUEUE_TAG_IMMEDIATE, _vdp1_dma_handler, NULL);
        assert(ret == 0);

        ret = dma_queue_flush(DMA_QUEUE_TAG_IMMEDIATE);
        assert(ret == 0);
}

void
vdp1_sync_draw_wait(void)
{
        if (!_state_vdp1_commit) {
                return;
        }

        while (!_state_vdp1_committed) {
        }

        _state_vdp1_committed = false;
        _state_vdp1_commit = false;
        _state_vdp1_change = true;
}

void
vdp2_sync_commit(void)
{
        _state_vdp2_commit = true;
        _state_vdp2_committed = false;
        _state_vdp2_pending = false;

        vdp2_commit();
}

void
vdp2_sync_commit_wait(void)
{
        while (_state_vdp2_commit && !_state_vdp2_committed) {
        }
}

uint16_t
vdp1_sync_last_command_get(void)
{
        return _vdp1_last_command;
}

void
vdp_sync_vblank_in_set(void (*ihr)(void))
{
        _user_vblank_in_handler = (ihr != NULL) ? ihr : _default_handler;
}

void
vdp_sync_vblank_out_set(void (*ihr)(void))
{
        _user_vblank_out_handler = (ihr != NULL) ? ihr : _default_handler;
}

static void
_vblank_in_handler(void)
{
        /* VBLANK-IN interrupt runs at scanline #224 */

        _vblank_in = true;
        _vblank_out = false;

        if (_sync && (_interval == 0) && _state_vdp2_commit && !_state_vdp2_pending) {
                _state_vdp2_pending = true;

                int8_t ret;
                ret = dma_queue_flush(DMA_QUEUE_TAG_VBLANK_IN);
                assert(ret == 0);
        }

        _user_vblank_in_handler();
}

static void
_vblank_out_handler(void)
{
        /* VBLANK-OUT interrupt runs at scanline #511 */

        _vblank_out = true;
        _vblank_in = false;

        if (_sync) {
                if ((_interval == 0) && _state_vdp1_change) {
                        /* Force stop drawing */
                        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
                        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
                        /* Manual mode (change) */
                        MEMORY_WRITE(16, VDP1(FBCR), 0x0003);

                        _state_vdp1_change = false;

                        /* Reset command address to the top */
                        _vdp1_last_command = 0x0000;
                }

                if (_interval > 0) {
                        _interval--;
                }
        }

        _user_vblank_out_handler();
}

static void
_sprite_end_handler(void)
{
        /* Set and get the last operation command address */
        uint16_t copr_bits;
        copr_bits = MEMORY_READ(16, VDP1(COPR));

        _vdp1_last_command = copr_bits >> 2;

        /* VDP1 request to commit is finished */
        _state_vdp1_committed = true;
}

static void
_vdp1_dma_handler(void *work __unused)
{
        /* Since the DMA transfer went through, the VDP1 is idling, so
         * start drawing */
        MEMORY_WRITE(16, VDP1(PTMR), 0x0001);
}

static void
_vdp2_commit_handler(void *work __unused)
{
        /* VDP2 request to commit is finished */
        _state_vdp2_commit = false;
        _state_vdp2_committed = true;
        _state_vdp2_pending = false;
}

static void
_default_handler(void)
{
}
