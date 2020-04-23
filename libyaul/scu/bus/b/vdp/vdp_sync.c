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
#define USER_CALLBACK_COUNT     8

#if DEBUG_DMA_QUEUE_ENABLE == 0
#define SCU_MASK_OR     (SCU_IC_MASK_VBLANK_IN |                               \
                         SCU_IC_MASK_VBLANK_OUT |                              \
                         SCU_IC_MASK_SPRITE_END)
#else
#define SCU_MASK_OR     (SCU_IC_MASK_VBLANK_IN |                               \
                         SCU_IC_MASK_VBLANK_OUT |                              \
                         SCU_IC_MASK_SPRITE_END |                              \
                         SCU_IC_MASK_LEVEL_0_DMA_END |                         \
                         SCU_IC_MASK_LEVEL_1_DMA_END |                         \
                         SCU_IC_MASK_LEVEL_2_DMA_END)
#endif /* DEBUG_DMA_QUEUE_ENABLE */
#define SCU_MASK_AND    (SCU_IC_MASK_ALL & ~SCU_MASK_OR)

#define STATE_SYNC                      (0x02) /* Request to synchronize */
#define STATE_INTERLACE_SINGLE          (0x04)
#define STATE_INTERLACE_DOUBLE          (0x08)
#define STATE_MASK                      (0x0E)

#define STATE_VDP1_MODE_AUTO            (0x00)
#define STATE_VDP1_MODE_FIXED           (0x01)
#define STATE_VDP1_MODE_VARIABLE        (0x02)
#define STATE_VDP1_MODE_MASK            (0x03)

#define STATE_VDP1_REQUEST_XFER_LIST    (0x04) /* VDP1 request to transfer list */
#define STATE_VDP1_LIST_XFERRED         (0x08) /* VDP1 finished transferring list via SCU-DMA */
#define STATE_VDP1_REQUEST_COMMIT_LIST  (0x10) /* VDP1 request to commit list */
#define STATE_VDP1_LIST_COMMITTED       (0x20) /* VDP1 finished committing list */
#define STATE_VDP1_REQUEST_CHANGE       (0x40) /* VDP1 request to change frame buffers */
#define STATE_VDP1_CHANGED              (0x80) /* VDP1 changed frame buffers */
#define STATE_VDP1_MASK                 (0xFC)

#define STATE_VDP2_COMITTING            (0x01) /* VDP2 is committing state via SCU-DMA */
#define STATE_VDP2_COMMITTED            (0x02) /* VDP2 finished committing state */
#define STATE_VDP2_MASK                 (0x03)

#define TRANSFER_TYPE_BUFFER            (0)
#define TRANSFER_TYPE_ORDERLIST         (1)

typedef struct {
        uint8_t transfer_type;
        const struct vdp1_cmdt_orderlist *cmdt_orderlist;
        const struct vdp1_cmdt *cmdts;
        const uint16_t count;
        void (*callback)(void *);
        void *work;
} vdp1_sync_put_args_t;

typedef void (*vdp1_func_ptr)(const void *);

static void _vblank_in_handler(void);
static void _vblank_out_handler(void);
static void _sprite_end_handler(void);

/* Unused when DEBUG_DMA_QUEUE_ENABLE is set to 0 */
static void _vdp1_dma_handler(const struct dma_queue_transfer *) __used;
static void _vdp2_dma_handler(const struct dma_queue_transfer *);

static volatile struct {
        uint8_t sync;
        uint8_t vdp1;
        uint8_t vdp2;
        uint8_t field_count;
} _state __aligned(4);

/* Keep track of the current command table operation */
static volatile uint16_t _vdp1_last_command = 0x0000;

static struct callback _user_vdp1_sync_callback;
static struct callback _user_vblank_in_callback;
static struct callback _user_vblank_out_callback;

static struct callback_list * _user_callback_list;

static const uint16_t _fbcr_bits[] __unused = {
        /* Render even-numbered lines */
        0x0008,
        /* Render odd-numbered lines */
        0x000C
};

static void _vdp1_mode_auto_sync_put(const void *);
static void _vdp1_mode_auto_dma(const void *);
static void _vdp1_mode_auto_sprite_end(const void *);
static void _vdp1_mode_auto_vblank_in(const void *);
static void _vdp1_mode_auto_vblank_out(const void *);

