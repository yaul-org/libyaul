/* #define VDP_SYNC_DEBUG */
/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include <gdb.h>

#include <cpu/cache.h>
#include <cpu/dmac.h>
#include <cpu/intc.h>

#include <scu/ic.h>

#include <vdp.h>

#include <sys/dma-queue.h>

#include "vdp-internal.h"

/* CPU-DMAC channel used for vdp2_sync() */
#define SYNC_DMAC_CHANNEL       0

/* Maximum number of user callbacks */
#define USER_CALLBACK_COUNT     4
#define SCU_MASK_OR     (SCU_IC_MASK_VBLANK_IN |                               \
                         SCU_IC_MASK_VBLANK_OUT |                              \
                         SCU_IC_MASK_LEVEL_0_DMA_END)
#define SCU_MASK_AND    (SCU_IC_MASK_ALL & ~SCU_MASK_OR)

#define SYNC_FLAG_VDP1_SYNC             (1 << 0) /* Request to synchronize VDP1 */
#define SYNC_FLAG_VDP2_SYNC             (1 << 1) /* Request to synchronize VDP1 */
#define SYNC_FLAG_INTERLACE_SINGLE      (1 << 2)
#define SYNC_FLAG_INTERLACE_DOUBLE      (1 << 3)
#define SYNC_FLAG_MASK                  (0x0F)

#define VDP1_INTERVAL_MODE_AUTO         (0x00)
#define VDP1_INTERVAL_MODE_FIXED        (0x01)
#define VDP1_INTERVAL_MODE_VARIABLE     (0x02)
#define VDP1_INTERVAL_MODE_MASK         (0x03)

#define VDP1_FB_MODE_ERASE_CHANGE       (0x00)
#define VDP1_FB_MODE_CHANGE_ONLY        (0x01)
#define VDP1_FB_MODE_MASK               (0x01)

#define VDP1_FLAG_IDLE                  (0x00)
#define VDP1_FLAG_REQUEST_XFER_LIST     (1 << 0) /* VDP1 request to transfer list */
#define VDP1_FLAG_REQUEST_COMMIT_LIST   (1 << 1) /* VDP1 request to commit list */
#define VDP1_FLAG_REQUEST_CHANGE        (1 << 2) /* VDP1 request to change frame buffers */
#define VDP1_FLAG_LIST_XFERRED          (1 << 3) /* VDP1 finished transferring list via SCU-DMA */
#define VDP1_FLAG_LIST_COMMITTED        (1 << 4) /* VDP1 finished committing list */
#define VDP1_FLAG_CHANGED               (1 << 5) /* VDP1 changed frame buffers */
#define VDP1_FLAG_MASK                  (0x3F)

#define VDP2_FLAG_IDLE                  (0x00)
#define VDP2_FLAG_REQUEST_COMMIT        (1 << 0) /* VDP2 request to commit state */
#define VDP2_FLAG_COMITTING             (1 << 1) /* VDP2 is committing state via SCU-DMA */
#define VDP2_FLAG_COMMITTED             (1 << 2) /* VDP2 finished committing state */
#define VDP2_FLAG_MASK                  (0x07)

#define TRANSFER_TYPE_BUFFER            (0)
#define TRANSFER_TYPE_ORDERLIST         (1)

#define VDP1_TVMR_VBE                   (0x0008)

#define VDP1_FBCR_NONE                  (0x0000)
#define VDP1_FBCR_FCT                   (0x0001) /* Setting prohibited */
#define VDP1_FBCR_FCM                   (0x0002) /* Erase in the next field */
#define VDP1_FBCR_FCM_FCT               (0x0003) /* Change in next field */
#define VDP1_FBCR_DIL                   (0x0004)
#define VDP1_FBCR_DIE                   (0x0008)
#define VDP1_FBCR_EOS                   (0x0010)

#define VDP1_PTMR_IDLE                  (0x0000)
#define VDP1_PTMR_PLOT                  (0x0001)
#define VDP1_PTMR_AUTO                  (0x0002)

#ifdef VDP_SYNC_DEBUG
#include <dbgio.h>

