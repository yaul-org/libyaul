/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include <cpu/cache.h>
#include <cpu/dmac.h>

#include <vdp.h>

#include <sys/dma-queue.h>

#include <dbgio.h>

#include "../dbgio-internal.h"

#include "cons/cons.h"

#include "vdp2_font.inc"

#define STATE_IDLE                      0x00
#define STATE_INITIALIZED               0x01
#define STATE_PARTIALLY_INITIALIZED     0x02
#define STATE_BUFFER_DIRTY              0x04
#define STATE_BUFFER_CLEARED            0x08
#define STATE_BUFFER_FLUSHING           0x10
#define STATE_BUFFER_FORCE_FLUSHING     0x20

typedef struct {
        struct dma_reg_buffer dma_reg_buffer;

        /* Base CPD VRAM address */
        uint32_t cp_table;
        /* Base palette CRAM address */
        uint32_t color_palette;

        /* Base page VRAM address */
        uint32_t page_base;
        uint16_t *page_pnd;
        /* Size of splt page */
        uint16_t page_size;
        uint16_t page_width;
        uint16_t page_height;
        /* PND value for clearing a page */
        uint16_t pnd_clear;

        uint8_t state;
} dev_state_t;

static struct {
        /* List of pointers to free */
        void *free_ptrs[2];
        /* VDP sync callback ID to remove */
        int8_t sync_cid;
} _init_work;

static void _init(const dbgio_vdp2_t *);
static void _deinit(void);
static void _buffer(const char *);
static void _flush(void);

static inline void __always_inline _pnd_clear(int16_t, int16_t);
static inline void __always_inline _pnd_write(int16_t, int16_t,
    uint16_t);

static void _buffer_clear(void);
static void _buffer_area_clear(int16_t, int16_t, int16_t, int16_t);
static void _buffer_line_clear(int16_t, int16_t, int16_t);
static void _buffer_write(int16_t, int16_t, uint8_t);

static void _font_1bpp_4bpp_decompress(uint8_t *, const uint8_t *, uint8_t,
    uint8_t);

static void _init_dma_handler(const struct dma_queue_transfer *);
static void _flush_dma_handler(const struct dma_queue_transfer *);

static void _cleanup_initialization(void);

/* Restrictions:
 * 1. Screen will always be displayed
 * 2. Rotational backgrounds are not supported
 * 3. Screen priority is always 7
 * 4. 1x1 plane size is enforced
 * 5. Page 0 of plane A will always be used
 * 6. Resets scroll position to (0, 0)
 */

static const dbgio_vdp2_t _default_params = {
        .font_cpd = &_font_cpd[0],
        .font_pal = &_font_pal[0],
        .font_fg = 7,
        .font_bg = 0,

        .scrn = SCRN_NBG3,

        .cpd_bank = 3,
        .cpd_offset = 0x00000,

        .pnd_bank = 3,
        .pnd_offset = 2,

        .cpd_cycp = {
                .t0 = VRAM_CYCP_PNDR_NBG3,
                .t1 = VRAM_CYCP_NO_ACCESS,
                .t2 = VRAM_CYCP_NO_ACCESS,
                .t3 = VRAM_CYCP_NO_ACCESS,
                .t4 = VRAM_CYCP_CHPNDR_NBG3,
                .t5 = VRAM_CYCP_NO_ACCESS,
                .t6 = VRAM_CYCP_NO_ACCESS,
                .t7 = VRAM_CYCP_NO_ACCESS
        },
        .pnd_cycp = {
                .t0 = VRAM_CYCP_PNDR_NBG3,
                .t1 = VRAM_CYCP_NO_ACCESS,
                .t2 = VRAM_CYCP_NO_ACCESS,
                .t3 = VRAM_CYCP_NO_ACCESS,
                .t4 = VRAM_CYCP_CHPNDR_NBG3,
                .t5 = VRAM_CYCP_NO_ACCESS,
                .t6 = VRAM_CYCP_NO_ACCESS,
                .t7 = VRAM_CYCP_NO_ACCESS
        },

        .cram_index = 0
};

/* Parameters set by the user */
static dbgio_vdp2_t _params;