static void _vdp1_mode_fixed_sync_put(const void *);
static void _vdp1_mode_fixed_dma(const void *);
static void _vdp1_mode_fixed_sprite_end(const void *);
static void _vdp1_mode_fixed_vblank_in(const void *);
static void _vdp1_mode_fixed_vblank_out(const void *);

static void _vdp1_mode_variable_sync_put(const void *);
static void _vdp1_mode_variable_dma(const void *);
static void _vdp1_mode_variable_sprite_end(const void *);
static void _vdp1_mode_variable_vblank_in(const void *);
static void _vdp1_mode_variable_vblank_out(const void *);

/* Interface to either of the three modes */
static const struct {
        vdp1_func_ptr sync_put;
        vdp1_func_ptr dma;
        vdp1_func_ptr sprite_end;
        vdp1_func_ptr vblank_in;
        vdp1_func_ptr vblank_out;
}
_vdp1_mode_table[] = {
        {
                .sync_put = _vdp1_mode_auto_sync_put,
                .dma = _vdp1_mode_auto_dma,
                .sprite_end = _vdp1_mode_auto_sprite_end,
                .vblank_in = _vdp1_mode_auto_vblank_in,
                .vblank_out = _vdp1_mode_auto_vblank_out
        }, {
                .sync_put = _vdp1_mode_fixed_sync_put,
                .dma = _vdp1_mode_fixed_dma,
                .sprite_end = _vdp1_mode_fixed_sprite_end,
                .vblank_in = _vdp1_mode_fixed_vblank_in,
                .vblank_out = _vdp1_mode_fixed_vblank_out
        }, {
                .sync_put = _vdp1_mode_variable_sync_put,
                .dma = _vdp1_mode_variable_dma,
                .sprite_end = _vdp1_mode_variable_sprite_end,
                .vblank_in = _vdp1_mode_variable_vblank_in,
                .vblank_out = _vdp1_mode_variable_vblank_out
        }
},
/* Pointer to the current VDP1 mode */
*_current_vdp1_mode;

static void _vdp1_init(void);

static inline __always_inline void _vdp1_sync_put_call(const void *);
static inline __always_inline void _vdp1_dma_call(const void *);
static inline __always_inline void _vdp1_sprite_end_call(const void *);
static inline __always_inline void _vdp1_vblank_in_call(const void *);
static inline __always_inline void _vdp1_vblank_out_call(const void *);

static void _vdp1_cmdt_transfer(const struct vdp1_cmdt *,
    const uint32_t, const uint16_t);
static void _vdp1_cmdt_orderlist_transfer(const struct vdp1_cmdt_orderlist *);

static void _vdp2_init(void);
static void _vdp2_commit_xfer_tables_init(void);
static void _vdp2_sync_commit(struct cpu_dmac_cfg *);
static void _vdp2_sync_back_screen_table(struct cpu_dmac_cfg *);

void
vdp_sync_init(void)
{
        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_MASK_OR);

        _user_callback_list = callback_list_alloc(USER_CALLBACK_COUNT);

        callback_init(&_user_vblank_in_callback);
        callback_init(&_user_vblank_out_callback);

        _state.sync = 0x00;
        _state.vdp1 = 0x00;
        _state.vdp2 = 0x00;
        _state.field_count = 0x00;

        _vdp1_init();
        _vdp2_init();

        vdp_sync_vblank_in_clear();
        vdp_sync_vblank_out_clear();

        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_IN, _vblank_in_handler);
        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_OUT, _vblank_out_handler);

        scu_ic_mask_chg(SCU_MASK_AND, SCU_IC_MASK_NONE);
}

