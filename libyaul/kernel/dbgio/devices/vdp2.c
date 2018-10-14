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

#include <vdp2.h>

#include <sys/dma-queue.h>

#include <dbgio.h>

#include "../dbgio-internal.h"

#include "cons/cons.h"

#include "vdp2_font.inc"

#define STATE_IDLE              0x00
#define STATE_BUFFER_DIRTY      0x01
#define STATE_BUFFER_FLUSHING   0x02

typedef struct {
        uint32_t dma_reg_buffer[DMA_REG_BUFFER_WORD_COUNT];

        struct scrn_cell_format cell_format;

        uint16_t page_size;
        uint16_t page_width;
        uint16_t page_height;
        uint16_t *page_pnd;
        uint16_t page_clear_pnd;

        uint8_t state;
} dev_state_t;

static void _init(const dbgio_vdp2_t *);
static void _flush(void);

static inline void __attribute__ ((always_inline)) _pnd_clear(uint32_t, uint32_t);
static inline void __attribute__ ((always_inline)) _pnd_write(uint32_t, uint32_t, uint16_t);

static void _buffer_clear(void);
static void _buffer_area_clear(int32_t, int32_t, int32_t, int32_t);
static void _buffer_line_clear(int32_t, int32_t, int32_t);
static void _buffer_write(int32_t, int32_t, uint8_t);

static void _dma_font_handler(void *);
static void _dma_handler(void *);

