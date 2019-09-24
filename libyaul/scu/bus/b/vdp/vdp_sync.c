/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <gdb.h>

#include <cpu/cache.h>
#include <cpu/dmac.h>
#include <cpu/intc.h>

#include <scu/ic.h>

#include <vdp.h>

#include <sys/dma-queue.h>
#include <sys/callback-list.h>

#include <string.h>

#include "vdp-internal.h"

/* Debug: Use dma-queue to transfer VDP1 command list */
#define DEBUG_DMA_QUEUE_ENABLE  0

/* CPU-DMAC channel used for vdp2_sync() */
#define SYNC_DMAC_CHANNEL       0

/* Maximum number of user callbacks */
#define USER_CALLBACK_COUNT     16

#define SCU_MASK_OR     (SCU_IC_MASK_VBLANK_IN | SCU_IC_MASK_VBLANK_OUT | SCU_IC_MASK_SPRITE_END)
#define SCU_MASK_AND    (SCU_IC_MASK_ALL & ~SCU_MASK_OR)

#define STATE_SYNC                      (0x01) /* Request to synchronize */
#define STATE_INTERLACE_SINGLE          (0x02)
#define STATE_INTERLACE_DOUBLE          (0x04)

#define STATE_VDP1_REQUEST_XFER_LIST    (0x01) /* VDP1 request to transfer list */
#define STATE_VDP1_LIST_XFERRED         (0x02) /* VDP1 finished transferring list via SCU-DMA */
#define STATE_VDP1_REQUEST_COMMIT_LIST  (0x04) /* VDP1 request to commit list */
#define STATE_VDP1_LIST_COMMITTED       (0x08) /* VDP1 finished committing list */
#define STATE_VDP1_REQUEST_CHANGE       (0x10) /* VDP1 request to change frame buffers */
#define STATE_VDP1_CHANGED              (0x20) /* VDP1 changed frame buffers */

#define STATE_VDP2_COMITTING            (0x01) /* VDP2 is committing state via SCU-DMA */
#define STATE_VDP2_COMMITTED            (0x02) /* VDP2 finished committing state */

static void _vblank_in_handler(void);
static void _vblank_out_handler(void);
static void _sprite_end_handler(void);

static void _vdp1_dma_handler(const struct dma_queue_transfer *);

static void _vdp2_commit_handler(const struct dma_queue_transfer *);

static volatile struct {
        uint8_t sync;
        uint8_t vdp1;
        uint8_t vdp2;
        uint8_t field_count;
} _state __aligned(4);

/* Keep track of the current command table operation */
static volatile uint16_t _vdp1_last_command = 0x0000;

/* VDP1 SCU-DMA state */
static struct scu_dma_level_cfg _vdp1_dma_cfg __unused;
static struct scu_dma_reg_buffer _vdp1_dma_reg_buffer __unused;

static struct callback _user_vdp1_sync_callback;

static struct callback _user_vblank_in_callback;
static struct callback _user_vblank_out_callback;

static struct callback_list * _user_callback_list;

static const uint16_t _fbcr_bits[] = {
        /* Render even-numbered lines */
        0x0008,
        /* Render odd-numbered lines */
        0x000C
};

static void _vdp1_init(void);

static void _vdp2_init(void);
static void _vdp2_commit_xfer_tables_init(void);
static void _vdp2_sync_commit(struct cpu_dmac_cfg *);
static void _vdp2_sync_back_screen_table(struct cpu_dmac_cfg *);

void
vdp_sync_init(void)
{
        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_MASK_OR);

        _user_callback_list = callback_list_alloc(USER_CALLBACK_COUNT);

        callback_init(&_user_vdp1_sync_callback);
        callback_init(&_user_vblank_in_callback);
        callback_init(&_user_vblank_out_callback);

        _vdp1_init();
        _vdp2_init();

        _state.sync = 0x00;
        _state.vdp1 = 0x00;
        _state.vdp2 = 0x00;
        _state.field_count = 0x00;

        vdp_sync_vblank_in_clear();
        vdp_sync_vblank_out_clear();

        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_IN, _vblank_in_handler);
        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_OUT, _vblank_out_handler);

        scu_ic_mask_chg(SCU_MASK_AND, SCU_IC_MASK_NONE);
}