void
vdp_sync(void)
{
        struct scu_dma_reg_buffer *reg_buffer;
        reg_buffer = &_state_vdp2()->commit.reg_buffer;

        int8_t ret __unused;
        ret = dma_queue_enqueue(reg_buffer, DMA_QUEUE_TAG_VBLANK_IN,
            _vdp2_dma_handler, NULL);
#ifdef DEBUG
        assert(ret == 0);
#endif /* DEBUG */

        const uint32_t scu_mask = scu_ic_mask_get();

        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_MASK_OR);

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

        _state.sync |= STATE_SYNC;

        scu_ic_mask_chg(SCU_MASK_AND, SCU_IC_MASK_NONE);

        /* Wait until VDP1 changed frame buffers and wait until VDP2 state has
         * been committed */
        while (true) {
                const bool vdp1_state_mask = STATE_VDP1_REQUEST_XFER_LIST |
                                             STATE_VDP1_CHANGED;
                const bool vdp1_working =
                    (_state.vdp1 & vdp1_state_mask) == STATE_VDP1_REQUEST_XFER_LIST;

                const bool vdp2_working =
                    (_state.vdp2 & STATE_VDP2_COMMITTED) == 0x00;

                if (!vdp1_working && !vdp2_working) {
                        break;
                }
        }

        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_MASK_OR);

        callback_list_process(_user_callback_list, true);

        _state.sync &= ~STATE_MASK;
        _state.vdp1 &= ~STATE_VDP1_MASK;
        _state.vdp2 &= ~STATE_VDP2_MASK;

        scu_ic_mask_chg(SCU_MASK_AND, SCU_IC_MASK_NONE);

        vdp1_sync_last_command_set(0);

        scu_ic_mask_set(scu_mask);
}

bool
vdp1_sync_rendering(void)
{
        const uint8_t state_mask = STATE_VDP1_REQUEST_COMMIT_LIST |
                                   STATE_VDP1_LIST_COMMITTED;

        return (_state.vdp1 & state_mask) == STATE_VDP1_REQUEST_COMMIT_LIST;
}

void
vdp1_sync_interval_set(const int8_t interval)
{
        uint8_t mode;

        /* XXX: This should take into account PAL systems */
        if ((interval == VDP1_SYNC_INTERVAL_60HZ) || (interval > 60)) {
                mode = STATE_VDP1_MODE_AUTO;
        } else if (interval <= VDP1_SYNC_INTERVAL_VARIABLE) {
                mode = STATE_VDP1_MODE_VARIABLE;
        } else {
                mode = STATE_VDP1_MODE_FIXED;
        }

        _state.vdp1 &= ~STATE_VDP1_MODE_MASK;
        _state.vdp1 |= mode;

        _current_vdp1_mode = &_vdp1_mode_table[mode];
}

void
vdp1_sync_cmdt_put(const struct vdp1_cmdt *cmdts, const uint16_t count,
    void (*callback)(void *), void *work)
{
#ifdef DEBUG
        assert(cmdts != NULL);
#endif /* DEBUG */

        if (count == 0) {
                return;
        }

        const vdp1_sync_put_args_t args = {
                .transfer_type = TRANSFER_TYPE_BUFFER,
                .cmdts = cmdts,
                .count = count,
                .callback = callback,
                .work = work
        };

        _vdp1_sync_put_call(&args);
}

void
vdp1_sync_cmdt_list_put(const struct vdp1_cmdt_list *cmdt_list,
    void (*callback)(void *), void *work)
{
#ifdef DEBUG
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);
#endif /* DEBUG */

        vdp1_sync_cmdt_put(cmdt_list->cmdts, cmdt_list->count, callback, work);
}

void
vdp1_sync_cmdt_orderlist_put(const struct vdp1_cmdt_orderlist *cmdt_orderlist,
    void (*callback)(void *), void *work)
{
#ifdef DEBUG
        assert(cmdt_orderlist != NULL);
#endif /* DEBUG */

        const vdp1_sync_put_args_t args = {
                .transfer_type = TRANSFER_TYPE_ORDERLIST,
                .cmdt_orderlist = cmdt_orderlist,
                .callback = callback,
                .work = work
        };

        _vdp1_sync_put_call(&args);
}

uint16_t
vdp1_sync_last_command_get(void)
{
        return _vdp1_last_command;
}

void
vdp1_sync_last_command_set(const uint16_t index)
{
        _vdp1_last_command = index;
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
        callback_init(&_user_vdp1_sync_callback);

        scu_ic_ihr_set(SCU_IC_INTERRUPT_SPRITE_END, _sprite_end_handler);

        vdp1_sync_interval_set(VDP1_SYNC_INTERVAL_60HZ);

        vdp1_sync_last_command_set(0);
}

static inline void __always_inline
_vdp1_sync_put_call(const void *args_ptr)
{
        _current_vdp1_mode->sync_put(args_ptr);
}

static inline void __always_inline
_vdp1_dma_call(const void *args_ptr)
{
        _current_vdp1_mode->dma(args_ptr);
}

static inline void __always_inline
_vdp1_sprite_end_call(const void *args_ptr)
{
        _current_vdp1_mode->sprite_end(args_ptr);
}