#define DEBUG_PRINTF(...) do {                                                 \
        dbgio_printf(__VA_ARGS__);                                             \
        dbgio_flush();                                                         \
} while(false)
#else
#define DEBUG_PRINTF(...)
#endif /* VDP_SYNC_DEBUG */

typedef void (*vdp1_func_ptr)(const void *);

typedef struct {
        uint8_t transfer_type;
        const vdp1_cmdt_orderlist_t *cmdt_orderlist;
        const vdp1_cmdt_t *cmdts;
        const uint16_t index;
        const uint16_t count;
        vdp1_sync_callback_t callback;
        void *work;
} __aligned(4) vdp1_sync_put_args_t;

struct vdp1_state {
        unsigned int interval_mode:3;
        unsigned int previous_fb_mode:2;
        unsigned int fb_mode:2;
        unsigned int field_count:1;
        unsigned int flags:6;
} __packed __aligned(4);

struct vdp2_state {
        uint8_t flags;
} __packed __aligned(4);

static volatile struct {
        struct vdp1_state vdp1;
        struct vdp2_state vdp2;
        uint8_t flags;
} _state __aligned(4);

static_assert(sizeof(struct vdp1_state) == 4);
static_assert(sizeof(struct vdp2_state) == 4);
static_assert(sizeof(_state) == 12);

static scu_dma_handle_t _vdp1_dma_handle;
static scu_dma_handle_t _vdp1_orderlist_dma_handle;

static callback_t _user_vdp1_sync_callback;
static callback_t _user_vblank_in_callback;
static callback_t _user_vblank_out_callback;

static callback_list_t *_user_callback_list;

static const uint16_t _fbcr_bits[] __unused = {
        /* Render even-numbered lines */
        0x0008,
        /* Render odd-numbered lines */
        0x000C
};

static void _vdp1_mode_auto_sync_put(const void *args_ptr);
static void _vdp1_mode_auto_dma(const void *args_ptr);
static void _vdp1_mode_auto_sprite_end(const void *args_ptr);
static void _vdp1_mode_auto_vblank_in(const void *args_ptr);
static void _vdp1_mode_auto_vblank_out(const void *args_ptr);

static void _vdp1_mode_fixed_sync_put(const void *args_ptr);
static void _vdp1_mode_fixed_dma(const void *args_ptr);
static void _vdp1_mode_fixed_sprite_end(const void *args_ptr);
static void _vdp1_mode_fixed_vblank_in(const void *args_ptr);
static void _vdp1_mode_fixed_vblank_out(const void *args_ptr);

/* All state is kept in struct vdp1_state, and accessed via _state.vdp1.
 *
 * 1. When a command table list (vdp1_cmdt_list) is "put" via
 *    vdp1_sync_cmdt_list_put(), a transfer is initiated. The function blocks
 *    until the transfer is complete. VDP1 plotting starts.
 *
 * 2. When vdp_sync() is called, SYNC_FLAG_VDP1_SYNC is set.
 *
 * 3. At VBLANK-IN, a comparison of VDP1(COPR) and the number of command tables
 *    transferred is performed. If not equal, the VBLANK-IN interrupt handler
 *    returns until the next time it's fired.
 *
 * 4. When VDP1(COPR) is equal to the number of command tables transferred, then
 *    VDP1_FLAG_REQUEST_CHANGE is set. VDP1(TVMR) sets VBE, and sets FCM|FCT to
 *    VDP1(FBCR).
 *
 * 5. In VBLANK-OUT, if the request to change framebuffers is set
 *    (VDP1_FLAG_REQUEST_CHANGE), then clear VBE from VDP1(TVMR), and wait until
 *    scanline #0 is reached. Waiting until scanline #0 is reached should ensure
 *    that the VDP1 has enough time to fully change framebuffers. If plotting
 *    occurs (for another transfer), then the VDP1 will abort the plot,
 *    resulting in a soft lock. All state is reset at this point.
 *
 * 6. vdp1_sync_wait() blocks until SYNC_FLAG_VDP1_SYNC is cleared. */