static dev_state_t *_dev_state;

const dbgio_dev_ops_t _internal_dev_ops_vdp2 = {
        .dev = DBGIO_DEV_VDP2,
        .default_params = &_default_params,
        .init = (void (*)(const void *))_init,
        .deinit = _deinit,
        .buffer = _buffer,
        .flush = _flush
};

static void
_init(const dbgio_vdp2_t *params)
{
        struct {
                /* Holds transfers for font CPD and PAL */
                struct dma_xfer xfer_tbl[2];

                struct dma_reg_buffer reg_buffer;
        } *dma_font;

        static const cons_ops_t cons_ops = {
                .clear = _buffer_clear,
                .area_clear = _buffer_area_clear,
                .line_clear = _buffer_line_clear,
                .write = _buffer_write
        };

        assert(params != NULL);

        assert(params->font_cpd != NULL);
        assert(params->font_pal != NULL);

        assert((params->font_fg >= 0) && (params->font_bg <= 15));
        assert((params->font_bg >= 0) && (params->font_bg <= 15));

        assert((params->scrn == SCRN_NBG0) ||
               (params->scrn == SCRN_NBG1) ||
               (params->scrn == SCRN_NBG2) ||
               (params->scrn == SCRN_NBG3));

        assert((params->scrn != SCRN_RBG0) &&
               (params->scrn != SCRN_RBG1));

        assert((params->cpd_bank >= 0) && (params->cpd_bank <= 3));
        /* XXX: Fetch the VRAM bank split configuration and determine the VRAM
         *      bank size */
        assert(params->cpd_offset < VRAM_4SPLIT_BANK_SIZE_4MBIT);

        assert((params->pnd_bank >= 0) && (params->pnd_bank <= 3));
        /* XXX: Determine the page size and check against the number of
         *      available offsets */

        /* There are 128 16-color banks, depending on CRAM mode */
        /* XXX: Fetch CRAM mode and check number of available 16-color banks */
        assert((params->cram_index >= 0) && (params->cram_index < 128));

        if ((_dev_state->state & (STATE_INITIALIZED | STATE_PARTIALLY_INITIALIZED)) != 0x00) {
                return;
        }

        if (_dev_state == NULL) {
                _dev_state = malloc(sizeof(dev_state_t));
        }
        assert(_dev_state != NULL);

        (void)memset(_dev_state, 0x00, sizeof(dev_state_t));

        _dev_state->page_size = SCRN_CALCULATE_PAGE_SIZE_M(1 * 1, 1);
        _dev_state->page_width = SCRN_CALCULATE_PAGE_WIDTH_M(1 * 1);
        _dev_state->page_height = SCRN_CALCULATE_PAGE_HEIGHT_M(1 * 1);

        /* One page per plane */
        _dev_state->page_base = VRAM_ADDR_4MBIT(params->pnd_bank,
            params->pnd_offset * _dev_state->page_size);

        _dev_state->cp_table = VRAM_ADDR_4MBIT(params->cpd_bank, params->cpd_offset);
        _dev_state->color_palette = CRAM_ADDR(params->cram_index << 3);

        struct scrn_cell_format cell_format = {
                .scf_scroll_screen = params->scrn,
                .scf_cc_count = SCRN_CCC_PALETTE_16,
                .scf_character_size = 1 * 1,
                .scf_pnd_size = 1, /* 1-word */
                .scf_auxiliary_mode = 0,
                .scf_cp_table = _dev_state->cp_table,
                .scf_color_palette = _dev_state->color_palette,
                .scf_plane_size = 1 * 1,
                .scf_map.plane_a = _dev_state->page_base,
                .scf_map.plane_b = _dev_state->page_base,
                .scf_map.plane_c = _dev_state->page_base,
                .scf_map.plane_d = _dev_state->page_base
        };

        vdp2_scrn_cell_format_set(&cell_format);

        /* Restricting the page to 64x32 avoids wasting space */
        _dev_state->page_size /= 2;

        /* PND value used to clear pages */
        _dev_state->pnd_clear = SCRN_PND_CONFIG_0(
                cell_format.scf_cp_table,
                cell_format.scf_color_palette,
                /* vf = */ 0,
                /* hf = */ 0);

        if (_dev_state->page_pnd == NULL) {
                _dev_state->page_pnd = malloc(_dev_state->page_size);
        }
        assert(_dev_state->page_pnd != NULL);

        uint8_t *dec_cpd;
        dec_cpd = (uint8_t *)malloc(FONT_4BPP_SIZE);
        assert(dec_cpd != NULL);
        _init_work.free_ptrs[0] = dec_cpd;

        _font_1bpp_4bpp_decompress(dec_cpd, params->font_cpd, params->font_fg,
            params->font_bg);

        struct dma_level_cfg dma_level_cfg;

        /* Align to a 32-byte boundary */
        /* XXX: Refactor { */
        void *aligned;
        aligned = malloc(sizeof(*dma_font) + 32);
        assert(aligned != NULL);
        _init_work.free_ptrs[1] = aligned;

        uint32_t aligned_offset;
        aligned_offset = (((uint32_t)aligned + 0x0000001F) & ~0x0000001F) - (uint32_t)aligned;
        dma_font = (void *)((uint32_t)aligned + aligned_offset);
        /* } */

        dma_level_cfg.dlc_mode = DMA_MODE_INDIRECT;
        dma_level_cfg.dlc_xfer.indirect = &dma_font->xfer_tbl[0];
        dma_level_cfg.dlc_stride = DMA_STRIDE_2_BYTES;
        dma_level_cfg.dlc_update = DMA_UPDATE_NONE;

        /* Font CPD */
        dma_font->xfer_tbl[0].len = FONT_4BPP_SIZE;
        dma_font->xfer_tbl[0].dst = (uint32_t)_dev_state->cp_table;
        dma_font->xfer_tbl[0].src = CPU_CACHE_THROUGH | (uint32_t)dec_cpd;

        /* Font PAL */
        dma_font->xfer_tbl[1].len = FONT_COLOR_COUNT * sizeof(color_rgb555_t);
        dma_font->xfer_tbl[1].dst = _dev_state->color_palette;
        dma_font->xfer_tbl[1].src = DMA_INDIRECT_TBL_END | CPU_CACHE_THROUGH | (uint32_t)params->font_pal;

        scu_dma_config_buffer(&dma_font->reg_buffer, &dma_level_cfg);

        int8_t ret;
        ret = dma_queue_enqueue(&dma_font->reg_buffer, DMA_QUEUE_TAG_VBLANK_IN,
            _init_dma_handler, NULL);
        assert(ret == 0);

        /* 64x32 page PND */
        dma_level_cfg.dlc_mode = DMA_MODE_DIRECT;
        dma_level_cfg.dlc_xfer.direct.len = _dev_state->page_size;
        dma_level_cfg.dlc_xfer.direct.dst = (uint32_t)_dev_state->page_base;
        dma_level_cfg.dlc_xfer.direct.src = CPU_CACHE_THROUGH | (uint32_t)&_dev_state->page_pnd[0];
        dma_level_cfg.dlc_stride = DMA_STRIDE_2_BYTES;
        dma_level_cfg.dlc_update = DMA_UPDATE_NONE;

        scu_dma_config_buffer(&_dev_state->dma_reg_buffer, &dma_level_cfg);

        cons_init(&cons_ops, CONS_COLS_MIN, CONS_ROWS_MIN);

        /* We're truly initialized once the user has made at least one call to
         * vdp_sync() */
        _init_work.sync_cid = vdp_sync_user_callback_add(
                (void (*)(void *))_cleanup_initialization, NULL);

        /* Due to the 1BPP font being decompressed in cached H-WRAM, we need to
         * flush the cache as the DMA transfer accesses the uncached mirror
         * address to the decompressed 4BPP font, which could result in fetching
         * stale values not yet written back to H-WRAM */
        cpu_cache_purge();

        /* Copy user's set device parameters */
        (void)memcpy(&_params, params, sizeof(dbgio_vdp2_t));

        _dev_state->state = STATE_PARTIALLY_INITIALIZED;
}