static inline void __always_inline
_vdp1_vblank_in_call(const void *args_ptr)
{
        _current_vdp1_mode->vblank_in(args_ptr);
}

static inline void __always_inline
_vdp1_vblank_out_call(const void *args_ptr)
{
        _current_vdp1_mode->vblank_out(args_ptr);
}

static void
_vdp1_mode_auto_sync_put(const void *args_ptr)
{
        const vdp1_sync_put_args_t *args = args_ptr;

        /* Wait until the previous command table list transfer is done */
        if ((_state.vdp1 & STATE_VDP1_REQUEST_XFER_LIST) != 0x00) {
                while ((_state.vdp1 & STATE_VDP1_LIST_XFERRED) == 0x00) {
                }
        }

        const uint32_t scu_mask = scu_ic_mask_get();

        scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_MASK_OR);
        _state.vdp1 &= ~STATE_VDP1_MASK;
        _state.vdp1 |= STATE_VDP1_REQUEST_XFER_LIST;
        scu_ic_mask_set(scu_mask);

        const uint32_t vdp1_vram =
            VDP1_VRAM(vdp1_sync_last_command_get() * sizeof(struct vdp1_cmdt));

        switch (args->transfer_type) {
        case TRANSFER_TYPE_BUFFER:
                _vdp1_cmdt_transfer(args->cmdts, vdp1_vram, args->count);
                break;
        case TRANSFER_TYPE_ORDERLIST:
                _vdp1_cmdt_orderlist_transfer(args->cmdt_orderlist);
                break;
        }
}

static void
_vdp1_mode_auto_dma(const void *args_ptr __unused)
{
        /* Don't clear STATE_VDP1_REQUEST_XFER_LIST as we want to know that
         * we've requested to transfer a command list */
        _state.vdp1 |= STATE_VDP1_LIST_XFERRED;
}

static void
_vdp1_mode_auto_sprite_end(const void *args_ptr __unused)
{
        _state.vdp1 |= STATE_VDP1_LIST_COMMITTED;

        callback_call(&_user_vdp1_sync_callback);
}

