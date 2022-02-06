/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include <cpu/cache.h>
#include <cpu/dmac.h>
#include <cpu/intc.h>

#include <scu/ic.h>

#include <vdp.h>

#include <sys/dma-queue.h>

#include "vdp-internal.h"

/* #define VDP_SYNC_DEBUG */

#define SCU_MASK_MASK   (SCU_IC_MASK_VBLANK_IN |                               \
                         SCU_IC_MASK_VBLANK_OUT |                              \
                         SCU_IC_MASK_LEVEL_0_DMA_END |                         \
                         SCU_IC_MASK_LEVEL_1_DMA_END |                         \
                         SCU_IC_MASK_LEVEL_2_DMA_END)
#define SCU_MASK_UNMASK (SCU_IC_MASK_ALL & ~SCU_MASK_MASK)

#define SYNC_FLAG_NONE                  (0x00)
#define SYNC_FLAG_VDP1_SYNC             (1 << 0) /* Request to synchronize VDP1 */
#define SYNC_FLAG_VDP1_VBLANK_IN        (1 << 1) /* Enable VDP1 sync VBLANK-IN interrupt */
#define SYNC_FLAG_VDP1_VBLANK_OUT       (1 << 2) /* Enable VDP1 sync VBLANK-OUT outterrupt */
#define SYNC_FLAG_VDP2_SYNC             (1 << 3) /* Request to synchronize VDP1 */
#define SYNC_FLAG_INTERLACE_SINGLE      (1 << 4)
#define SYNC_FLAG_INTERLACE_DOUBLE      (1 << 5)

#define SYNC_FLAG_MASK                  (0x3F)
#define SYNC_FLAG_MASK_V1SVBI           (SYNC_FLAG_VDP1_SYNC | SYNC_FLAG_VDP1_VBLANK_IN)
#define SYNC_FLAG_MASK_V1SVBO           (SYNC_FLAG_VDP1_SYNC | SYNC_FLAG_VDP1_VBLANK_OUT)

#define VDP1_INTERVAL_MODE_AUTO         (0)
#define VDP1_INTERVAL_MODE_FIXED        (1)
#define VDP1_INTERVAL_MODE_VARIABLE     (2)

#define VDP1_FB_MODE_ERASE_CHANGE       (0)
#define VDP1_FB_MODE_CHANGE_ONLY        (1)

#define VDP1_FLAG_IDLE                  (0x00)
#define VDP1_FLAG_REQUEST_XFER_LIST     (1 << 0) /* VDP1 request to transfer list */
#define VDP1_FLAG_REQUEST_COMMIT_LIST   (1 << 1) /* VDP1 request to commit list */
#define VDP1_FLAG_REQUEST_CHANGE        (1 << 2) /* VDP1 request to change frame buffers */
#define VDP1_FLAG_LIST_XFERRED          (1 << 3) /* VDP1 finished transferring list via SCU-DMA */
#define VDP1_FLAG_LIST_COMMITTED        (1 << 4) /* VDP1 finished committing list */
#define VDP1_FLAG_CHANGED               (1 << 5) /* VDP1 changed frame buffers */
#define VDP1_FLAG_MASK                  (0x3F)

#define VDP2_FLAG_IDLE                  (0x00)
#define VDP2_FLAG_REQUEST_COMMIT_REGS   (1 << 0) /* VDP2 request to commit registers */
#define VDP2_FLAG_REGS_COMMITTED        (1 << 1) /* VDP2 finished committing registers */
#define VDP2_FLAG_MASK                  (0x03)

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
#include <sys/cdefs.h>

#include <dbgio.h>

#define DEBUG_PRINTF(...) do {                                                 \
        const uint32_t _sr_mask = cpu_intc_mask_get();                         \
                                                                               \
        cpu_intc_mask_set(15);                                                 \
        dbgio_printf(__VA_ARGS__);                                             \
        dbgio_flush();                                                         \
        cpu_intc_mask_set(_sr_mask);                                           \
} while(false)
#else
#define DEBUG_PRINTF(...)
#endif /* VDP_SYNC_DEBUG */

struct vdp1_mode_table;

typedef void (*vdp1_mode_func_ptr)(void);

struct vdp1_state {
        unsigned int flags:6;
        unsigned int field_count:1;
        unsigned int interval_mode:2;
        unsigned int previous_fb_mode:1;
        unsigned int fb_mode:1;
        unsigned int frame_rate:5;
        int8_t frame_count;
        const struct vdp1_mode_table *current_mode;
} __aligned(4);