static const dbgio_vdp2_t _default_params = {
        .font_cpd = &_font_cpd[0],
        .font_pal = &_font_pal[0],
        .font_fg = 1,
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

static dev_state_t *_dev_state;

const dbgio_dev_ops_t _internal_dev_ops_vdp2 = {
        .dev = DBGIO_DEV_VDP2,
        .default_params = &_default_params,
        .init = (void (*)(const void *))_init,
        .buffer = cons_buffer,
        .flush = _flush
};

static void
_init(const dbgio_vdp2_t *params)
{
        struct {
                /* Holds transfers for font CPD and PAL */
                struct dma_xfer xfer_tbl[2];

                uint32_t reg_buffer[DMA_REG_BUFFER_WORD_COUNT];
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

        assert((params->cpd_bank >= 0) && (params->cpd_bank <= 3));
        /* XXX: Fetch the VRAM bank split configuration and determine
         * the VRAM bank size */
        assert(params->cpd_offset < VRAM_4SPLIT_BANK_SIZE_4MBIT);

        assert((params->pnd_bank >= 0) && (params->pnd_bank <= 3));
        /* XXX: Determine the page size and check against the number of
         * available offsets */

        /* There are 128 16-color banks, depending on CRAM mode */
        /* XXX: Fetch CRAM mode and check number of available 16-color
         * banks */
        assert((params->cram_index >= 0) && (params->cram_index < 128));

        if (_dev_state == NULL) {
                _dev_state = malloc(sizeof(dev_state_t));

                memset(_dev_state, 0x00, sizeof(dev_state_t));
        }
        assert(_dev_state != NULL);

        _dev_state->page_size = SCRN_CALCULATE_PAGE_SIZE_M(1 * 1, 1);
        _dev_state->page_width = SCRN_CALCULATE_PAGE_WIDTH_M(1 * 1);
        _dev_state->page_height = SCRN_CALCULATE_PAGE_HEIGHT_M(1 * 1);

        uint32_t cpd;
        cpd = VRAM_ADDR_4MBIT(params->cpd_bank, params->cpd_offset);

        /* One page per plane */
        uint32_t page;
        page = VRAM_ADDR_4MBIT(params->pnd_bank,
            params->pnd_offset * _dev_state->page_size);

        uint32_t color_palette;
        color_palette = CRAM_ADDR(params->cram_index << 3);

        _dev_state->cell_format.scf_scroll_screen = params->scrn;
        _dev_state->cell_format.scf_cc_count = SCRN_CCC_PALETTE_16;
        _dev_state->cell_format.scf_character_size = 1 * 1;
        _dev_state->cell_format.scf_pnd_size = 1; /* 1-word */
        _dev_state->cell_format.scf_auxiliary_mode = 0;
        _dev_state->cell_format.scf_cp_table = cpd;
        _dev_state->cell_format.scf_color_palette = color_palette;
        _dev_state->cell_format.scf_plane_size = 1 * 1;
        _dev_state->cell_format.scf_map.plane_a = page;
        _dev_state->cell_format.scf_map.plane_b = page;
        _dev_state->cell_format.scf_map.plane_c = page;
        _dev_state->cell_format.scf_map.plane_d = page;

        vdp2_scrn_cell_format_set(&_dev_state->cell_format);
        vdp2_scrn_priority_set(params->scrn, 7);
        vdp2_scrn_display_set(params->scrn, /* transparent = */ true);

        vdp2_vram_cycp_bank_set(params->cpd_bank, &params->cpd_cycp);
        vdp2_vram_cycp_bank_set(params->pnd_bank, &params->pnd_cycp);

        /* Restricting the page to 64x32 avoids wasting space */
        _dev_state->page_size /= 2;

        /* PND value used to clear pages */
        _dev_state->page_clear_pnd = SCRN_PND_CONFIG_0(
                _dev_state->cell_format.scf_cp_table,
                _dev_state->cell_format.scf_color_palette,
                /* vf = */ 0,
                /* hf = */ 0);

        if (_dev_state->page_pnd == NULL) {
                _dev_state->page_pnd = malloc(_dev_state->page_size);
        }
        assert(_dev_state->page_pnd != NULL);

        struct dma_level_cfg dma_level_cfg;

        void *aligned;
        aligned = malloc(sizeof(*dma_font) + 32);
        assert(aligned != NULL);

        /* Align to a 32-byte boundary */
        /* XXX: Refactor { */
        uint32_t aligned_offset;
        aligned_offset = (((uint32_t)aligned + 0x0000001F) & ~0x0000001F) - (uint32_t)aligned;
        dma_font = (void *)((uint32_t)aligned + aligned_offset);
        /* } */

        dma_level_cfg.dlc_mode = DMA_MODE_INDIRECT;
        dma_level_cfg.dlc_xfer.indirect = &dma_font->xfer_tbl[0];
        dma_level_cfg.dlc_stride = DMA_STRIDE_2_BYTES;
        dma_level_cfg.dlc_update = DMA_UPDATE_NONE;

        /* Font CPD */
        dma_font->xfer_tbl[0].len = FONT_SIZE;
        dma_font->xfer_tbl[0].dst = (uint32_t)_dev_state->cell_format.scf_cp_table;
        dma_font->xfer_tbl[0].src = CPU_CACHE_THROUGH | (uint32_t)params->font_cpd;

        /* Font PAL */
        dma_font->xfer_tbl[1].len = FONT_COLOR_COUNT * sizeof(color_rgb888_t);
        dma_font->xfer_tbl[1].dst = _dev_state->cell_format.scf_color_palette;
        dma_font->xfer_tbl[1].src = DMA_INDIRECT_TBL_END | CPU_CACHE_THROUGH | (uint32_t)params->font_pal;

        scu_dma_config_buffer(dma_font->reg_buffer, &dma_level_cfg);

        dma_queue_enqueue(dma_font->reg_buffer, DMA_QUEUE_TAG_VBLANK_IN,
            _dma_font_handler, aligned);

        /* 64x32 page PND */
        dma_level_cfg.dlc_mode = DMA_MODE_DIRECT;
        dma_level_cfg.dlc_xfer.direct.len = _dev_state->page_size;
        dma_level_cfg.dlc_xfer.direct.dst = (uint32_t)_dev_state->cell_format.scf_map.plane_a;
        dma_level_cfg.dlc_xfer.direct.src = CPU_CACHE_THROUGH | (uint32_t)&_dev_state->page_pnd[0];
        dma_level_cfg.dlc_stride = DMA_STRIDE_2_BYTES;
        dma_level_cfg.dlc_update = DMA_UPDATE_NONE;

        scu_dma_config_buffer(_dev_state->dma_reg_buffer, &dma_level_cfg);

        cons_init(&cons_ops, CONS_COLS_MIN, CONS_ROWS_MIN);

        _dev_state->state = STATE_BUFFER_DIRTY;

        _flush();
}

static void
_flush(void)
{
        if (_dev_state->state != STATE_BUFFER_DIRTY) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_FLUSHING;

        dma_queue_enqueue(_dev_state->dma_reg_buffer, DMA_QUEUE_TAG_VBLANK_IN,
            _dma_handler, NULL);
}

static inline void __attribute__ ((always_inline))
_pnd_clear(uint32_t col, uint32_t row)
{
        _pnd_write(col, row, _dev_state->page_clear_pnd);
}

static inline void __attribute__ ((always_inline))
_pnd_write(uint32_t col, uint32_t row, uint16_t value)
{
        uint32_t offset;
        offset = col + (row * _dev_state->page_width);

        _dev_state->page_pnd[offset] = value;
}

static void
_buffer_clear(void)
{
        _dev_state->state |= STATE_BUFFER_DIRTY;

        (void)memset(_dev_state->page_pnd, 0x00, _dev_state->page_size);
}

static void
_buffer_area_clear(int32_t col_start, int32_t col_end, int32_t row_start,
    int32_t row_end)
{
        _dev_state->state |= STATE_BUFFER_DIRTY;

        int32_t row;
        for (row = row_start; row < row_end; row++) {
                int32_t col;
                for (col = col_start; col < col_end; col++) {
                        _pnd_clear(col, row);
                }
        }
}

static void
_buffer_line_clear(int32_t col_start, int32_t col_end, int32_t row)
{
        _dev_state->state |= STATE_BUFFER_DIRTY;

        int32_t col;
        for (col = col_start; col < col_end; col++) {
                _pnd_clear(col, row);
        }
}

static void
_buffer_write(int32_t col, int32_t row, uint8_t ch)
{
        _dev_state->state |= STATE_BUFFER_DIRTY;

        uint16_t pnd;
        pnd = SCRN_PND_CONFIG_0(
                _dev_state->cell_format.scf_cp_table + (ch * 0x20),
                _dev_state->cell_format.scf_color_palette,
                /* vf = */ 0,
                /* hf = */ 0);

        _pnd_write(col, row, pnd);
}

static void
_dma_font_handler(void *work)
{
        free(work);
}

static void
_dma_handler(void *work __unused)
{
        _dev_state->state &= ~(STATE_BUFFER_DIRTY | STATE_BUFFER_FLUSHING);
}
