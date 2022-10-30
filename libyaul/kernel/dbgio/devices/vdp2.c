/*
 * Copyright (c) 2012-2019 Israel Jacquez
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
#include <cpu/intc.h>

#include <dbgio/dbgio.h>

#include <vdp.h>

#include <sys/dma-queue.h>

#include <internal.h>
#include <dbgio/dbgio-internal.h>
#include <vdp-internal.h>

#include "cons/cons.h"

#include "vdp2_font.inc"

#define STATE_IDLE                  (0x00)
#define STATE_INITIALIZED           (0x01)
#define STATE_BUFFER_DIRTY          (0x02)
#define STATE_BUFFER_CLEARED        (0x04)
#define STATE_BUFFER_FLUSHING       (0x08)
#define STATE_BUFFER_FORCE_FLUSHING (0x10)

/* CPU-DMAC channel used for _flush() and _buffer_clear() */
#define DEV_DMAC_CHANNEL 0

struct dev_state {
        uint8_t state;

        dbgio_vdp2_t params;

        uint16_t *page_ptr;
        uint16_t page_size; /* Size of split page */
        uint16_t page_stride;

        int16_vec2_t tv_resolution;
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
        .cpd_base      = VDP2_VRAM_ADDR(3, 0x1C000),
        .palette_base  = VDP2_CRAM_MODE_0_OFFSET(0, 127, 0)
};

static const vdp2_scrn_normal_map_t _default_normal_map = {
        .plane_a = (vdp2_vram_t)VDP2_VRAM_ADDR(3, 0x1E000),
        .plane_b = (vdp2_vram_t)VDP2_VRAM_ADDR(3, 0x1E000),
        .plane_c = (vdp2_vram_t)VDP2_VRAM_ADDR(3, 0x1E000),
        .plane_d = (vdp2_vram_t)VDP2_VRAM_ADDR(3, 0x1E000)
};