static_assert(sizeof(struct vdp1_state) == 8);

struct vdp2_state {
        uint8_t flags;
        scu_dma_level_t commit_level;
} __aligned(4);

static_assert(sizeof(struct vdp2_state) == 8);

static volatile struct {
        struct vdp1_state vdp1;
        struct vdp2_state vdp2;
        uint8_t flags;
} _state __aligned(16);

static_assert(sizeof(_state) == 20);

static scu_dma_handle_t _vdp1_dma_handle;
static scu_dma_handle_t _vdp1_orderlist_dma_handle;
static scu_dma_handle_t _vdp1_stride_dma_handle;

static callback_t _vdp1_put_callback;
static callback_t _vdp1_render_callback;
static callback_t _vdp1_transfer_over_callback;
static callback_t _vblank_in_callback;
static callback_t _vblank_out_callback;

static const uint16_t _fbcr_bits[] __unused = {
        /* Render even-numbered lines */
        0x0008,
        /* Render odd-numbered lines */
        0x000C
};

static void _vdp1_mode_auto_dma(void);
static void _vdp1_mode_auto_sync_render(void);
static void _vdp1_mode_auto_sprite_end(void);
static void _vdp1_mode_auto_vblank_in(void);
static void _vdp1_mode_auto_vblank_out(void);

static void _vdp1_mode_fixed_dma(void);
static void _vdp1_mode_fixed_sync_render(void);
static void _vdp1_mode_fixed_sprite_end(void);
static void _vdp1_mode_fixed_vblank_in(void);
static void _vdp1_mode_fixed_vblank_out(void);

/* All state is kept in struct vdp1_state, and accessed via _state.vdp1.
 *
 * 1. When a command table list (vdp1_cmdt_list) is "put" via
 *    vdp1_sync_cmdt_list_put(), a transfer is initiated. The function blocks
 *    until the transfer is complete. VDP1 plotting starts.
 *
 * 2. When vdp1_sync() is called, SYNC_FLAG_VDP1_SYNC is set.
 *
 * 3. At VBLANK-IN, a check is performed if the VDP1 is still plotting. If so,
 *    the VBLANK-IN interrupt handler returns until the next time it's fired.
 *
 * 4. When the VDP1 is done plotting, VDP1_FLAG_REQUEST_CHANGE is set.
 *    VDP1(TVMR) sets VBE, and sets FCM|FCT to VDP1(FBCR).
 *
 * 5. In VBLANK-OUT, if the request to change frame buffers is set
 *    (VDP1_FLAG_REQUEST_CHANGE), then clear VBE from VDP1(TVMR), and wait until
 *    scanline #0 is reached. Waiting until scanline #0 is reached should ensure
 *    that the VDP1 has enough time to fully change frame buffers. If plotting
 *    occurs (for another transfer), then the VDP1 will abort the plot,
 *    resulting in a soft lock. All state is reset at this point.
 *
 * 6. vdp1_sync_wait() blocks until SYNC_FLAG_VDP1_SYNC is cleared. */
static void _vdp1_mode_variable_dma(void);
static void _vdp1_mode_variable_sync_render(void);
static void _vdp1_mode_variable_sprite_end(void);
static void _vdp1_mode_variable_vblank_in(void);
static void _vdp1_mode_variable_vblank_out(void);

/* Interface to either of the three modes */
static const struct vdp1_mode_table {
        vdp1_mode_func_ptr dma;
        vdp1_mode_func_ptr sync_render;
        vdp1_mode_func_ptr sprite_end;
        vdp1_mode_func_ptr vblank_in;
        vdp1_mode_func_ptr vblank_out;
} _vdp1_mode_table[] = {
        {
                .dma         = _vdp1_mode_auto_dma,
                .sync_render = _vdp1_mode_auto_sync_render,
                .sprite_end  = _vdp1_mode_auto_sprite_end,
                .vblank_in   = _vdp1_mode_auto_vblank_in,
                .vblank_out  = _vdp1_mode_auto_vblank_out
        }, {
                .dma         = _vdp1_mode_fixed_dma,
                .sync_render = _vdp1_mode_fixed_sync_render,
                .sprite_end  = _vdp1_mode_fixed_sprite_end,
                .vblank_in   = _vdp1_mode_fixed_vblank_in,
                .vblank_out  = _vdp1_mode_fixed_vblank_out
        }, {
                .dma         = _vdp1_mode_variable_dma,
                .sync_render = _vdp1_mode_variable_sync_render,
                .sprite_end  = _vdp1_mode_variable_sprite_end,
                .vblank_in   = _vdp1_mode_variable_vblank_in,
                .vblank_out  = _vdp1_mode_variable_vblank_out
        }
};