static void
_deinit(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & (STATE_PARTIALLY_INITIALIZED | STATE_INITIALIZED)) == 0x00) {
                return;
        }

        free(_dev_state->page_pnd);
        free(_dev_state);

        _dev_state = NULL;

        _dev_state->state = STATE_IDLE;
}

static void
_buffer(const char *buffer)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & (STATE_PARTIALLY_INITIALIZED | STATE_INITIALIZED)) == 0x00) {
                return;
        }

        /* It's the best we can do for now. If the current buffer is marked for
         * flushing, we have to silently drop any calls to write to the
         * buffer */
        uint8_t state_mask;
        state_mask = STATE_BUFFER_FLUSHING | STATE_BUFFER_FORCE_FLUSHING;

        if ((_dev_state->state & state_mask) == STATE_BUFFER_FLUSHING) {
                return;
        }

        cons_buffer(buffer);
}

static void
_flush(void)
{
        if (_dev_state == NULL) {
                return;
        }

        if ((_dev_state->state & (STATE_PARTIALLY_INITIALIZED | STATE_INITIALIZED)) == 0x00) {
                return;
        }

        if ((_dev_state->state & STATE_BUFFER_DIRTY) != STATE_BUFFER_DIRTY) {
                return;
        }

        if ((_dev_state->state & STATE_BUFFER_FLUSHING) == STATE_BUFFER_FLUSHING) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_FLUSHING;

        /* Force reset */
        vdp2_scrn_priority_set(_params.scrn, 7);
        vdp2_scrn_scroll_x_set(_params.scrn, F16(0.0f));
        vdp2_scrn_scroll_y_set(_params.scrn, F16(0.0f));
        vdp2_scrn_display_set(_params.scrn, /* transparent = */ true);

        vdp2_vram_cycp_bank_set(_params.cpd_bank, &_params.cpd_cycp);
        vdp2_vram_cycp_bank_set(_params.pnd_bank, &_params.pnd_cycp);

        int8_t ret;
        ret = dma_queue_enqueue(&_dev_state->dma_reg_buffer, DMA_QUEUE_TAG_VBLANK_IN,
            _flush_dma_handler, NULL);
        assert(ret == 0);

        _cleanup_initialization();
}

