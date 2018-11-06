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

#include "vdp-internal.h"

static void _vblank_in_handler(void);
static void _vblank_out_handler(void);
static void _sprite_end_handler(void);

static void _vdp1_dma_handler(void *);

static void _vdp2_commit_handler(void *);

static void _default_handler(void);

static inline bool __attribute__ ((always_inline)) _interlace_mode_double(void);
static inline bool __attribute__ ((always_inline)) _vdp1_transfer_over(void);

/* VDP1 related state */
/* VDP1 Request for VDP1 to draw */
static volatile bool _state_vdp1_commit = false;
/* VDP1 finished transferring via DMA */
static volatile bool _state_vdp1_transferred = false;
/* VDP1 finished drawing */
static volatile bool _state_vdp1_committed = false;
/* Request to change frame buffer */
static volatile bool _state_vdp1_change = false;
/* Keep track of the current command table operation */
static volatile uint16_t _vdp1_last_command = 0x0000;
/* SCU-DMA state */
static struct dma_level_cfg _vdp1_dma_cfg;
static struct dma_reg_buffer _vdp1_dma_reg_buffer;

/* VDP2 relate state */
/* Request for VDP2 to update state */
static volatile bool _state_vdp2_commit = false;
/* VDP2 finished updating state */
static volatile bool _state_vdp2_committed = false;
/* Request is pending */
static volatile bool _state_vdp2_pending = false;

static volatile bool _sync = false;
static volatile int16_t _field_count = 0;

static volatile bool _vblank_in = false;
static volatile bool _vblank_out = false;

static void (*_user_vblank_in_handler)(void);
static void (*_user_vblank_out_handler)(void);

void
vdp_sync_init(void)
{
        uint16_t scu_mask;
        scu_mask = IC_MASK_VBLANK_IN | IC_MASK_VBLANK_OUT | IC_MASK_SPRITE_END;

        scu_ic_mask_chg(IC_MASK_ALL, scu_mask);

        _state_vdp1_commit = false;
        _state_vdp1_committed = false;
        _state_vdp1_change = false;
        _state_vdp1_transferred = false;

        _state_vdp2_commit = false;
        _state_vdp2_committed = false;
        _state_vdp2_pending = false;

        _sync = false;
        _field_count = 0;

        _vdp1_last_command = 0x0000;

        _vblank_in = false;
        _vblank_out = false;

        vdp_sync_vblank_in_clear();
        vdp_sync_vblank_out_clear();

        scu_ic_ihr_set(IC_INTERRUPT_VBLANK_IN, _vblank_in_handler);
        scu_ic_ihr_set(IC_INTERRUPT_VBLANK_OUT, _vblank_out_handler);

        vdp2_commit_handler_set(_vdp2_commit_handler, NULL);

        scu_ic_ihr_set(IC_INTERRUPT_SPRITE_END, _sprite_end_handler);

        scu_ic_mask_chg(~scu_mask, IC_MASK_NONE);
}

void
vdp_sync(int16_t interval __unused)
{
        uint32_t scu_mask;
        scu_mask = IC_MASK_VBLANK_IN | IC_MASK_VBLANK_OUT;

        /* Enter critical section */
        scu_ic_mask_chg(IC_MASK_ALL, scu_mask);

        assert(!_sync);

        vdp1_sync_draw_wait();

        _sync = true;
        scu_ic_mask_chg(~scu_mask, IC_MASK_NONE);

        /* Wait until VDP1 changed frame buffers and wait until VDP2
         * state has been committed. */
        bool vdp1_working;
        bool vdp2_working;
        do {
                vdp1_working = _state_vdp1_change;
                vdp2_working = _state_vdp2_commit && !_state_vdp2_committed;
        } while (vdp1_working || vdp2_working);

        scu_ic_mask_chg(IC_MASK_ALL, scu_mask);
        _sync = false;
        scu_ic_mask_chg(~scu_mask, IC_MASK_NONE);
}