void
vdp_sync(int16_t interval __unused)
{
        _state.sync &= ~STATE_INTERLACE_SINGLE;
        _state.sync &= ~STATE_INTERLACE_DOUBLE;

        switch ((_state_vdp2()->regs.tvmd >> 6) & 0x3) {
        case 0x2:
                _state.sync |= STATE_INTERLACE_SINGLE;
                break;
        case 0x3:
                _state.sync |= STATE_INTERLACE_DOUBLE;
                break;
        }

        vdp1_sync_draw_wait();

        struct scu_dma_reg_buffer *reg_buffer;
        reg_buffer = &_state_vdp2()->commit.reg_buffer;

        int8_t ret __unused;
        ret = dma_queue_enqueue(reg_buffer, DMA_QUEUE_TAG_VBLANK_IN,
            _vdp2_commit_handler, NULL);
#ifdef DEBUG
        assert(ret == 0);
#endif /* DEBUG */

        uint32_t scu_mask;
        scu_mask = scu_ic_mask_get();

        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_MASK_OR);
        _state.sync |= STATE_SYNC;
        scu_ic_mask_chg(SCU_MASK_AND, SCU_IC_MASK_NONE);

        /* There are times when the list transfer is completed before syncing */
        if ((_state.vdp1 & (STATE_VDP1_REQUEST_XFER_LIST | STATE_VDP1_LIST_XFERRED)) != 0x00) {
                _state.vdp1 |= STATE_VDP1_REQUEST_CHANGE;
        }

        /* Wait until VDP1 changed frame buffers and wait until VDP2 state has
         * been committed */
        bool vdp1_working;
        bool vdp2_working;

        while (true) {
                uint8_t state_vdp1;
                state_vdp1 = _state.vdp1;
                uint8_t state_vdp2;
                state_vdp2 = _state.vdp2;

                bool vdp1_request_change;
                vdp1_request_change = (state_vdp1 & STATE_VDP1_REQUEST_CHANGE) != 0x00;

                bool vdp1_changed;
                vdp1_changed = (state_vdp1 & STATE_VDP1_CHANGED) != 0x00;
                vdp1_working = vdp1_request_change && !vdp1_changed;

                bool vdp2_committed;
                vdp2_committed = (state_vdp2 & STATE_VDP2_COMMITTED) != 0x00;
                vdp2_working = !vdp2_committed;

                if (!vdp1_working && !vdp2_working) {
                        break;
                }
        }

        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_MASK_OR);

        callback_list_process(_user_callback_list, true);

        _state.sync = 0x00;
        _state.vdp1 = 0x00;
        _state.vdp2 = 0x00;

        scu_ic_mask_chg(SCU_MASK_AND, SCU_IC_MASK_NONE);

        /* Reset command address to the top */
        _vdp1_last_command = 0x0000;

        scu_ic_mask_set(scu_mask);
}

bool
vdp1_sync_drawing(void)
{
        if ((_state.vdp1 & STATE_VDP1_LIST_COMMITTED) != 0x00) {
                return false;
        }

        const uint8_t state_mask = STATE_VDP1_REQUEST_COMMIT_LIST |
                                   STATE_VDP1_REQUEST_XFER_LIST |
                                   STATE_VDP1_LIST_XFERRED;

        return ((_state.vdp1 & state_mask) != 0x00);
}

void
vdp1_sync_draw(struct vdp1_cmdt_list *cmdt_list, void (*callback)(void *), void *work)
{
#ifdef DEBUG
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);
        assert(cmdt_list->cmdt != NULL);
        assert(cmdt_list->count > 0);