static void
_vdp1_mode_auto_vblank_in(const void *args_ptr __unused)
{
        uint8_t state_vdp1;
        state_vdp1 = _state.vdp1;

        if ((state_vdp1 & STATE_VDP1_REQUEST_XFER_LIST) == 0x00) {
                return;
        }

        /* If we're still transferring, then abort */
        if ((state_vdp1 & STATE_VDP1_LIST_XFERRED) == 0x00) {
                assert(false && "Exceeded transfer time");
                return;
        }

        _state.vdp1 |= STATE_VDP1_REQUEST_COMMIT_LIST;
        _state.vdp1 |= STATE_VDP1_REQUEST_CHANGE;

        /* Going from manual to 1-cycle mode requires the FCM and FCT
         * bits to be cleared. Otherwise, we get weird behavior from the
         * VDP1.
         *
         * However, VDP1(FBCR) must not be entirely cleared. This caused
         * a lot of glitching when in double-density interlace mode */

        MEMORY_WRITE(16, VDP1(FBCR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0002);
}

static void
_vdp1_mode_auto_vblank_out(const void *args_ptr __unused)
{
        if ((_state.vdp1 & STATE_VDP1_REQUEST_CHANGE) == 0x00) {
                return;
        }

        _state.vdp1 &= ~STATE_VDP1_REQUEST_COMMIT_LIST;
        _state.vdp1 &= ~STATE_VDP1_REQUEST_CHANGE;

        _state.vdp1 |= STATE_VDP1_LIST_COMMITTED;
        _state.vdp1 |= STATE_VDP1_CHANGED;
}

static void
_vdp1_mode_fixed_sync_put(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_fixed_dma(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_fixed_sprite_end(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_fixed_vblank_in(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_fixed_vblank_out(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_variable_sync_put(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_variable_dma(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_variable_sprite_end(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_variable_vblank_in(const void *args_ptr __unused)
{
}

static void
_vdp1_mode_variable_vblank_out(const void *args_ptr __unused)
{
}

static void
_vdp1_cmdt_transfer(const struct vdp1_cmdt *cmdts,
    const uint32_t vdp1_vram,
    const uint16_t count)
{
        /* Keep track of how many commands are being sent.
         *
         * Remove the "draw end" command from the count as it needs to be
         * overwritten on next request to sync VDP1 */
        const uint16_t last_command = vdp1_sync_last_command_get();
        vdp1_sync_last_command_set(last_command + (count - 1));

        const uint32_t xfer_len = count * sizeof(struct vdp1_cmdt);
        const uint32_t xfer_dst = vdp1_vram;
        const uint32_t xfer_src = (uint32_t)cmdts;

#if DEBUG_DMA_QUEUE_ENABLE == 1
        static struct scu_dma_level_cfg dma_cfg = {
                .mode = SCU_DMA_MODE_DIRECT,
                .xfer.direct.len = 0x00000000,
                .xfer.direct.dst = 0x00000000,
                .xfer.direct.src = 0x00000000,
                .stride = SCU_DMA_STRIDE_2_BYTES,
                .update = SCU_DMA_UPDATE_NONE
        };

        static struct scu_dma_reg_buffer dma_reg_buffer;

        dma_cfg.xfer.direct.len = xfer_len;
        dma_cfg.xfer.direct.dst = xfer_dst;
        dma_cfg.xfer.direct.src = CPU_CACHE_THROUGH | xfer_src;

        scu_dma_config_buffer(&dma_reg_buffer, &dma_cfg);

        int8_t ret __unused;
        ret = dma_queue_enqueue(&dma_reg_buffer, DMA_QUEUE_TAG_IMMEDIATE,
            _vdp1_dma_handler, NULL);
#ifdef DEBUG
        assert(ret == 0);
#endif /* DEBUG */

        ret = dma_queue_flush(DMA_QUEUE_TAG_IMMEDIATE);
#ifdef DEBUG
        assert(ret >= 0);
#endif /* DEBUG */
#else
        (void)memcpy((void *)xfer_dst, (void *)xfer_src, xfer_len);

        /* Call handler directly */
        _vdp1_dma_call(NULL);
#endif /* DEBUG_DMA_QUEUE_ENABLE */
}

static void
_vdp1_cmdt_orderlist_transfer(const struct vdp1_cmdt_orderlist *cmdt_orderlist)
{
        /* Reset it. We don't have any control over where in VRAM command tables
         * are being sent */
        vdp1_sync_last_command_set(0);

        const struct scu_dma_xfer *xfer_table __unused =
            (const struct scu_dma_xfer *)&cmdt_orderlist;

#if DEBUG_DMA_QUEUE_ENABLE == 1
        assert(false && "Not yet implemented");
#else
        const struct scu_dma_xfer *current_xfer = xfer_table;

        while ((current_xfer->len & SCU_DMA_INDIRECT_TBL_END) != 0x00000000) {
                const uint32_t xfer_len =
                    (current_xfer->len & ~SCU_DMA_INDIRECT_TBL_END) *
                    sizeof(struct vdp1_cmdt);
                const uint32_t xfer_dst = current_xfer->dst;
                const uint32_t xfer_src = current_xfer->src;

                (void)memcpy((void *)xfer_dst, (void *)xfer_src, xfer_len);

                current_xfer++;
        }

        /* Call handler directly */
        _vdp1_dma_call(NULL);
#endif /* DEBUG_DMA_QUEUE_ENABLE */
}

static void
_sprite_end_handler(void)
{
        _vdp1_sprite_end_call(NULL);
}

static void
_vdp1_dma_handler(const struct dma_queue_transfer *transfer)
{
        if ((transfer != NULL) && (transfer->status != DMA_QUEUE_STATUS_COMPLETE)) {
                return;
        }

        _vdp1_dma_call(NULL);
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

static void
_vdp2_dma_handler(const struct dma_queue_transfer *transfer)
{
        if ((transfer != NULL) && (transfer->status != DMA_QUEUE_STATUS_COMPLETE)) {
                return;
        }

        _state.vdp2 |= STATE_VDP2_COMMITTED;
        _state.vdp2 &= ~STATE_VDP2_COMITTING;
}

static void
_vblank_in_handler(void)
{
        /* VBLANK-IN interrupt runs at scanline #224 */
        if ((_state.sync & STATE_SYNC) == 0x00) {
                goto no_sync;
        }

        _vdp1_vblank_in_call(NULL);

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

        _vdp1_vblank_out_call(NULL);

no_sync:
        callback_call(&_user_vblank_out_callback);
}