static void _vdp1_init(void);

static inline __always_inline void _vdp1_sync_put(void);
static inline __always_inline void _vdp1_dma_call(void);
static inline __always_inline void _vdp1_sync_render_call(void);
static inline __always_inline void _vdp1_sprite_end_call(void);
static inline __always_inline void _vdp1_vblank_in_call(void);
static inline __always_inline void _vdp1_vblank_out_call(void);

static void _vdp1_dma_transfer(const scu_dma_handle_t *dma_handle);

static void _vdp2_init(void);

static void _scu_dma_level_end_handler(void *work);
static void _vblank_in_handler(void);
static void _vblank_out_handler(void);

void
__vdp_sync_init(void)
{
        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        callback_init(&_vblank_in_callback);
        callback_init(&_vblank_out_callback);

        _state.flags = SYNC_FLAG_NONE;

        _vdp1_init();
        _vdp2_init();

        vdp_sync_vblank_in_clear();
        vdp_sync_vblank_out_clear();

        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_IN, _vblank_in_handler);
        scu_ic_ihr_set(SCU_IC_INTERRUPT_VBLANK_OUT, _vblank_out_handler);

        scu_ic_mask_chg(SCU_MASK_UNMASK, SCU_IC_MASK_NONE);

        cpu_intc_mask_set(sr_mask);
}

void
vdp1_sync(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        if ((_state.flags & SYNC_FLAG_VDP1_SYNC) == SYNC_FLAG_VDP1_SYNC) {
                return;
        }

        if ((_state.vdp1.flags & VDP1_FLAG_REQUEST_XFER_LIST) != VDP1_FLAG_REQUEST_XFER_LIST) {
                return;
        }

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        _state.flags |= SYNC_FLAG_MASK_V1SVBI | SYNC_FLAG_MASK_V1SVBO;

        cpu_intc_mask_set(sr_mask);

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

bool
vdp1_sync_busy(void)
{
        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(0);

        bool busy = ((_state.flags & SYNC_FLAG_VDP1_SYNC) == SYNC_FLAG_VDP1_SYNC);

        cpu_intc_mask_set(sr_mask);

        return busy;
}

void
vdp1_sync_wait(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(0);

        while ((_state.flags & SYNC_FLAG_VDP1_SYNC) == SYNC_FLAG_VDP1_SYNC) {
        }

        cpu_intc_mask_set(sr_mask);

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

void
vdp1_sync_interval_set(int8_t interval)
{
        uint8_t mode;

        _state_vdp1()->regs->tvmr &= ~VDP1_TVMR_VBE;

        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);

        if (interval == 0) {
                mode = VDP1_INTERVAL_MODE_AUTO;

                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_NONE);
        } else if (interval < 0) {
                mode = VDP1_INTERVAL_MODE_VARIABLE;

                /* Normalize the frame rate:
                 *  0: no frame rate cap
                 *  1: cap at 30Hz
                 *  2: cap at 20Hz
                 *  3: cap at 15Hz
                 *  4: cap at 12Hz
                 *  6: cap at 10Hz */
                _state.vdp1.frame_rate = -interval - 1;
                _state.vdp1.frame_count = 0;

                vdp2_tvmd_vblank_in_next_wait(1);
                MEMORY_WRITE(16, VDP1(PTMR), VDP1_PTMR_IDLE);
                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM_FCT);
                vdp2_tvmd_vcount_wait(0);
        } else {
                mode = VDP1_INTERVAL_MODE_FIXED;

                _state.vdp1.frame_rate = interval;
                _state.vdp1.frame_count = 0;

                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM_FCT);
        }

        _state.vdp1.interval_mode = mode;

        _state.vdp1.current_mode = &_vdp1_mode_table[mode];
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
vdp1_sync_cmdt_put(const vdp1_cmdt_t *cmdts, uint16_t count, uint16_t index)
{
        assert(cmdts != NULL);

        if (count == 0) {
                return;
        }

        _vdp1_sync_put();

        scu_dma_handle_t * const dma_handle =
            &_vdp1_dma_handle;

        dma_handle->dnr = CPU_CACHE_THROUGH | (uint32_t)cmdts;
        dma_handle->dnw = VDP1_VRAM(index * sizeof(vdp1_cmdt_t));
        dma_handle->dnc = count * sizeof(vdp1_cmdt_t);

        _vdp1_dma_transfer(dma_handle);
}

