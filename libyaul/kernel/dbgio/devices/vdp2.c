/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cpu/cache.h>
#include <cpu/dmac.h>
#include <cpu/intc.h>

#include <dbgio/dbgio.h>

#include <vdp.h>

#include <sys/dma-queue.h>

#include <internal.h>
#include <dbgio/dbgio-internal.h>
#include <vdp-internal.h>

#include "cons/cons-internal.h"

typedef enum state_flags {
    STATE_IDLE                  = 0x00,
    STATE_INITIALIZED           = 0x01,
    STATE_BUFFER_DIRTY          = 0x02,
    STATE_BUFFER_CLEARED        = 0x04,
    STATE_BUFFER_FLUSHING       = 0x08,
    STATE_BUFFER_FORCE_FLUSHING = 0x10
} state_flags_t;

#define DEFAULT_NBGX_CPD_BANK    (3)
#define DEFAULT_NBGX_CPD         VDP2_VRAM_ADDR(DEFAULT_NBGX_CPD_BANK, 0x1C000)

#define DEFAULT_NBGX_PND_BANK    (3)
#define DEFAULT_NBGX_MAP_PLANE_A VDP2_VRAM_ADDR(DEFAULT_NBGX_PND_BANK, 0x1E000)
#define DEFAULT_NBGX_MAP_PLANE_B VDP2_VRAM_ADDR(DEFAULT_NBGX_PND_BANK, 0x1E000)
#define DEFAULT_NBGX_MAP_PLANE_C VDP2_VRAM_ADDR(DEFAULT_NBGX_PND_BANK, 0x1E000)
#define DEFAULT_NBGX_MAP_PLANE_D VDP2_VRAM_ADDR(DEFAULT_NBGX_PND_BANK, 0x1E000)
/* CRAM mode might cause issues in case it's changed */
#define DEFAULT_NBGX_PND(cpd_addr, pal_addr)                                   \
    VDP2_SCRN_PND_CONFIG_1(0, cpd_addr, pal_addr)

#define DEFAULT_NBGX_PAL         VDP2_CRAM_MODE_0_OFFSET(0, 127, 0)

/* T0: NBG3 pattern name data read on
 * T4: NBG3 character pattern name data read */
#define DEFAULT_NBGX_CYCP_MASK   0xFFF0FFF0
#define DEFAULT_NBGX_CYCP        0x00070003

/* CPU-DMAC channel used for _flush() and _buffer_clear() */
#define DEV_DMAC_CHANNEL 0

struct dev_state {
    state_flags_t state;

    dbgio_vdp2_t params;
    bool default_params;

    FILE file;

    const dbgio_vdp2_charmap_t *font_charmap;
    uint16_t *page_ptr;
    uint16_t page_size; /* Size of split page */
    uint16_t page_stride;

    uint16_vec2_t tv_resolution;
};

/* XXX: Move this comment to documentation */
/* The following parameter restrictions are enforced:
 *   - Screen will always be displayed
 *   - Rotational backgrounds are not supported
 *   - Screen priority is always 7
 *   - Page 0 of plane A will always be used
 *   - scroll position is reset to (0, 0) */

static const vdp2_scrn_cell_format_t _default_cell_format = {
    .scroll_screen = VDP2_SCRN_NBG3,
    .ccc           = VDP2_SCRN_CCC_PALETTE_16,
    .char_size     = VDP2_SCRN_CHAR_SIZE_1X1,
    .pnd_size      = 1,
    .aux_mode      = VDP2_SCRN_AUX_MODE_1,
    .plane_size    = VDP2_SCRN_PLANE_SIZE_1X1,
    .cpd_base      = DEFAULT_NBGX_CPD,
    .palette_base  = DEFAULT_NBGX_PAL
};

static const vdp2_scrn_normal_map_t _default_normal_map = {
    .plane_a = DEFAULT_NBGX_MAP_PLANE_A,
    .plane_b = DEFAULT_NBGX_MAP_PLANE_B,
    .plane_c = DEFAULT_NBGX_MAP_PLANE_C,
    .plane_d = DEFAULT_NBGX_MAP_PLANE_D
};

static const dbgio_vdp2_t _default_params = {
    .font         = &__dbgio_default_font,
    .font_charmap = NULL,

    .cell_format  = &_default_cell_format,
    .normal_map   = &_default_normal_map,
};

/* Device state for both async and direct VDP2 devices */
static struct dev_state *_dev_state;

static void _buffer_clear(void);
static void _buffer_area_clear(int16_t col_start, int16_t col_end, int16_t row_start, int16_t row_end);
static void _buffer_line_clear(int16_t row);
static void _buffer_line_partial_clear(int16_t col_start, int16_t col_end, int16_t row);
static void _buffer_write(int16_t col, int16_t row, uint8_t ch);

static const cons_ops_t _cons_ops = {
    .clear              = _buffer_clear,
    .area_clear         = _buffer_area_clear,
    .line_clear         = _buffer_line_clear,
    .line_partial_clear = _buffer_line_partial_clear,
    .write              = _buffer_write
};