static void _vdp1_mode_variable_sync_put(const void *args_ptr);
static void _vdp1_mode_variable_dma(const void *args_ptr);
static void _vdp1_mode_variable_sprite_end(const void *args_ptr);
static void _vdp1_mode_variable_vblank_in(const void *args_ptr);
static void _vdp1_mode_variable_vblank_out(const void *args_ptr);

/* Interface to either of the three modes */
static const struct {
        vdp1_func_ptr sync_put;
        vdp1_func_ptr dma;
        vdp1_func_ptr sprite_end;
        vdp1_func_ptr vblank_in;
        vdp1_func_ptr vblank_out;
} _vdp1_mode_table[] = {
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
}, *_current_vdp1_mode; /* Pointer to the current VDP1 mode */

static void _vdp1_init(void);

static inline __always_inline void _vdp1_sync_put_call(const void *);
static inline __always_inline void _vdp1_dma_call(const void *);
static inline __always_inline void _vdp1_sprite_end_call(const void *);
static inline __always_inline void _vdp1_vblank_in_call(const void *);
static inline __always_inline void _vdp1_vblank_out_call(const void *);

static void _vdp1_dma(scu_dma_handle_t *dma_handle);
static void _vdp1_cmdt_transfer(const vdp1_cmdt_t *cmdts, vdp1_vram_t vdp1_vram, uint16_t count);
static void _vdp1_cmdt_orderlist_transfer(const vdp1_cmdt_orderlist_t *cmdt_orderlist);

static void _vdp2_init(void);
static void _vdp2_registers_transfer(cpu_dmac_cfg_t *dmac_cfg);
static void _vdp2_back_screen_transfer(cpu_dmac_cfg_t *dmac_cfg);

static void _vblank_in_handler(void);
static void _vblank_out_handler(void);
static void _vdp2_dma_handler(const dma_queue_transfer_t *transfer);

void
_internal_vdp_sync_init(void)
{
        const uint32_t intc_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        _user_callback_list = callback_list_alloc(USER_CALLBACK_COUNT);

        callback_init(&_user_vblank_in_callback);
        callback_init(&_user_vblank_out_callback);

        _state.flags = 0x00;

        _state.vdp1.flags = VDP1_FLAG_IDLE;
        _state.vdp1.field_count = 0;
        _state.vdp1.previous_fb_mode = VDP1_FB_MODE_ERASE_CHANGE;
        _state.vdp1.fb_mode = VDP1_FB_MODE_ERASE_CHANGE;

        _state.vdp1.interval_mode = VDP1_INTERVAL_MODE_AUTO;
        _state.vdp2.flags = VDP2_FLAG_IDLE;

        _vdp1_init();
        _vdp2_init();

        vdp_sync_vblank_in_clear();
        vdp_sync_vblank_out_clear();

        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_IN, _vblank_in_handler);
        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_OUT, _vblank_out_handler);

        scu_ic_mask_chg(SCU_MASK_AND, SCU_IC_MASK_NONE);

        cpu_intc_mask_set(intc_mask);
}

void
vdp1_sync(void)
{
        DEBUG_PRINTF("vdp1_sync: Enter\n");

        if ((_state.flags & SYNC_FLAG_VDP1_SYNC) == SYNC_FLAG_VDP1_SYNC) {
                return;
        }

        if ((_state.vdp1.flags & VDP1_FLAG_REQUEST_XFER_LIST) != VDP1_FLAG_REQUEST_XFER_LIST) {
                return;
        }

        const uint32_t intc_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        _state.flags |= SYNC_FLAG_VDP1_SYNC;

        cpu_intc_mask_set(intc_mask);
}

void
vdp1_sync_wait(void)
{
        while ((_state.flags & SYNC_FLAG_VDP1_SYNC) == SYNC_FLAG_VDP1_SYNC) {
        }
}