void
vdp1_sync_cmdt_list_put(const vdp1_cmdt_list_t *cmdt_list, uint16_t index)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);

        vdp1_sync_cmdt_put(cmdt_list->cmdts, cmdt_list->count, index);
}

void
vdp1_sync_cmdt_orderlist_put(const vdp1_cmdt_orderlist_t *cmdt_orderlist)
{
        assert(cmdt_orderlist != NULL);

        _vdp1_sync_put();

        scu_dma_handle_t * const dma_handle =
            &_vdp1_orderlist_dma_handle;

        dma_handle->dnw = CPU_CACHE_THROUGH | (uint32_t)cmdt_orderlist;

        _vdp1_dma_transfer(dma_handle);
}

void
vdp1_sync_cmdt_stride_put(const void *buffer, uint16_t count,
    uint16_t cmdt_index, uint16_t index)
{
        assert(buffer != NULL);

        if (count == 0) {
                return;
        }

        _vdp1_sync_put();

        scu_dma_handle_t * const dma_handle =
            &_vdp1_stride_dma_handle;

        dma_handle->dnr = CPU_CACHE_THROUGH | (uint32_t)buffer;
        dma_handle->dnw = VDP1_CMD_TABLE(index, cmdt_index);
        dma_handle->dnc = count * sizeof(uint16_t);

        _vdp1_dma_transfer(dma_handle);
}

void
vdp1_sync_put_wait(void)
{
        if ((_state.vdp1.flags & VDP1_FLAG_REQUEST_XFER_LIST) != VDP1_FLAG_REQUEST_XFER_LIST) {
                return;
        }

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(0);

        while ((_state.vdp1.flags & VDP1_FLAG_LIST_XFERRED) != VDP1_FLAG_LIST_XFERRED) {
        }

        cpu_intc_mask_set(sr_mask);
}