static void _shared_display_set(bool display);

static size_t
_file_write(FILE *f __unused, const uint8_t *s, size_t l)
{
    __cons_write((const char *)s, l);

    return l;
}

static void
_cons_dimensions_calculate(uint16_t *cols, uint16_t *rows)
{
    *cols = _dev_state->tv_resolution.x >> 3;
    *rows = _dev_state->tv_resolution.y >> 3;

    const dbgio_vdp2_t * const params = &_dev_state->params;

    const vdp2_scrn_cell_format_t * const cell_format = params->cell_format;

    if (cell_format->char_size == VDP2_SCRN_CHAR_SIZE_2X2) {
        *cols >>= 1;
        *rows >>= 1;
    }
}

static inline void __always_inline
_pnd_write(int16_t col, int16_t row, uint16_t value)
{
    const uint32_t offset = col + (row * _dev_state->page_stride);

    _dev_state->page_ptr[offset] = value;
}

static inline void __always_inline
_pnd_clear(int16_t col, int16_t row)
{
    _pnd_write(col, row, _dev_state->font_charmap['\0'].pnd);
}

static void
_buffer_clear(void)
{
    /* Don't try to clear the buffer again if it's already been cleared */
    if ((_dev_state->state & STATE_BUFFER_CLEARED) == STATE_BUFFER_CLEARED) {
        return;
    }

    _dev_state->state |= STATE_BUFFER_DIRTY;

    const uint16_t pnd_value = _dev_state->font_charmap['\0'].pnd;
    uint32_t clear_value = (pnd_value << 16) | pnd_value;

    cpu_dmac_memset(DEV_DMAC_CHANNEL, _dev_state->page_ptr, clear_value, _dev_state->page_size);

    _dev_state->state |= STATE_BUFFER_CLEARED;
}

static void
_buffer_area_clear(int16_t col_start, int16_t col_end, int16_t row_start,
  int16_t row_end)
{
    _dev_state->state |= STATE_BUFFER_DIRTY;

    for (int16_t row = row_start; row < row_end; row++) {
        for (int16_t col = col_start; col < col_end; col++) {
            _pnd_clear(col, row);
        }
    }
}

static void
_buffer_line_clear(int16_t row)
{
    _dev_state->state |= STATE_BUFFER_DIRTY;

    for (int16_t col = 0; col < _dev_state->page_stride; col++) {
        _pnd_clear(col, row);
    }
}

static void
_buffer_line_partial_clear(int16_t col_start, int16_t col_end, int16_t row)
{
    _dev_state->state |= STATE_BUFFER_DIRTY;

    for (int16_t col = col_start; col < col_end; col++) {
        _pnd_clear(col, row);
    }
}

static void
_buffer_write(int16_t col, int16_t row, uint8_t ch)
{
    _dev_state->state |= STATE_BUFFER_DIRTY;
    _dev_state->state &= ~STATE_BUFFER_CLEARED;

    _pnd_write(col, row, _dev_state->font_charmap[ch].pnd);
}

static void
_dev_state_init(const dbgio_vdp2_t *params)
{
    assert(params != NULL);

    _dev_state = __malloc(sizeof(struct dev_state));
    assert(_dev_state != NULL);

    (void)memset(_dev_state, 0, sizeof(struct dev_state));

    _dev_state->state = STATE_IDLE;
    _dev_state->params = *params;
    _dev_state->default_params = (params == &_default_params);

#ifdef DEBUG
    if (!_dev_state->default_params) {
        assert(params->font != NULL);
        assert(params->font_charmap != NULL);
        assert(params->cell_format != NULL);
        assert(params->normal_map != NULL);
    }
#endif /* DEBUG */

    _dev_state->font_charmap = params->font_charmap;

    /* Nothing else is needed. Use a FILE to direct all writes via vfprintf() */
    _dev_state->file.write = _file_write;

    const vdp2_scrn_cell_format_t * const cell_format =
      params->cell_format;

    _dev_state->page_size = VDP2_SCRN_PAGE_SIZE_CALCULATE(cell_format);
    _dev_state->page_stride = VDP2_SCRN_PAGE_WIDTH_CALCULATE(cell_format);

    /* Restricting the page to 64x32 avoids wasting space */
    _dev_state->page_size >>= 1;

    _dev_state->page_ptr = __malloc(_dev_state->page_size);
    assert(_dev_state->page_ptr != NULL);

    _dev_state->tv_resolution = _state_vdp2()->tv.resolution;
}