void
vdp2_sync(void)
{
        DEBUG_PRINTF("vdp2_sync: Enter\n");

        if ((_state.flags & (SYNC_FLAG_VDP2_SYNC)) == SYNC_FLAG_VDP2_SYNC) {
                return;
        }

        const uint32_t intc_mask = cpu_intc_mask_get();

        /* Wait for DMA queue to finish flushing, prior to syncing */
        DEBUG_PRINTF("Flush #1\n");
        dma_queue_flush_wait();

        cpu_intc_mask_set(15);

        scu_dma_handle_t *handle;
        handle = _state_vdp2()->commit.handle;

        int8_t ret __unused;
        ret = dma_queue_enqueue(handle, DMA_QUEUE_TAG_VBLANK_IN,
            _vdp2_dma_handler, NULL);
        assert(ret == 0);

        _state.flags &= ~SYNC_FLAG_INTERLACE_SINGLE;
        _state.flags &= ~SYNC_FLAG_INTERLACE_DOUBLE;

        switch ((_state_vdp2()->regs->tvmd >> 6) & 0x3) {
        case 0x2:
                _state.flags |= SYNC_FLAG_INTERLACE_SINGLE;
                break;
        case 0x3:
                _state.flags |= SYNC_FLAG_INTERLACE_DOUBLE;
                break;
        }

        _state.flags |= SYNC_FLAG_VDP2_SYNC;

        cpu_intc_mask_set(intc_mask);
}

void
vdp2_sync_wait(void)
{
        if ((_state.flags & SYNC_FLAG_VDP2_SYNC) != SYNC_FLAG_VDP2_SYNC) {
                return;
        }

        const uint32_t intc_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(0);

        /* Wait until VDP1 changed frame buffers and wait until VDP2 state has
         * been committed */
        DEBUG_PRINTF("Starting loop\n");

        while (true) {
                if ((_state.vdp2.flags & VDP2_FLAG_REQUEST_COMMIT) != 0x00) {
                        DEBUG_PRINTF("Flushing VBLANK-IN\n");

                        _state.vdp2.flags &= ~VDP2_FLAG_REQUEST_COMMIT;
                        _state.vdp2.flags |= VDP2_FLAG_COMITTING;

                        int ret __unused;
                        ret = dma_queue_flush(DMA_QUEUE_TAG_VBLANK_IN);
                        assert(ret >= 0);
                }

                dma_queue_flush_wait();

                const bool vdp2_working =
                    (_state.vdp2.flags != VDP2_FLAG_COMMITTED);

                if (!vdp2_working) {
                        DEBUG_PRINTF("Next frame\n\n\n\n\n");
                        break;
                }
        }

        cpu_intc_mask_set(15);

        callback_list_process(_user_callback_list, /* clear = */ true);

        _state.flags &= ~SYNC_FLAG_VDP2_SYNC;
        _state.vdp2.flags &= ~VDP2_FLAG_MASK;

        cpu_intc_mask_set(intc_mask);
}

void
vdp1_sync_interval_set(const int8_t interval)
{
        uint8_t mode;

        /* Clear VBE */
        _state_vdp1()->regs->tvmr &= ~VDP1_TVMR_VBE;

        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);

        /* XXX: This should take into account PAL systems */
        if ((interval == VDP1_SYNC_INTERVAL_60HZ) || (interval > 60)) {
                mode = VDP1_INTERVAL_MODE_AUTO;

                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_NONE);
        } else if (interval <= VDP1_SYNC_INTERVAL_VARIABLE) {
                mode = VDP1_INTERVAL_MODE_VARIABLE;

                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM_FCT);
        } else {
                mode = VDP1_INTERVAL_MODE_FIXED;
        }

        _state.vdp1.interval_mode = mode;

        _current_vdp1_mode = &_vdp1_mode_table[mode];
}

vdp_sync_mode_t
vdp1_sync_mode_get(void)
{
        return _state.vdp1.fb_mode;
}

void
vdp1_sync_mode_set(vdp_sync_mode_t mode)
{
        _state.vdp1.fb_mode = mode;
}