void
vdp1_sync_render(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        if ((_state.vdp1.flags & VDP1_FLAG_REQUEST_COMMIT_LIST) == VDP1_FLAG_REQUEST_COMMIT_LIST) {
                return;
        }

        if ((_state.vdp1.flags & VDP1_FLAG_REQUEST_XFER_LIST) != VDP1_FLAG_REQUEST_XFER_LIST) {
                return;
        }

        while ((_state.vdp1.flags & VDP1_FLAG_LIST_XFERRED) != VDP1_FLAG_LIST_XFERRED) {
        }

        _vdp1_sync_render_call();

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        _state.vdp1.flags |= VDP1_FLAG_REQUEST_COMMIT_LIST;

        cpu_intc_mask_set(sr_mask);

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

void
vdp1_sync_put_set(callback_handler_t callback_handler, void *work)
{
        callback_set(&_vdp1_put_callback, callback_handler, work);
}

void
vdp1_sync_render_set(callback_handler_t callback_handler, void *work)
{
        callback_set(&_vdp1_render_callback, callback_handler, work);
}

void
vdp1_sync_transfer_over_set(callback_handler_t callback_handler, void *work)
{
        callback_set(&_vdp1_transfer_over_callback, callback_handler, work);
}

void
vdp2_sync(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        if ((_state.flags & (SYNC_FLAG_VDP2_SYNC)) == SYNC_FLAG_VDP2_SYNC) {
                return;
        }

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

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

        cpu_intc_mask_set(sr_mask);

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

void
vdp2_sync_wait(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(0);

        while ((_state.flags & SYNC_FLAG_VDP2_SYNC) == SYNC_FLAG_VDP2_SYNC) {
        }

        cpu_intc_mask_set(sr_mask);

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

void
vdp_sync_vblank_in_set(callback_handler_t callback_handler, void *work)
{
        callback_set(&_vblank_in_callback, callback_handler, work);
}

void
vdp_sync_vblank_out_set(callback_handler_t callback_handler, void *work)
{
        callback_set(&_vblank_out_callback, callback_handler, work);
}

static void
_vdp1_init(void)
{
        const scu_dma_level_cfg_t dma_cfg = {
                .mode            = SCU_DMA_MODE_DIRECT,
                /* Prevent assertion */
                .xfer.direct.len = 0xFFFFFFFF,
                .xfer.direct.dst = 0xFFFFFFFF,
                .xfer.direct.src = 0xFFFFFFFF,
                .space           = SCU_DMA_SPACE_BUS_B,
                .stride          = SCU_DMA_STRIDE_2_BYTES,
                .update          = SCU_DMA_UPDATE_NONE
        };

        const scu_dma_level_cfg_t orderlist_dma_cfg = {
                .mode          = SCU_DMA_MODE_INDIRECT,
                /* Avoid assertion if pointer to indirect table is NULL */
                .xfer.indirect = (void *)0xFFFFFFFF,
                .space         = SCU_DMA_SPACE_BUS_B,
                .stride        = SCU_DMA_STRIDE_2_BYTES,
                .update        = SCU_DMA_UPDATE_NONE
        };

        const scu_dma_level_cfg_t stride_dma_cfg = {
                .mode            = SCU_DMA_MODE_DIRECT,
                /* Prevent assertion */
                .xfer.direct.len = 0xFFFFFFFF,
                .xfer.direct.dst = 0xFFFFFFFF,
                .xfer.direct.src = 0xFFFFFFFF,
                .space           = SCU_DMA_SPACE_BUS_B,
                .stride          = SCU_DMA_STRIDE_32_BYTES,
                .update          = SCU_DMA_UPDATE_WUP
        };

        callback_init(&_vdp1_put_callback);
        callback_init(&_vdp1_render_callback);
        callback_init(&_vdp1_transfer_over_callback);

        _state.vdp1.flags = VDP1_FLAG_IDLE;
        _state.vdp1.current_mode = NULL;
        _state.vdp1.field_count = 0;
        _state.vdp1.previous_fb_mode = VDP1_FB_MODE_ERASE_CHANGE;
        _state.vdp1.fb_mode = VDP1_FB_MODE_ERASE_CHANGE;
        _state.vdp1.frame_rate = 0;
        _state.vdp1.frame_count = 0;

        _state.vdp1.interval_mode = VDP1_INTERVAL_MODE_AUTO;

        vdp1_sync_mode_set(VDP1_SYNC_MODE_ERASE_CHANGE);
        vdp1_sync_interval_set(0);

        scu_dma_config_buffer(&_vdp1_dma_handle, &dma_cfg);
        scu_dma_config_buffer(&_vdp1_orderlist_dma_handle, &orderlist_dma_cfg);
        scu_dma_config_buffer(&_vdp1_stride_dma_handle, &stride_dma_cfg);
}

static inline void
_vdp1_transfer_over_process(void)
{
        const vdp1_transfer_status_t transfer_status =
            vdp1_transfer_status_get();

        if ((transfer_status.cef | transfer_status.bef) == 0) {
                callback_call(&_vdp1_transfer_over_callback);
        }
}

static inline void __always_inline
_vdp1_sync_put(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        /* Wait when a previous list is still transferring, or when the VDP1 is
         * rendering. We don't have to wait for request for frame buffer change
         * as at that point, the VDP1 is idling */
        if ((_state.vdp1.flags & VDP1_FLAG_REQUEST_COMMIT_LIST) == VDP1_FLAG_REQUEST_COMMIT_LIST) {
                while ((_state.vdp1.flags & VDP1_FLAG_LIST_COMMITTED) != VDP1_FLAG_LIST_COMMITTED) {
                }
        } else if ((_state.vdp1.flags & VDP1_FLAG_REQUEST_XFER_LIST) == VDP1_FLAG_REQUEST_XFER_LIST) {
                while ((_state.vdp1.flags & VDP1_FLAG_LIST_XFERRED) != VDP1_FLAG_LIST_XFERRED) {
                }
        }

        const uint32_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        uint8_t state_vdp1_flags;
        state_vdp1_flags = _state.vdp1.flags;

        state_vdp1_flags &= ~VDP1_FLAG_LIST_XFERRED;
        state_vdp1_flags &= ~VDP1_FLAG_LIST_COMMITTED;
        state_vdp1_flags |= VDP1_FLAG_REQUEST_XFER_LIST;

        _state.vdp1.flags = state_vdp1_flags;

        cpu_intc_mask_set(sr_mask);

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

static inline void __always_inline
_vdp1_dma_call(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        _state.vdp1.current_mode->dma();

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

static inline void __always_inline
_vdp1_sync_render_call(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        _state.vdp1.current_mode->sync_render();

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

static inline void __always_inline
_vdp1_sprite_end_call(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        /* Prevent the VBLANK-OUT interrupt from firing in case the callback
         * from the user runs long enough to be interrupted by the VBLANK-OUT
         * interrupt */
        _state.flags &= ~SYNC_FLAG_VDP1_VBLANK_OUT;

        _state.vdp1.current_mode->sprite_end();

        _state.flags |= SYNC_FLAG_VDP1_VBLANK_OUT;

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

static inline void __always_inline
_vdp1_vblank_in_call(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        _state.vdp1.current_mode->vblank_in();

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

static inline void __always_inline
_vdp1_vblank_out_call(void)
{
        _state.vdp1.current_mode->vblank_out();
}

static void
_vdp1_mode_auto_dma(void)
{
        MEMORY_WRITE(16, VDP1(PTMR), VDP1_PTMR_AUTO);

        _state.vdp1.flags |= VDP1_FLAG_LIST_XFERRED;

        callback_call(&_vdp1_put_callback);
}

static void
_vdp1_mode_auto_sync_render(void)
{
}

static void
_vdp1_mode_auto_sprite_end(void)
{
        _state.vdp1.flags |= VDP1_FLAG_LIST_COMMITTED;

        callback_call(&_vdp1_render_callback);
}

static void
_vdp1_mode_auto_vblank_in(void)
{
        if ((_state.vdp1.flags & VDP1_FLAG_REQUEST_COMMIT_LIST) != VDP1_FLAG_REQUEST_COMMIT_LIST) {
                return;
        }

        _vdp1_sprite_end_call();
}

static void
_vdp1_mode_auto_vblank_out(void)
{
        /* Going from manual to 1-cycle mode requires the FCM and FCT bits to be
         * cleared. Otherwise, we get weird behavior from the VDP1.
         *
         * However, VDP1(FBCR) must not be entirely cleared. This caused a lot
         * of glitching when in double-density interlace mode */
        MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_NONE);

        switch (_state.vdp1.fb_mode) {
        case VDP1_FB_MODE_ERASE_CHANGE:
                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_NONE);
                break;
        case VDP1_FB_MODE_CHANGE_ONLY:
                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM_FCT);
                break;
        }

        /* Wait until scanline #0 is reached to avoid the frame buffer change
         * from aborting plotting, resulting in a soft lock */
        vdp2_tvmd_vcount_wait(0);

        _vdp1_transfer_over_process();

        _state.flags &= ~SYNC_FLAG_VDP1_SYNC;

        _state.vdp1.flags &= ~VDP1_FLAG_MASK;
}

static void
_vdp1_mode_fixed_dma(void)
{
        _state.vdp1.flags |= VDP1_FLAG_LIST_XFERRED;

        callback_call(&_vdp1_put_callback);
}

static void
_vdp1_mode_fixed_sync_render(void)
{
        MEMORY_WRITE(16, VDP1(PTMR), VDP1_PTMR_PLOT);
}

static void
_vdp1_mode_fixed_sprite_end(void)
{
        uint8_t state_vdp1_flags;
        state_vdp1_flags = _state.vdp1.flags;

        if ((state_vdp1_flags & VDP1_FLAG_LIST_COMMITTED) != VDP1_FLAG_LIST_COMMITTED) {
                state_vdp1_flags |= VDP1_FLAG_LIST_COMMITTED;

                _state.vdp1.flags = state_vdp1_flags;

                callback_call(&_vdp1_render_callback);
        }
}

static void
_vdp1_mode_fixed_vblank_in(void)
{
        /* Cache the state to avoid multiple loads */
        uint8_t state_vdp1_flags;
        state_vdp1_flags = _state.vdp1.flags;

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_COMMIT_LIST) != VDP1_FLAG_REQUEST_COMMIT_LIST) {
                return;
        }

        const vdp1_transfer_status_t transfer_status =
            vdp1_transfer_status_get();

        const bool list_committed =
            ((state_vdp1_flags & VDP1_FLAG_LIST_COMMITTED) == VDP1_FLAG_LIST_COMMITTED);

        if (transfer_status.cef && !list_committed) {
                _vdp1_sprite_end_call();
        }

        if ((_state.vdp1.frame_count + 1) == _state.vdp1.frame_rate) {
                /* One frame before matching the frame rate, call to erase the
                 * display frame buffer so that by the next frame we can call to
                 * change frame buffers */
                if (_state.vdp1.fb_mode == VDP1_FB_MODE_ERASE_CHANGE) {
                        _state_vdp1()->regs->tvmr &= ~VDP1_TVMR_VBE;

                        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);
                        MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM);
                }
        } else if (_state.vdp1.frame_count == _state.vdp1.frame_rate) {
                /* Undo the increment at the end of function */
                _state.vdp1.frame_count = -1;

                /* Force stop plotting */
                if (!list_committed && !transfer_status.cef) {
                        MEMORY_WRITE(16, VDP1(PTMR), VDP1_PTMR_IDLE);

                        _vdp1_sprite_end_call();
                }

                state_vdp1_flags |= VDP1_FLAG_REQUEST_CHANGE;

                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM_FCT);
        }

        _state.vdp1.flags = state_vdp1_flags;

        _state.vdp1.frame_count++;
}