static inline void __always_inline
_scroll_screen_init(const dbgio_vdp2_t *params)
{
    assert(params != NULL);
    assert(_dev_state != NULL);

    const vdp2_scrn_cell_format_t * const cell_format =
      params->cell_format;

    const vdp2_scrn_normal_map_t * const normal_map =
      params->normal_map;

    vdp2_scrn_cell_format_set(cell_format, normal_map);

    vdp2_scrn_priority_set(cell_format->scroll_screen, 7);
    vdp2_scrn_scroll_x_set(cell_format->scroll_screen, FIX16(0.0f));
    vdp2_scrn_scroll_y_set(cell_format->scroll_screen, FIX16(0.0f));

    _shared_display_set(true);

    if (_dev_state->default_params) {
        vdp2_vram_cycp_t * const vram_cycp = vdp2_vram_cycp_get();
        vdp2_vram_cycp_bank_t * const cycp_bank =
          &vram_cycp->pt[DEFAULT_NBGX_CPD_BANK];

        cycp_bank->raw &= DEFAULT_NBGX_CYCP_MASK;
        cycp_bank->raw |= DEFAULT_NBGX_CYCP;

        /* If default params, generate the charmap */
        dbgio_vdp2_charmap_t * const font_charmap =
          __malloc(UINT8_MAX * sizeof(dbgio_vdp2_charmap_t));

        assert(font_charmap != NULL);

        vdp2_vram_t vram;
        vram = params->cell_format->cpd_base;

        const vdp2_cram_t cram = params->cell_format->palette_base;

        for (uint32_t i = 0; i < UINT8_MAX; i++) {
            font_charmap[i].pnd = DEFAULT_NBGX_PND(vram, cram);

            vram += 0x20;
        }

        _dev_state->font_charmap = font_charmap;
    }
}

static void
_shared_init(const dbgio_vdp2_t *params)
{
    _dev_state_init(params);

    uint16_t cols;
    uint16_t rows;

    _cons_dimensions_calculate(&cols, &rows);

    __cons_init(&_cons_ops, cols, rows);

    _scroll_screen_init(params);
}

static void
_shared_deinit(void)
{
    if (_dev_state == NULL) {
        return;
    }

    if ((_dev_state->state & STATE_INITIALIZED) != STATE_INITIALIZED) {
        return;
    }

    if (_dev_state->default_params) {
        __free((void *)_dev_state->font_charmap);
    }

    __free(_dev_state->page_ptr);
    __free(_dev_state);

    _dev_state = NULL;
}

static void
_shared_test(void)
{
    /* It's the best we can do for now. If the current buffer is marked for
     * flushing, we have to silently drop any calls to write to the buffer */
    const uint8_t state_mask =
      STATE_BUFFER_FLUSHING | STATE_BUFFER_FORCE_FLUSHING;

    if ((_dev_state->state & state_mask) == STATE_BUFFER_FLUSHING) {
        return;
    }

    uint16_vec2_t * const tv_resolution =
      &_dev_state->tv_resolution;
    const uint16_vec2_t * const vdp2_tv_resolution =
      &_state_vdp2()->tv.resolution;

    if ((tv_resolution->x != vdp2_tv_resolution->x) ||
        (tv_resolution->y != vdp2_tv_resolution->y)) {
        *tv_resolution = *vdp2_tv_resolution;

        uint16_t cols;
        uint16_t rows;

        _cons_dimensions_calculate(&cols, &rows);

        __cons_resize(cols, rows);
    }
}

static void
_shared_puts(const char *buffer)
{
    if (_dev_state == NULL) {
        return;
    }

    if ((_dev_state->state & STATE_INITIALIZED) != STATE_INITIALIZED) {
        return;
    }

    _shared_test();

    (void)fputs(buffer, &_dev_state->file);
}

static void
_shared_printf(const char *format, va_list ap)
{
    if (_dev_state == NULL) {
        return;
    }

    if ((_dev_state->state & STATE_INITIALIZED) != STATE_INITIALIZED) {
        return;
    }

    _shared_test();

    (void)vfprintf(&_dev_state->file, format, ap);
}

static void
_shared_font_load(void)
{
    if (_dev_state == NULL) {
        return;
    }

    if ((_dev_state->state & STATE_INITIALIZED) != STATE_INITIALIZED) {
        return;
    }

    const dbgio_vdp2_t * const params = &_dev_state->params;

    const vdp2_scrn_cell_format_t * const cell_format =
      params->cell_format;

    __dbgio_font_1bpp_4bpp_decompress(params->font, (void *)cell_format->cpd_base);

    (void)memcpy((void *)cell_format->palette_base, params->font->pal,
      params->font->pal_size);
}

static void
_shared_display_set(bool display)
{
    vdp2_scrn_disp_t disp;

    switch (_dev_state->params.cell_format->scroll_screen) {
    case VDP2_SCRN_NBG1:
        disp = VDP2_SCRN_DISPTP_NBG1;
        break;
    case VDP2_SCRN_NBG2:
        disp = VDP2_SCRN_DISPTP_NBG2;
        break;
    case VDP2_SCRN_NBG3:
        disp = VDP2_SCRN_DISPTP_NBG3;
        break;
    default:
    case VDP2_SCRN_NBG0:
        disp = VDP2_SCRN_DISPTP_NBG0;
        break;
    }

    vdp2_scrn_disp_t disp_mask;
    disp_mask = vdp2_scrn_display_get();

    if (display) {
        disp_mask |= disp;
    } else {
        disp_mask &= ~disp;
    }

    vdp2_scrn_display_set(disp_mask);
}

#include "vdp2.inc"
#include "vdp2-async.inc"