#endif /* DEBUG */

        uint16_t count;
        count = cmdt_list->cmdt - cmdt_list->cmdts;

        if (count == 0) {
                return;
        }

        /* Wait as previous draw calls haven't yet been committed, or at the
         * very least, have the command list transferred to VRAM */
        vdp1_sync_draw_wait();

        callback_set(&_user_vdp1_sync_callback, callback, work);

        uint32_t vdp1_vram;
        vdp1_vram = VDP1_VRAM(_vdp1_last_command * sizeof(struct vdp1_cmdt));

#ifdef DEBUG
        /* Assert that we don't exceed the amount of VRAM dedicated to command
         * lists */
        assert(vdp1_vram < (uint32_t)(vdp1_vram_texture_base_get()));

        /* Assert that the last command table in the list is terminated properly
         * (draw end) */
        assert((cmdt_list->cmdts[count - 1].cmd_ctrl & 0x8000) == 0x8000);
#endif /* DEBUG */

        /* Keep track of how many commands are being sent. Remove the "draw end"
         * command from the count as it needs to be overwritten on next request
         * to sync VDP1 */
        _vdp1_last_command += count - 1;

        _state.vdp1 |= STATE_VDP1_REQUEST_XFER_LIST;
        _state.vdp1 &= ~STATE_VDP1_LIST_XFERRED;
        _state.vdp1 &= ~STATE_VDP1_LIST_COMMITTED;
        _state.vdp1 &= ~STATE_VDP1_CHANGED;

#if DEBUG_DMA_QUEUE_ENABLE == 1
        uint32_t xfer_len;
        xfer_len = count * sizeof(struct vdp1_cmdt);
        uint32_t xfer_dst;
        xfer_dst = vdp1_vram;
        uint32_t xfer_src;
        xfer_src = CPU_CACHE_THROUGH | (uint32_t)cmdt_list->cmdts;

        _vdp1_dma_cfg.mode = SCU_DMA_MODE_DIRECT;
        _vdp1_dma_cfg.xfer.direct.len = xfer_len;
        _vdp1_dma_cfg.xfer.direct.dst = xfer_dst;
        _vdp1_dma_cfg.xfer.direct.src = xfer_src;
        _vdp1_dma_cfg.stride = SCU_DMA_STRIDE_2_BYTES;
        _vdp1_dma_cfg.update = SCU_DMA_UPDATE_NONE;

        scu_dma_config_buffer(&_vdp1_dma_reg_buffer, &_vdp1_dma_cfg);

        int8_t ret __unused;
        ret = dma_queue_enqueue(&_vdp1_dma_reg_buffer, DMA_QUEUE_TAG_IMMEDIATE,
            _vdp1_dma_handler, NULL);
#ifdef DEBUG
        assert(ret == 0);
#endif /* DEBUG */

        ret = dma_queue_flush(DMA_QUEUE_TAG_IMMEDIATE);
#ifdef DEBUG
        assert(ret >= 0);
#endif /* DEBUG */
#else
        (void)memcpy((void *)vdp1_vram,
            (void *)(CPU_CACHE_THROUGH | (uint32_t)cmdt_list->cmdts),
            count * sizeof(struct vdp1_cmdt));

        /* Call handler directly */
        _vdp1_dma_handler(NULL);
#endif /* DEBUG_DMA_QUEUE_ENABLE */
}