void
vdp1_sync_cmdt_put(const vdp1_cmdt_t *cmdts, const uint16_t count,
    const uint16_t index, vdp1_sync_callback_t callback, void *work)
{
        assert(cmdts != NULL);

        if (count == 0) {
                return;
        }

        /* Check if index exceeds VRAM command table bounds */

        const vdp1_sync_put_args_t args = {
                .transfer_type = TRANSFER_TYPE_BUFFER,
                .cmdts = cmdts,
                .index = index,
                .count = count,
                .callback = callback,
                .work = work
        };

        _vdp1_sync_put_call(&args);
}

void
vdp1_sync_cmdt_list_put(const vdp1_cmdt_list_t *cmdt_list,
    const uint16_t index, vdp1_sync_callback_t callback, void *work)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);

        vdp1_sync_cmdt_put(cmdt_list->cmdts, cmdt_list->count, index, callback, work);
}

void
vdp1_sync_cmdt_orderlist_put(const vdp1_cmdt_orderlist_t *cmdt_orderlist,
    vdp1_sync_callback_t callback, void *work)
{
        assert(cmdt_orderlist != NULL);

        const vdp1_sync_put_args_t args = {
                .transfer_type = TRANSFER_TYPE_ORDERLIST,
                .cmdt_orderlist = cmdt_orderlist,
                .callback = callback,
                .work = work
        };

        _vdp1_sync_put_call(&args);
}

void
vdp2_sync_commit(void)
{
        static cpu_dmac_cfg_t dmac_cfg = {
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

        _state.vdp2.flags &= ~VDP2_FLAG_COMMITTED;
        _state.vdp2.flags |= VDP2_FLAG_COMITTING;

        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
        cpu_dmac_enable();

        cpu_cache_purge();

        _vdp2_back_screen_transfer(&dmac_cfg);
        _vdp2_registers_transfer(&dmac_cfg);

        _state.vdp2.flags |= VDP2_FLAG_COMMITTED;
}

void
vdp_sync_vblank_in_set(vdp_sync_callback_t callback)
{
        callback_set(&_user_vblank_in_callback, callback, NULL);
}

void
vdp_sync_vblank_out_set(vdp_sync_callback_t callback)
{
        callback_set(&_user_vblank_out_callback, callback, NULL);
}

callback_id_t
vdp_sync_user_callback_add(vdp_sync_callback_t callback, void *work)
{
        return callback_list_callback_add(_user_callback_list, callback, work);
}

void
vdp_sync_user_callback_remove(callback_id_t id)
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
        const scu_dma_level_cfg_t dma_cfg = {
                .mode = SCU_DMA_MODE_DIRECT,
                .xfer.direct.len = 0xFFFFFFFF,
                .xfer.direct.dst = 0xFFFFFFFF,
                .xfer.direct.src = 0xFFFFFFFF,
                .stride = SCU_DMA_STRIDE_2_BYTES,
                .update = SCU_DMA_UPDATE_NONE
        };

        const scu_dma_level_cfg_t orderlist_dma_cfg = {
                .mode = SCU_DMA_MODE_INDIRECT,
                .xfer.indirect = (void *)0xFFFFFFFF,
                .stride = SCU_DMA_STRIDE_2_BYTES,
                .update = SCU_DMA_UPDATE_NONE
        };

        callback_init(&_user_vdp1_sync_callback);

        vdp1_sync_mode_set(VDP1_SYNC_MODE_ERASE_CHANGE);
        vdp1_sync_interval_set(VDP1_SYNC_INTERVAL_60HZ);

        scu_dma_config_buffer(&_vdp1_dma_handle, &dma_cfg);
        scu_dma_config_buffer(&_vdp1_orderlist_dma_handle, &orderlist_dma_cfg);
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

        DEBUG_PRINTF("sync_put: Enter\n");

        /* Mask interrupts to make sure this variable does not get modified at
         * the same time */
        const uint32_t intc_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        _state.vdp1.flags &= ~VDP1_FLAG_MASK;
        _state.vdp1.flags |= VDP1_FLAG_REQUEST_XFER_LIST;

        cpu_intc_mask_set(intc_mask);

        const vdp1_vram_t vdp1_vram = VDP1_VRAM(args->index);

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
        /* Don't clear VDP1_FLAG_REQUEST_XFER_LIST as we want to know that we've
         * requested to transfer a command list.
         *
         * This is important as there are cases where we don't transfer anything
         * to the VDP1, but still expect to sync */
        _state.vdp1.flags |= VDP1_FLAG_LIST_XFERRED;
}