void
vdp1_sync_draw(const struct vdp1_cmdt_list *cmdt_list)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);
        assert(cmdt_list->cmdt != NULL);
        assert(cmdt_list->count > 0);

        uint16_t count;
        count = cmdt_list->cmdt - cmdt_list->cmdts;

        assert(count > 0);

        /* Wait as previous draw calls haven't yet been committed, or at the
         * very least, have the command list transferred to VRAM */
        vdp1_sync_draw_wait();

        _state_vdp1_commit = true;
        _state_vdp1_committed = false;

        uint32_t xfer_len;
        xfer_len = count * sizeof(struct vdp1_cmdt);
        uint32_t xfer_dst;
        xfer_dst = VDP1_CMD_TABLE(_vdp1_last_command, 0);
        uint32_t xfer_src;
        xfer_src = CPU_CACHE_THROUGH | (uint32_t)cmdt_list->cmdts;

        _vdp1_dma_cfg.dlc_mode = DMA_MODE_DIRECT;
        _vdp1_dma_cfg.dlc_xfer.direct.len = xfer_len;
        _vdp1_dma_cfg.dlc_xfer.direct.dst = xfer_dst;
        _vdp1_dma_cfg.dlc_xfer.direct.src = xfer_src;
        _vdp1_dma_cfg.dlc_stride = DMA_STRIDE_2_BYTES;
        _vdp1_dma_cfg.dlc_update = DMA_UPDATE_NONE;

        scu_dma_config_buffer(&_vdp1_dma_reg_buffer, &_vdp1_dma_cfg);

        int8_t ret;
        ret = dma_queue_enqueue(&_vdp1_dma_reg_buffer, DMA_QUEUE_TAG_IMMEDIATE, _vdp1_dma_handler, NULL);
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

        if (_interlace_mode_double()) {
                /* Wait for transfer only as we can't wait until VDP1 processes
                 * the command list */
                while (!_state_vdp1_transferred) {
                }
        } else {
                /* Wait until VDP1 has processed the command list */
                while (!_state_vdp1_committed) {
                }

                _state_vdp1_commit = false;
                _state_vdp1_committed = false;
        }

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
        const uint16_t fbcr_bits[] = {
                /* Render even-numbered lines */
                0x0008,
                /* Render odd-numbered lines */
                0x000C
        };

        /* VBLANK-IN interrupt runs at scanline #224 */

        _vblank_in = true;
        _vblank_out = false;

        if (!_sync) {
                goto no_sync;
        }

        /* Commit VDP2 by default, unless double-density interlace mode is set
         * as we need to wait two field scans */
        bool commit_vdp2;
        commit_vdp2 = true;

        if ((_interlace_mode_double()) && _state_vdp1_transferred) {
                /* Assert for now, until we can perform a pseudo draw
                 * continuation */
                if ((_vdp1_transfer_over())) {
                        assert(false);

                        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);

                        return;
                }

                /* Get even/odd field scan */
                uint8_t field_scan;
                field_scan = vdp2_tvmd_field_scan_get();

                if (field_scan == _field_count) {
                        MEMORY_WRITE(16, VDP1(FBCR), fbcr_bits[field_scan]);
                        MEMORY_WRITE(16, VDP1(PTMR), 0x0002);

                        _field_count++;
                }

                /* Don't commit VDP2 until we've completed two field scans */
                commit_vdp2 = (_field_count < 2);
        }

        if (commit_vdp2 && _state_vdp2_commit && !_state_vdp2_pending) {
                _state_vdp2_pending = true;

                int8_t ret;
                ret = dma_queue_flush(DMA_QUEUE_TAG_VBLANK_IN);
                assert(ret == 0);
        }

no_sync:
        _user_vblank_in_handler();
}

static void
_vblank_out_handler(void)
{
        /* VBLANK-OUT interrupt runs at scanline #511 */

        _vblank_out = true;
        _vblank_in = false;

        if (!_sync) {
                goto no_sync;
        }

        if (!_state_vdp1_change) {
                goto no_sync;
        }

        if ((_interlace_mode_double())) {
                /* If we've completed two field scans */
                if (_field_count == 2) {
                        _field_count = 0;

                        _state_vdp1_commit = false;
                        _state_vdp1_committed = false;
                        _state_vdp1_change = false;
                        _state_vdp1_transferred = false;

                        /* Reset command address to the top */
                        _vdp1_last_command = 0x0000;
                }
        } else {
                /* Manual mode (change) */
                MEMORY_WRITE(16, VDP1(FBCR), 0x0003);

                _state_vdp1_change = false;
                _state_vdp1_transferred = false;

                /* Reset command address to the top */
                _vdp1_last_command = 0x0000;
        }

no_sync:
        _user_vblank_out_handler();
}

static void
_sprite_end_handler(void)
{
        _vdp1_last_command = vdp1_cmdt_current_get();

        /* VDP1 request to commit is finished */
        _state_vdp1_committed = true;
}

static void
_vdp1_dma_handler(void *work __unused)
{
        _state_vdp1_transferred = true;

        if ((_interlace_mode_double())) {
                return;
        }

        /* Since the DMA transfer went through, the VDP1 is idling, so start
         * drawing */
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

static inline bool __attribute__ ((always_inline))
_interlace_mode_double(void)
{
        return (_state_vdp2()->tv.interlace == TVMD_INTERLACE_DOUBLE);
}

static inline bool __attribute__ ((always_inline))
_vdp1_transfer_over(void)
{
        struct vdp1_transfer_status transfer_status;
        vdp1_transfer_status_get(&transfer_status);

        struct vdp1_mode_status mode_status;
        vdp1_mode_status_get(&mode_status);

        /* Detect if VDP1 is still drawing (transfer over status) */
        bool transfer_over;
        transfer_over = (transfer_status.vte_bef == 0x00) &&
                        (transfer_status.vte_cef == 0x00);

        return ((mode_status.vms_ptm1 != 0x00) && transfer_over);
}