void
vdp1_sync_draw_wait(void)
{
        if (!(vdp1_sync_drawing())) {
                return;
        }

        uint32_t scu_mask;
        scu_mask = scu_ic_mask_get();

        scu_ic_mask_chg(SCU_MASK_AND, SCU_IC_MASK_NONE);

        while ((_state.vdp1 & STATE_VDP1_LIST_XFERRED) == 0x00) {
        }

        bool interlace_mode_double;
        interlace_mode_double = (_state.sync & STATE_INTERLACE_DOUBLE) != 0x00;

        /* When in double-density interlace mode, wait for transfer only as we
         * can't wait until VDP1 processes the command list */
        if (!interlace_mode_double) {
                /* Wait until VDP1 has processed the command list */
                while ((_state.vdp1 & STATE_VDP1_LIST_COMMITTED) == 0x00) {
                }
        }

        scu_ic_mask_set(scu_mask);
}

uint16_t
vdp1_sync_last_command_get(void)
{
        return _vdp1_last_command;
}

void
vdp2_sync_commit(void)
{
        static struct cpu_dmac_cfg dmac_cfg = {
                .channel= SYNC_DMAC_CHANNEL,
                .src_mode = CPU_DMAC_SOURCE_INCREMENT,
                .src = 0x00000000,
                .dst = 0x00000000,
                .dst_mode = CPU_DMAC_DESTINATION_INCREMENT,
                .len = 0x00000000,
                .stride = CPU_DMAC_STRIDE_2_BYTES,
                .bus_mode = CPU_DMAC_BUS_MODE_CYCLE_STEAL,
                .ihr = NULL
        };

        _vdp2_sync_back_screen_table(&dmac_cfg);
        _vdp2_sync_commit(&dmac_cfg);
}

void
vdp_sync_vblank_in_set(void (*callback_func)(void))
{
        callback_set(&_user_vblank_in_callback, (void (*)(void *))callback_func, NULL);
}

void
vdp_sync_vblank_out_set(void (*callback_func)(void))
{
        callback_set(&_user_vblank_out_callback, (void (*)(void *))callback_func, NULL);
}

int8_t
vdp_sync_user_callback_add(void (*callback)(void *), void *work)
{
        return callback_list_callback_add(_user_callback_list, callback, work);
}

void
vdp_sync_user_callback_remove(const uint8_t id)
{
        callback_list_callback_remove(_user_callback_list, id);
}

void
vdp_sync_user_callback_clear(void)
{
        callback_list_clear(_user_callback_list);
}

static void
_vdp1_init(void)
{
        _vdp1_last_command = 0x0000;

        scu_ic_ihr_set(SCU_IC_INTERRUPT_SPRITE_END, _sprite_end_handler);
}

static void
_vdp2_init(void)
{
        _vdp2_commit_xfer_tables_init();
}

static void
_vdp2_commit_xfer_tables_init(void)
{
        struct scu_dma_xfer *xfer;

        /* Write VDP2(TVMD) first */
        xfer = &_state_vdp2()->commit.xfer_table[COMMIT_XFER_VDP2_REG_TVMD];
        xfer->len = 2;
        xfer->dst = VDP2(0x0000);
        xfer->src = CPU_CACHE_THROUGH | (uint32_t)&_state_vdp2()->regs.tvmd;

        /* Skip committing the first 7 VDP2 registers:
         * TVMD
         * EXTEN
         * TVSTAT R/O
         * VRSIZE R/W
         * HCNT   R/O
         * VCNT   R/O */
        xfer = &_state_vdp2()->commit.xfer_table[COMMIT_XFER_VDP2_REGS];
        xfer->len = sizeof(_state_vdp2()->regs) - 14;
        xfer->dst = VDP2(0x000E);
        xfer->src = CPU_CACHE_THROUGH | (uint32_t)&_state_vdp2()->regs.buffer[7];

        xfer = &_state_vdp2()->commit.xfer_table[COMMIT_XFER_BACK_SCREEN_BUFFER];
        xfer->len = 0;
        xfer->dst = 0x00000000;
        xfer->src = 0x00000000;

        struct scu_dma_level_cfg dma_level_cfg = {
                .mode = SCU_DMA_MODE_INDIRECT,
                .xfer.indirect = &_state_vdp2()->commit.xfer_table[0],
                .stride = SCU_DMA_STRIDE_2_BYTES,
                .update = SCU_DMA_UPDATE_NONE
        };

        struct scu_dma_reg_buffer *reg_buffer;
        reg_buffer = &_state_vdp2()->commit.reg_buffer;

        scu_dma_config_buffer(reg_buffer, &dma_level_cfg);
}

