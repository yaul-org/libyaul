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
        uint8_t state;

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
} dev_state_t;

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
        .scrn = VDP2_SCRN_NBG3,
        .cpd_bank = 3,
        .cpd_offset = 0x00000,
        .pnd_bank = 3,
        .pnd_offset = 2,
        .cpd_cycp = {
                .t0 = VDP2_VRAM_CYCP_PNDR_NBG3,
                .t1 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t2 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t3 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t4 = VDP2_VRAM_CYCP_CHPNDR_NBG3,
                .t5 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t6 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t7 = VDP2_VRAM_CYCP_NO_ACCESS
        },
        .pnd_cycp = {
                .t0 = VDP2_VRAM_CYCP_PNDR_NBG3,
                .t1 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t2 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t3 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t4 = VDP2_VRAM_CYCP_CHPNDR_NBG3,
                .t5 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t6 = VDP2_VRAM_CYCP_NO_ACCESS,
                .t7 = VDP2_VRAM_CYCP_NO_ACCESS
        },
        .cram_index = 0
};

/* Parameters set by the user */
static dbgio_vdp2_t _params;

/* Device state for both async and direct VDP2 devices */
static dev_state_t *_dev_state;

static void _buffer_clear(void);
static void _buffer_area_clear(int16_t, int16_t, int16_t, int16_t);
static void _buffer_line_clear(int16_t, int16_t, int16_t);
static void _buffer_write(int16_t, int16_t, uint8_t);

static const cons_ops_t _cons_ops = {
        .clear = _buffer_clear,
        .area_clear = _buffer_area_clear,
        .line_clear = _buffer_line_clear,
        .write = _buffer_write
};

static inline void __always_inline
_pnd_write(int16_t col, int16_t row, uint16_t value)
{
        int16_t offset;
        offset = col + (row * _dev_state->page_width);

        _dev_state->page_pnd[offset] = value;
}

static inline void __always_inline
_pnd_clear(int16_t col, int16_t row)
{
        _pnd_write(col, row, _dev_state->pnd_clear);
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

        struct cpu_dmac_cfg dmac_cfg  = {
                .channel = ch,
                .dst = (uint32_t)&_dev_state->page_pnd[0],
                .dst_mode = CPU_DMAC_DESTINATION_INCREMENT,
                .src_mode = CPU_DMAC_SOURCE_FIXED,
                .src = _dev_state->pnd_clear,
                .len = _dev_state->page_size,
                .stride = CPU_DMAC_STRIDE_2_BYTES,
                .bus_mode = CPU_DMAC_BUS_MODE_CYCLE_STEAL,
                .ihr = NULL
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
        pnd = VDP2_SCRN_PND_CONFIG_0(
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

#include "vdp2-async.inc"
#include "vdp2-simple.inc"