static void
_vdp1_mode_fixed_vblank_out(void)
{
        uint8_t state_vdp1_flags;
        state_vdp1_flags = _state.vdp1.flags;

        if ((state_vdp1_flags & VDP1_FLAG_CHANGED) == VDP1_FLAG_CHANGED) {
                return;
        }

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_CHANGE) != VDP1_FLAG_REQUEST_CHANGE) {
                return;
        }

        /* Wait until scanline #0 is reached to avoid the frame buffer change
         * from aborting plotting, resulting in a soft lock.
         *
         * This is for the case when a request to plot is started immediately
         * following the VBLANK-OUT interrupt */
        vdp2_tvmd_vcount_wait(0);

        _vdp1_transfer_over_process();

        _state.flags &= ~SYNC_FLAG_VDP1_SYNC;

        state_vdp1_flags &= ~VDP1_FLAG_MASK;

        _state.vdp1.flags = state_vdp1_flags;
}

static void
_vdp1_mode_variable_dma(void)
{
        _state.vdp1.flags |= VDP1_FLAG_LIST_XFERRED;

        callback_call(&_vdp1_put_callback);
}

static void
_vdp1_mode_variable_sync_render(void)
{
        MEMORY_WRITE(16, VDP1(PTMR), VDP1_PTMR_PLOT);
}