static void
_vblank_in_handler(void)
{
        /* VBLANK-IN interrupt runs at scanline #224 */

        if ((_state.sync & STATE_SYNC) == 0x00) {
                goto no_sync;
        }

        bool interlace_mode;
        interlace_mode = (_state.sync & (STATE_INTERLACE_SINGLE | STATE_INTERLACE_DOUBLE)) != 0x00;

        if (interlace_mode) {
                /* When in single/double-density interlace mode and field count
                 * is zero, commit VDP2 state only once */
                if (_state.field_count == 2) {
                        goto no_sync;
                }

                bool interlace_mode_single;
                interlace_mode_single = (_state.sync & STATE_INTERLACE_SINGLE) != 0x00;

                /* When in single-density interlace mode, call to plot only
                 * once */
                if (interlace_mode_single && (_state.field_count > 0)) {
                        goto no_sync;
                }

                _state.vdp1 |= STATE_VDP1_REQUEST_COMMIT_LIST;

                /* Get even/odd field scan */
                uint8_t field_scan;
                field_scan = vdp2_tvmd_field_scan_get();

                /* Going from manual to 1-cycle mode requires the FCM and FCT
                 * bits to be cleared. Otherwise, we get weird behavior from the
                 * VDP1.
                 *
                 * However, VDP1(FBCR) must not be entirely cleared. This caused
                 * a lot of glitching when in double-density interlace mode */
                MEMORY_WRITE(16, VDP1(FBCR), _fbcr_bits[field_scan]);

                bool vdp1_list_xferred;
                vdp1_list_xferred = (_state.vdp1 & STATE_VDP1_LIST_XFERRED) != 0x00;

                if (vdp1_list_xferred) {
                        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
                        MEMORY_WRITE(16, VDP1(PTMR), 0x0002);
                }
        }

        if ((_state.vdp2 & (STATE_VDP2_COMITTING | STATE_VDP2_COMMITTED)) == 0x00) {
                _state.vdp2 |= STATE_VDP2_COMITTING;

                struct scu_dma_xfer *xfer;
                xfer = &_state_vdp2()->commit.xfer_table[COMMIT_XFER_BACK_SCREEN_BUFFER];

                xfer->len = _state_vdp2()->back.count * sizeof(color_rgb555_t);
                xfer->dst = (uint32_t)_state_vdp2()->back.vram;
                xfer->src = SCU_DMA_INDIRECT_TBL_END |
                            CPU_CACHE_THROUGH |
                            (uint32_t)_state_vdp2()->back.buffer;

                int8_t ret __unused;
                ret = dma_queue_flush(DMA_QUEUE_TAG_VBLANK_IN);
#ifdef DEBUG
                assert(ret >= 0);
#endif /* DEBUG */
        }

no_sync:
        callback_call(&_user_vblank_in_callback);
}