static void
_vdp1_mode_auto_sprite_end(const void *args_ptr __unused)
{
        _state.vdp1.flags |= VDP1_FLAG_LIST_COMMITTED;

        callback_call(&_user_vdp1_sync_callback);
}

static void
_vdp1_mode_auto_vblank_in(const void *args_ptr __unused)
{
        uint8_t state_vdp1_flags = _state.vdp1.flags;

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_XFER_LIST) == 0x00) {
                return;
        }

        /* If we're still transferring, then abort */
        if ((state_vdp1_flags & VDP1_FLAG_LIST_XFERRED) == 0x00) {
                assert(false && "Exceeded transfer time");

                return;
        }

        state_vdp1_flags |= VDP1_FLAG_REQUEST_COMMIT_LIST;
        state_vdp1_flags |= VDP1_FLAG_REQUEST_CHANGE;

        _state.vdp1.flags = state_vdp1_flags;

        /* Going from manual to 1-cycle mode requires the FCM and FCT
         * bits to be cleared. Otherwise, we get weird behavior from the
         * VDP1.
         *
         * However, VDP1(FBCR) must not be entirely cleared. This caused
         * a lot of glitching when in double-density interlace mode */

        MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_NONE);
        MEMORY_WRITE(16, VDP1(PTMR), VDP1_PTMR_IDLE);
        MEMORY_WRITE(16, VDP1(PTMR), VDP1_PTMR_AUTO);
}

static void
_vdp1_mode_auto_vblank_out(const void *args_ptr __unused)
{
        uint8_t state_vdp1_flags = _state.vdp1.flags;

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_CHANGE) == 0x00) {
                return;
        }

        state_vdp1_flags &= ~VDP1_FLAG_REQUEST_CHANGE;

        state_vdp1_flags |= VDP1_FLAG_LIST_COMMITTED;
        state_vdp1_flags |= VDP1_FLAG_CHANGED;

        _state.vdp1.flags = state_vdp1_flags;

        switch (_state.vdp1.fb_mode) {
        case VDP1_FB_MODE_ERASE_CHANGE:
                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_NONE);
                break;
        case VDP1_FB_MODE_CHANGE_ONLY:
                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM_FCT);
                break;
        }
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
_vdp1_mode_variable_sync_put(const void *args_ptr)
{
        _vdp1_mode_auto_sync_put(args_ptr);
}

static void
_vdp1_mode_variable_dma(const void *args_ptr __unused)
{
        DEBUG_PRINTF("vdp1_dma_handler: Enter\n");

        /* Since the DMA transfer went through, the VDP1 is idling, so start
         * drawing */
        MEMORY_WRITE(16, VDP1(PTMR), VDP1_PTMR_PLOT);

        uint8_t state_vdp1_flags = _state.vdp1.flags;

        /* Set the flags as transfered and request to draw the list */
        state_vdp1_flags |= VDP1_FLAG_REQUEST_COMMIT_LIST;
        state_vdp1_flags |= VDP1_FLAG_LIST_XFERRED;

        _state.vdp1.flags = state_vdp1_flags;
}

static void
_vdp1_mode_variable_sprite_end(const void *args_ptr __unused)
{
        DEBUG_PRINTF("sprite_end_handler: Enter\n");

        _state.vdp1.flags |= VDP1_FLAG_LIST_COMMITTED;

        callback_call(&_user_vdp1_sync_callback);
}