static const dbgio_vdp2_t _default_params = {
        .font_charmap = _font_charmap,
        .font_cpd     = _font_cpd,
        .font_pal     = _font_pal,
        .font_fg      = 7,
        .font_bg      = 0,

        .cell_format  = &_default_cell_format,
        .normal_map   = &_default_normal_map,
        .cycp_cpd     = 0,
        .cycp_pnd     = 4
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

static void _vram_cycp_timing_set(vdp2_vram_cycp_bank_t *cycp_bank, uint8_t value, uint8_t timing);

static inline void __always_inline
_pnd_write(int16_t col, int16_t row, uint16_t value)
{
        const uint32_t offset = col + (row * _dev_state->page_stride);

        _dev_state->page_ptr[offset] = value;
}

static inline void __always_inline
_pnd_clear(int16_t col, int16_t row)
{
        _pnd_write(col, row, _dev_state->params.font_charmap['\0'].pnd);
}

static void
_buffer_clear(void)
{
        /* Don't try to clear the buffer again if it's already been cleared */
        if ((_dev_state->state & STATE_BUFFER_CLEARED) == STATE_BUFFER_CLEARED) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_DIRTY;

        cpu_dmac_memset(DEV_DMAC_CHANNEL,
            _dev_state->page_ptr,
            _dev_state->params.font_charmap['\0'].pnd,
            _dev_state->page_size);

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

        _pnd_write(col, row, _dev_state->params.font_charmap[ch].pnd);
}

static inline uint8_t __always_inline
_1bpp_4bpp_convert(const uint8_t byte, const uint8_t *fgbg)
{
        const uint8_t ubyte = (byte >> 1) & 0x01;
        const uint8_t lbyte = byte & 0x01;

        return (fgbg[lbyte] << 4) | fgbg[ubyte];
}

static void
_font_1bpp_4bpp_decompress(uint8_t *dec_cpd, const uint8_t *cmp_cpd,
    const uint8_t fg, const uint8_t bg)
{
        assert(dec_cpd != NULL);
        assert(cmp_cpd != NULL);
        assert(((uintptr_t)cmp_cpd & 0x00000003) == 0x00000000);

        const uint8_t fgbg[] = {
                bg & 0x0F,
                fg & 0x0F
        };

        for (uint32_t i = 0, j = 0; i < FONT_1BPP_CPD_SIZE; i++) {
                uint8_t cpd;
                cpd = cmp_cpd[i];

                dec_cpd[j + 0] = _1bpp_4bpp_convert(cpd, fgbg);
                cpd >>= 2;
                dec_cpd[j + 1] = _1bpp_4bpp_convert(cpd, fgbg);
                cpd >>= 2;
                dec_cpd[j + 2] = _1bpp_4bpp_convert(cpd, fgbg);
                cpd >>= 2;
                dec_cpd[j + 3] = _1bpp_4bpp_convert(cpd, fgbg);

                j += 4;
        }
}

static void
_dev_state_init(const dbgio_vdp2_t *params)
{
        _dev_state = __malloc(sizeof(struct dev_state));
        assert(_dev_state != NULL);

        (void)memset(_dev_state, 0, sizeof(struct dev_state));

        _dev_state->state = STATE_IDLE;
        _dev_state->params = *params;

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

        uint32_t scroll_screen;

        vdp2_scrn_disp_t disp_mask;
        disp_mask = vdp2_scrn_display_get();

        switch (cell_format->scroll_screen) {
        case VDP2_SCRN_NBG1:
                scroll_screen = 1;
                disp_mask |= VDP2_SCRN_DISP_NBG1;
                break;
        case VDP2_SCRN_NBG2:
                scroll_screen = 2;
                disp_mask |= VDP2_SCRN_DISP_NBG2;
                break;
        case VDP2_SCRN_NBG3:
                scroll_screen = 3;
                disp_mask |= VDP2_SCRN_DISP_NBG3;
                break;
        default:
        case VDP2_SCRN_NBG0:
                scroll_screen = 0;
                disp_mask |= VDP2_SCRN_DISP_NBG0;
                break;
        }

        vdp2_scrn_display_set(disp_mask);

        const uint32_t cpd_bank =
            VDP2_VRAM_BANK(cell_format->cpd_base);
        const uint32_t pnd_bank =
            VDP2_VRAM_BANK(normal_map->plane_a);

        vdp2_vram_cycp_t * const vram_cycp = vdp2_vram_cycp_get();

        vdp2_vram_cycp_bank_t * const cycp_cpd_bank = &vram_cycp->pt[cpd_bank];
        const uint8_t cpd_value = VDP2_VRAM_CYCP_CHPNDR(scroll_screen);

        _vram_cycp_timing_set(cycp_cpd_bank, cpd_value, params->cycp_cpd);

        vdp2_vram_cycp_bank_t * const cycp_pnd_bank = &vram_cycp->pt[pnd_bank];
        const uint8_t pnd_value = VDP2_VRAM_CYCP_PNDR(scroll_screen);

        _vram_cycp_timing_set(cycp_pnd_bank, pnd_value, params->cycp_pnd);
}

static void
_vram_cycp_timing_set(vdp2_vram_cycp_bank_t *cycp_bank, uint8_t value, uint8_t timing)
{
        switch (timing) {
        case 0:
                cycp_bank->raw &= 0xFFFFFFF0;
                cycp_bank->raw |= value;
                break;
        case 1:
                cycp_bank->raw &= 0xFFFFFF0F;
                cycp_bank->raw |= value << 4;
                break;
        case 2:
                cycp_bank->raw &= 0xFFFFF0FF;
                cycp_bank->raw |= value << 8;
                break;
        case 3:
                cycp_bank->raw &= 0xFFFF0FFF;
                cycp_bank->raw |= value << 12;
                break;
        case 4:
                cycp_bank->raw &= 0xFFF0FFFF;
                cycp_bank->raw |= value << 16;
                break;
        case 5:
                cycp_bank->raw &= 0xFF0FFFFF;
                cycp_bank->raw |= value << 20;
                break;
        case 6:
                cycp_bank->raw &= 0xF0FFFFFF;
                cycp_bank->raw |= value << 24;
                break;
        case 7:
                cycp_bank->raw &= 0x0FFFFFFF;
                cycp_bank->raw |= value << 28;
                break;
        }
}

static void
_shared_init(const dbgio_vdp2_t *params)
{
        _dev_state_init(params);

        const uint16_t cols = _dev_state->tv_resolution.x / FONT_CHAR_WIDTH;
        const uint16_t rows = _dev_state->tv_resolution.y / FONT_CHAR_HEIGHT;

        cons_init(&_cons_ops, cols, rows);

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

        __free(_dev_state->page_ptr);

        __free(_dev_state);

        _dev_state = NULL;
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

        /* It's the best we can do for now. If the current buffer is marked for
         * flushing, we have to silently drop any calls to write to the
         * buffer */
        const uint8_t state_mask =
            STATE_BUFFER_FLUSHING | STATE_BUFFER_FORCE_FLUSHING;

        if ((_dev_state->state & state_mask) == STATE_BUFFER_FLUSHING) {
                return;
        }

        int16_vec2_t * const tv_resolution =
            &_dev_state->tv_resolution;
        const int16_vec2_t * const vdp2_tv_resolution =
            &_state_vdp2()->tv.resolution;

        if ((tv_resolution->x != vdp2_tv_resolution->x) ||
            (tv_resolution->y != vdp2_tv_resolution->y)) {
                *tv_resolution = *vdp2_tv_resolution;

                const uint16_t cols = tv_resolution->x / FONT_CHAR_WIDTH;
                const uint16_t rows = tv_resolution->y / FONT_CHAR_HEIGHT;

                cons_resize(cols, rows);
        }

        cons_buffer(buffer);
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

        _font_1bpp_4bpp_decompress((void *)cell_format->cpd_base,
            params->font_cpd,
            params->font_fg,
            params->font_bg);

        (void)memcpy((void *)cell_format->palette_base,
            params->font_pal,
            FONT_4BPP_COLOR_COUNT * sizeof(rgb1555_t));
}

#include "vdp2.inc"
#include "vdp2-async.inc"