static inline void __always_inline
_pnd_clear(int16_t col, int16_t row)
{
        _pnd_write(col, row, _dev_state->pnd_clear);
}

static inline void __always_inline
_pnd_write(int16_t col, int16_t row, uint16_t value)
{
        int16_t offset;
        offset = col + (row * _dev_state->page_width);

        _dev_state->page_pnd[offset] = value;
}

static void
_buffer_clear(void)
{
        /* Don't try to clear the buffer again if it's already been cleared */
        if ((_dev_state->state & STATE_BUFFER_CLEARED) == STATE_BUFFER_CLEARED) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_DIRTY;

        uint8_t ch;
        ch = 2;

        struct dmac_ch_cfg dmac_cfg  = {
                .dcc_ch = ch,
                .dcc_dst = (uint32_t)&_dev_state->page_pnd[0],
                .dcc_dst_mode = DMAC_DESTINATION_INCREMENT,
                .dcc_src_mode = DMAC_SOURCE_FIXED,
                .dcc_src = _dev_state->pnd_clear,
                .dcc_len = _dev_state->page_size,
                .dcc_stride = DMAC_STRIDE_2_BYTES,
                .dcc_bus_mode = DMAC_BUS_MODE_CYCLE_STEAL,
                .dcc_ihr = NULL
        };

        cpu_dmac_channel_config_set(&dmac_cfg);
        cpu_dmac_channel_start(ch);
        cpu_dmac_channel_wait(ch);

        _dev_state->state |= STATE_BUFFER_CLEARED;
}

static void
_buffer_area_clear(int16_t col_start, int16_t col_end, int16_t row_start,
    int16_t row_end)
{
        _dev_state->state |= STATE_BUFFER_DIRTY;

        /* XXX: Should we check if attempting to clear the entire buffer? */

        int16_t row;
        for (row = row_start; row < row_end; row++) {
                int16_t col;
                for (col = col_start; col < col_end; col++) {
                        _pnd_clear(col, row);
                }
        }
}