static void
_vdp1_mode_variable_vblank_in(const void *args_ptr __unused)
{
        if ((_state.flags & SYNC_FLAG_VDP1_SYNC) != SYNC_FLAG_VDP1_SYNC) {
                return;
        }

        /* Cache the state to avoid multiple loads */
        uint8_t state_vdp1_flags = _state.vdp1.flags;

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_COMMIT_LIST) != VDP1_FLAG_REQUEST_COMMIT_LIST) {
                return;
        }

        if ((state_vdp1_flags & VDP1_FLAG_LIST_COMMITTED) == VDP1_FLAG_LIST_COMMITTED) {
                return;
        }

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_CHANGE) == VDP1_FLAG_REQUEST_CHANGE) {
                return;
        }

        /* HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK HACK */
        volatile uint32_t *dnc =  &_vdp1_dma_handle.dnc;
        const uint16_t cmdt_count = *dnc / sizeof(vdp1_cmdt_t);

        if ((vdp1_cmdt_current_get()) < (cmdt_count - 1)) {
                return;
        }

        *dnc = 0;

        _vdp1_sprite_end_call(NULL);

        state_vdp1_flags |= VDP1_FLAG_REQUEST_CHANGE;

        /* Change and erase on next field */
        _state_vdp1()->regs->tvmr |= VDP1_TVMR_VBE;

        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);
        MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM_FCT);

        DEBUG_PRINTF("VBLANK-IN,TVMR=$8,FBCR=$3\n");

        _state.vdp1.flags = state_vdp1_flags;
}

static void
_vdp1_mode_variable_vblank_out(const void *args_ptr __unused)
{
        if ((_state.flags & SYNC_FLAG_VDP1_SYNC) != SYNC_FLAG_VDP1_SYNC) {
                return;
        }

        uint8_t state_vdp1_flags = _state.vdp1.flags;

        DEBUG_PRINTF("VBLANK-OUT,vdp1: RX%i RL%i RC%i  X%i L%i C%i, vdp2: 0x%02X\n",
            (state_vdp1_flags >> 0) & 1,
            (state_vdp1_flags >> 1) & 1,
            (state_vdp1_flags >> 2) & 1,
            (state_vdp1_flags >> 3) & 1,
            (state_vdp1_flags >> 4) & 1,
            (state_vdp1_flags >> 5) & 1,
            _state.vdp2.flags);

        if ((state_vdp1_flags & VDP1_FLAG_CHANGED) == VDP1_FLAG_CHANGED) {
                return;
        }

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_CHANGE) != VDP1_FLAG_REQUEST_CHANGE) {
                DEBUG_PRINTF("VBLANK-OUT,!VDP1_FLAG_REQUEST_CHANGE\n");;
                return;
        }

        state_vdp1_flags &= ~VDP1_FLAG_REQUEST_CHANGE;

        /* Clear VBE */
        _state_vdp1()->regs->tvmr &= ~VDP1_TVMR_VBE;

        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);

        /* This could be a hack, but wait until scanline #0 is reached to avoid
         * the frame buffer change from aborting plotting, resulting in a soft
         * lock */
        while (true) {
                vdp2_tvmd_extern_latch();

                const uint16_t vcount = vdp2_tvmd_vcount_get();

                if (vcount == 0) {
                        break;
                }
        }

        _state.flags &= ~SYNC_FLAG_VDP1_SYNC;
        state_vdp1_flags &= ~VDP1_FLAG_MASK;

        _state.vdp1.flags = state_vdp1_flags;

        DEBUG_PRINTF("VBLANK-OUT,TVMR=$0\n");
}

static void
_vdp1_dma(scu_dma_handle_t *dma_handle)
{
        const uint32_t intc_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        scu_dma_level_wait(0);
        scu_dma_config_set(0, SCU_DMA_START_FACTOR_ENABLE, dma_handle, NULL);

        cpu_cache_purge();

        scu_dma_level_fast_start(0);
        scu_dma_level_wait(0);

        _vdp1_dma_call(NULL);

        cpu_intc_mask_set(intc_mask);
}

static void
_vdp1_cmdt_transfer(const vdp1_cmdt_t *cmdts, vdp1_vram_t vdp1_vram, uint16_t count)
{
        _vdp1_dma_handle.dnr = CPU_CACHE_THROUGH | (uint32_t)cmdts;
        _vdp1_dma_handle.dnw = vdp1_vram;
        _vdp1_dma_handle.dnc = count * sizeof(vdp1_cmdt_t);

        _vdp1_dma(&_vdp1_dma_handle);
}