static void
_vdp1_mode_variable_sprite_end(void)
{
        _state.vdp1.flags |= VDP1_FLAG_LIST_COMMITTED;

        callback_call(&_vdp1_render_callback);
}

static void
_vdp1_mode_variable_vblank_in(void)
{
        /* Cache the state to avoid multiple loads */
        uint8_t state_vdp1_flags;
        state_vdp1_flags = _state.vdp1.flags;

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_COMMIT_LIST) != VDP1_FLAG_REQUEST_COMMIT_LIST) {
                return;
        }

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_CHANGE) == VDP1_FLAG_REQUEST_CHANGE) {
                return;
        }

        const int8_t frame_rate = _state.vdp1.frame_rate;
        int8_t frame_count;
        frame_count = _state.vdp1.frame_count;

        const vdp1_transfer_status_t transfer_status =
            vdp1_transfer_status_get();

        const bool list_committed =
            ((state_vdp1_flags & VDP1_FLAG_LIST_COMMITTED) == VDP1_FLAG_LIST_COMMITTED);

        if (transfer_status.cef || list_committed) {
                /* Avoid calling the sprite end mode callback more than once */
                if (!list_committed) {
                        _vdp1_sprite_end_call();
                }

                if ((frame_count + 1) == frame_rate) {
                        if (_state.vdp1.fb_mode == VDP1_FB_MODE_ERASE_CHANGE) {
                                _state_vdp1()->regs->tvmr &= ~VDP1_TVMR_VBE;

                                MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);
                                MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM);
                        }
                /* Resetting (-1 for undoing the increment at the end of
                 * function) the frame count only when the list has been
                 * committed is important because on the next VBLANK-IN, we need
                 * to request to change frame buffers */
                } else if ((frame_rate == 0) || (frame_count >= frame_rate)) {
                        frame_count = -1;

                        state_vdp1_flags |= VDP1_FLAG_REQUEST_CHANGE;

                        _state_vdp1()->regs->tvmr &= ~VDP1_TVMR_VBE;

                        if (_state.vdp1.fb_mode == VDP1_FB_MODE_ERASE_CHANGE) {
                                _state_vdp1()->regs->tvmr |= VDP1_TVMR_VBE;
                        }

                        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);

                        MEMORY_WRITE(16, VDP1(FBCR), VDP1_FBCR_FCM_FCT);
                }
        }

        frame_count++;

        _state.vdp1.flags = state_vdp1_flags;
        _state.vdp1.frame_count = frame_count;
}