static void
_buffer_line_clear(int16_t col_start, int16_t col_end, int16_t row)
{
        _dev_state->state |= STATE_BUFFER_DIRTY;

        int16_t col;
        for (col = col_start; col < col_end; col++) {
                _pnd_clear(col, row);
        }
}

static void
_buffer_write(int16_t col, int16_t row, uint8_t ch)
{
        _dev_state->state |= STATE_BUFFER_DIRTY;
        _dev_state->state &= ~STATE_BUFFER_CLEARED;

        uint16_t pnd;
        pnd = SCRN_PND_CONFIG_0(
                /* Each cell is 32 bytes */
                _dev_state->cp_table | (ch << 5),
                _dev_state->color_palette,
                /* vf = */ 0,
                /* hf = */ 0);

        _pnd_write(col, row, pnd);
}

static inline uint8_t __always_inline
_1bpp_4bpp_convert(uint8_t *row_1bpp, const uint8_t *fgbg)
{
        uint8_t out_4bpp;
        out_4bpp = 0x00;

        out_4bpp |= (fgbg[*row_1bpp & 0x01] & 0x0F) << 4;
        *row_1bpp >>= 1;
        out_4bpp |= fgbg[*row_1bpp & 0x01] & 0x0F;
        *row_1bpp >>= 1;

        return out_4bpp;
}

static void
_font_1bpp_4bpp_decompress(uint8_t *dec_cpd, const uint8_t *cmp_cpd, uint8_t fg, uint8_t bg)
{
        assert(dec_cpd != NULL);
        assert(cmp_cpd != NULL);
        assert(((uintptr_t)cmp_cpd & 0x00000003) == 0x00000000);

        const uint8_t fgbg[] = {
                bg,
                fg
        };

        uint32_t i;
        for (i = 0; i < FONT_1BPP_SIZE; i++) {
                uint32_t j;
                j = i << 2;

                uint8_t cpd;
                cpd = cmp_cpd[i];

                dec_cpd[j + 0] = _1bpp_4bpp_convert(&cpd, fgbg);
                dec_cpd[j + 1] = _1bpp_4bpp_convert(&cpd, fgbg);
                dec_cpd[j + 2] = _1bpp_4bpp_convert(&cpd, fgbg);
                dec_cpd[j + 3] = _1bpp_4bpp_convert(&cpd, fgbg);
        }
}

static void
_init_dma_handler(const struct dma_queue_transfer *transfer)
{
        if (transfer->dqt_status == DMA_QUEUE_STATUS_COMPLETE) {
                return;
        }

        _dev_state->state = STATE_IDLE;

        /* When a DMA request is canceled, it's called outside of any
         * internal interrupt handlers, so we're able to call free() */
        free(_init_work.free_ptrs[0]);
        free(_init_work.free_ptrs[1]);

        vdp_sync_user_callback_remove(_init_work.sync_cid);
}

static void
_flush_dma_handler(const struct dma_queue_transfer *transfer)
{
        if (transfer->dqt_status == DMA_QUEUE_STATUS_COMPLETE) {
                uint8_t state_mask;
                state_mask = STATE_BUFFER_DIRTY | STATE_BUFFER_FLUSHING | STATE_BUFFER_FORCE_FLUSHING;

                _dev_state->state &= ~state_mask;

                return;
        }

        /* If the DMA request was canceled, then we should allow force
         * flush while blocking any more writes to the buffer */
        _dev_state->state |= STATE_BUFFER_FORCE_FLUSHING;
}

static void
_cleanup_initialization(void)
{
        if ((_dev_state->state & STATE_PARTIALLY_INITIALIZED) != STATE_PARTIALLY_INITIALIZED) {
                return;
        }

        _dev_state->state &= ~STATE_PARTIALLY_INITIALIZED;
        _dev_state->state |= STATE_INITIALIZED;

        free(_init_work.free_ptrs[0]);
        free(_init_work.free_ptrs[1]);

        _init_work.free_ptrs[0] = NULL;
        _init_work.free_ptrs[1] = NULL;
}