static void
_vdp1_cmdt_orderlist_transfer(const vdp1_cmdt_orderlist_t *cmdt_orderlist)
{
        _vdp1_orderlist_dma_handle.dnw = CPU_CACHE_THROUGH | (uint32_t)cmdt_orderlist;

        _vdp1_dma(&_vdp1_orderlist_dma_handle);
}

static void
_vdp2_init(void)
{
        extern void _internal_vdp2_xfer_table_init(void);

        _internal_vdp2_xfer_table_init();
}

static void
_vdp2_registers_transfer(cpu_dmac_cfg_t *dmac_cfg)
{
        /* Skip committing the first 7 VDP2 registers:
         * 0x0000 TVMD
         * 0x0002 EXTEN
         * 0x0004 TVSTAT R/O
         * 0x0006 VRSIZE R/W
         * 0x0008 HCNT   R/O
         * 0x000A VCNT   R/O
         * 0x000C Reserved
         * 0x000E RAMCTL */

        dmac_cfg->len = sizeof(vdp2_registers_t) - 14;
        dmac_cfg->dst = VDP2(0x000E);
        dmac_cfg->src = (uint32_t)&_state_vdp2()->regs->buffer[7];

        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
        cpu_dmac_channel_config_set(dmac_cfg);

        MEMORY_WRITE(16, VDP2(0x0000), _state_vdp2()->regs->tvmd);

        cpu_dmac_channel_start(SYNC_DMAC_CHANNEL);

        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
}

static void
_vdp2_back_screen_transfer(cpu_dmac_cfg_t *dmac_cfg)
{
        dmac_cfg->len = _state_vdp2()->back.count * sizeof(color_rgb1555_t);
        dmac_cfg->dst = (uint32_t)_state_vdp2()->back.vram;
        dmac_cfg->src = (uint32_t)_state_vdp2()->back.buffer;

        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
        cpu_dmac_channel_config_set(dmac_cfg);

        cpu_dmac_channel_start(SYNC_DMAC_CHANNEL);

        cpu_dmac_channel_wait(SYNC_DMAC_CHANNEL);
}

static void
_vdp2_dma_handler(const dma_queue_transfer_t *transfer)
{
        if ((transfer->status & DMA_QUEUE_STATUS_COMPLETE) != DMA_QUEUE_STATUS_COMPLETE) {
                return;
        }

        DEBUG_PRINTF("vdp2_dma_handler: Enter\n");

        _state.vdp2.flags |= VDP2_FLAG_COMMITTED;
        _state.vdp2.flags &= ~VDP2_FLAG_COMITTING;
}

static void
_vblank_in_handler(void)
{
        /* VBLANK-IN interrupt runs at scanline #224 */

        if ((_state.flags & (SYNC_FLAG_VDP1_SYNC)) == SYNC_FLAG_VDP1_SYNC) {
                DEBUG_PRINTF("VBLANK-IN, SYNC_FLAG_VDP1_SYNC\n");

                _vdp1_vblank_in_call(NULL);
        }

        if ((_state.flags & (SYNC_FLAG_VDP2_SYNC)) == SYNC_FLAG_VDP2_SYNC) {
                /* Because SYNC_FLAG_SYNC is set, we request to commit */
                if (_state.vdp2.flags == VDP2_FLAG_IDLE) {
                        _state.vdp2.flags |= VDP2_FLAG_REQUEST_COMMIT;
                }
        }

        callback_call(&_user_vblank_in_callback);
}

static void
_vblank_out_handler(void)
{
        /* VBLANK-OUT interrupt runs at scanline #511 */

        if ((_state.flags & (SYNC_FLAG_VDP1_SYNC)) == SYNC_FLAG_VDP1_SYNC) {
                DEBUG_PRINTF("VBLANK-OUT, SYNC_FLAG_SYNC\n");

                _vdp1_vblank_out_call(NULL);
        }

        callback_call(&_user_vblank_out_callback);
}