static void
_vdp1_mode_variable_vblank_out(void)
{
        uint8_t state_vdp1_flags;
        state_vdp1_flags = _state.vdp1.flags;

        if ((state_vdp1_flags & VDP1_FLAG_CHANGED) == VDP1_FLAG_CHANGED) {
                return;
        }

        if ((state_vdp1_flags & VDP1_FLAG_REQUEST_CHANGE) != VDP1_FLAG_REQUEST_CHANGE) {
                return;
        }

        _state_vdp1()->regs->tvmr &= ~VDP1_TVMR_VBE;

        MEMORY_WRITE(16, VDP1(TVMR), _state_vdp1()->regs->tvmr);

        /* Wait until scanline #0 is reached to avoid the frame buffer change
         * from aborting plotting, resulting in a soft lock.
         *
         * This is for the case when a request to plot is started immediately
         * following the VBLANK-OUT interrupt */
        vdp2_tvmd_vcount_wait(0);

        _state.flags &= ~SYNC_FLAG_VDP1_SYNC;

        state_vdp1_flags &= ~VDP1_FLAG_MASK;

        _state.vdp1.flags = state_vdp1_flags;
}

static void
_vdp1_dma_transfer(const scu_dma_handle_t *dma_handle)
{
        scu_dma_config_set(0, SCU_DMA_START_FACTOR_ENABLE, dma_handle, NULL);

        scu_dma_level_end_set(0, _scu_dma_level_end_handler, NULL);

        cpu_cache_purge();

        scu_dma_level_fast_start(0);
}

static void
_vdp2_init(void)
{
        extern void __vdp2_xfer_table_init(void);

        __vdp2_xfer_table_init();

        _state.vdp2.flags = VDP2_FLAG_IDLE;
}

static void
_vdp2_sync_commit(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        uint8_t state_vdp2_flags;
        state_vdp2_flags = _state.vdp2.flags;

        if ((state_vdp2_flags & VDP2_FLAG_REQUEST_COMMIT_REGS) == VDP2_FLAG_REQUEST_COMMIT_REGS) {
                return;
        }

        state_vdp2_flags &= ~VDP2_FLAG_REGS_COMMITTED;
        state_vdp2_flags |= VDP2_FLAG_REQUEST_COMMIT_REGS;

        scu_dma_level_t level;
        level = 0;

        /* Find an available SCU-DMA level */
        if ((scu_dma_level_busy(level))) {
                if ((level = scu_dma_level_unused_get()) < 0) {
                        level = 0;
                }
        }

        _state.vdp2.commit_level = level;
        _state.vdp2.flags = state_vdp2_flags;

        __vdp2_commit(level);

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

void
_vdp2_sync_commit_wait(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        uint8_t state_vdp2_flags;
        state_vdp2_flags = _state.vdp2.flags;

        if ((state_vdp2_flags & VDP2_FLAG_REQUEST_COMMIT_REGS) != VDP2_FLAG_REQUEST_COMMIT_REGS) {
                return;
        }

        __vdp2_commit_wait(_state.vdp2.commit_level);

        state_vdp2_flags &= ~VDP2_FLAG_REQUEST_COMMIT_REGS;
        state_vdp2_flags |= VDP2_FLAG_REGS_COMMITTED;

        _state.vdp2.flags = state_vdp2_flags;

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

static void
_scu_dma_level_end_handler(void *work __unused)
{
        scu_dma_level_end_set(0, NULL, NULL);

        _vdp1_dma_call();
}

static void
_vblank_in_handler(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        DEBUG_PRINTF("_state.vdp1.flags: 0x%02X\n", _state.vdp1.flags);

        uint8_t state_flags;
        state_flags = _state.flags;

        if ((state_flags & SYNC_FLAG_MASK_V1SVBI) == SYNC_FLAG_MASK_V1SVBI) {
                _vdp1_vblank_in_call();
        }

        callback_call(&_vblank_in_callback);

        /* VBLANK-IN interrupt runs at scanline #224 */
        if ((state_flags & SYNC_FLAG_VDP2_SYNC) == SYNC_FLAG_VDP2_SYNC) {
                _vdp2_sync_commit();
        }

        int ret __unused;
        ret = dma_queue_flush(DMA_QUEUE_TAG_VBLANK_IN);
        assert(ret >= 0);

        dma_queue_flush_wait();

        _vdp2_sync_commit_wait();

        state_flags &= ~SYNC_FLAG_VDP2_SYNC;

        _state.flags = state_flags;

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}

static void
_vblank_out_handler(void)
{
        DEBUG_PRINTF("%s: Enter L%i\n", __FUNCTION__, __LINE__);

        /* VBLANK-OUT interrupt runs at scanline #511 */

        if ((_state.flags & SYNC_FLAG_MASK_V1SVBO) == SYNC_FLAG_MASK_V1SVBO) {
                _vdp1_vblank_out_call();
        }

        callback_call(&_vblank_out_callback);

        DEBUG_PRINTF("%s: Exit L%i\n", __FUNCTION__, __LINE__);
}