static void
_vblank_out_handler(void)
{
        /* VBLANK-OUT interrupt runs at scanline #511 */

        if ((_state.sync & STATE_SYNC) == 0x00) {
                goto no_sync;
        }

        bool interlace_mode;
        interlace_mode = (_state.sync & (STATE_INTERLACE_SINGLE | STATE_INTERLACE_DOUBLE)) != 0x00;

        if (!interlace_mode) {
                if ((_state.vdp1 & STATE_VDP1_REQUEST_CHANGE) == 0x00) {
                        goto no_sync;
                }

                /* Manual mode (change) */
                MEMORY_WRITE(16, VDP1(FBCR), 0x0003);
        } else {
                bool interlace_mode_double;
                interlace_mode_double = (_state.sync & STATE_INTERLACE_DOUBLE) != 0x00;

                if (interlace_mode_double) {
                        /* Get even/odd field scan */
                        uint8_t field_scan;
                        field_scan = vdp2_tvmd_field_scan_get();

                        MEMORY_WRITE(16, VDP1(FBCR), _fbcr_bits[field_scan]);
                }

                _state.field_count++;

                /* When in single-density interlace mode, wait two fields */
                if (_state.field_count < 2) {
                        goto no_change;
                }
        }

        _state.vdp1 &= ~STATE_VDP1_REQUEST_CHANGE;
        _state.vdp1 &= ~STATE_VDP1_LIST_XFERRED;

        _state.vdp1 |= STATE_VDP1_CHANGED;

no_change:
no_sync:
        callback_call(&_user_vblank_out_callback);
}

static void
_sprite_end_handler(void)
{
        /* VDP1 request to commit is finished */

        /* Don't clear STATE_VDP1_REQUEST_COMMIT_LIST as we need to know that
         * we've requested to commit */
        _state.vdp1 |= STATE_VDP1_LIST_COMMITTED;

        callback_call(&_user_vdp1_sync_callback);
}

static void
_vdp1_dma_handler(const struct dma_queue_transfer *transfer)
{
        if ((transfer != NULL) && (transfer->status != DMA_QUEUE_STATUS_COMPLETE)) {
                return;
        }

        /* Don't clear STATE_VDP1_REQUEST_XFER_LIST as we want to know that
         * we've requested to transfer a command list */
        _state.vdp1 |= STATE_VDP1_LIST_XFERRED;

        /* We can only draw during VBLANK-IN and in 1-cycle mode when in
         * double-density interlace mode */
        bool interlace_mode_double;
        interlace_mode_double = (_state.sync & STATE_INTERLACE_DOUBLE) != 0x00;

        if (interlace_mode_double) {
                return;
        }

        _state.vdp1 |= STATE_VDP1_REQUEST_COMMIT_LIST;

        /* Since the DMA transfer went through, the VDP1 is idling, so start
         * drawing */
        MEMORY_WRITE(16, VDP1(PTMR), 0x0001);
}

static void
_vdp2_commit_handler(const struct dma_queue_transfer *transfer)
{
        if ((transfer != NULL) && (transfer->status != DMA_QUEUE_STATUS_COMPLETE)) {
                return;
        }

        _state.vdp2 |= STATE_VDP2_COMMITTED;
        _state.vdp2 &= ~STATE_VDP2_COMITTING;
}

static void
_vdp2_sync_commit(struct cpu_dmac_cfg *dmac_cfg)
{
        assert(dmac_cfg != NULL);
        dmac_cfg->len = sizeof(_state_vdp2()->regs) - 14;
        dmac_cfg->dst = VDP2(0x000E);
        dmac_cfg->src = (uint32_t)&_state_vdp2()->regs.buffer[7];

        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
        cpu_dmac_channel_config_set(dmac_cfg);

        MEMORY_WRITE(16, VDP2(0x0000), _state_vdp2()->regs.buffer[0]);

        cpu_dmac_enable();
        cpu_dmac_channel_start(SYNC_DMAC_CHANNEL);

        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
}

static void
_vdp2_sync_back_screen_table(struct cpu_dmac_cfg *dmac_cfg)
{
        assert(dmac_cfg != NULL);
        dmac_cfg->len = _state_vdp2()->back.count * sizeof(color_rgb555_t);
        dmac_cfg->dst = (uint32_t)_state_vdp2()->back.vram;
        dmac_cfg->src = (uint32_t)_state_vdp2()->back.buffer;

        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
        cpu_dmac_channel_config_set(dmac_cfg);

        cpu_dmac_enable();
        cpu_dmac_channel_start(SYNC_DMAC_CHANNEL);
        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
}
