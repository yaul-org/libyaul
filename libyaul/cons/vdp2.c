/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <vdp2/cram.h>
#include <vdp2/pn.h>
#include <vdp2/priority.h>
#include <vdp2/scrn.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "cons.h"

/* The dimensions of cons is 40x28. Each cell is 32-bytes. Therefore,
 * 40*28*(32-bytes) (0x8C00 bytes) is needed for the character pattern
 * data. So, offset 0xA000 is on a 0x2000 byte boundary. */
static uint16_t *_nbg3_planes[4] = {
        /* VRAM B1 */
        (uint16_t *)VRAM_ADDR_4MBIT(3, 0x0A000),
        /* VRAM B1 */
        (uint16_t *)VRAM_ADDR_4MBIT(3, 0x0A000),
        /* VRAM B1 */
        (uint16_t *)VRAM_ADDR_4MBIT(3, 0x0A000),
        /* VRAM B1 */
        (uint16_t *)VRAM_ADDR_4MBIT(3, 0x0A000)
};
/* CRAM */
static uint32_t *_nbg3_color_palette = (uint32_t *)CRAM_BANK(63, 0);
/* VRAM B1 */
static uint32_t *_nbg3_character_pattern = (uint32_t *)VRAM_ADDR_4MBIT(3, 0x00020);

static uint32_t _character_no = 0;

static void cons_vdp2_clear(struct cons *, int32_t, int32_t, int32_t, int32_t);
static void cons_vdp2_reset(struct cons *);
static void cons_vdp2_scroll(struct cons *);
static void cons_vdp2_write(struct cons *, int, uint8_t, uint8_t);

void
cons_vdp2_init(struct cons *cons)
{
        struct scrn_cell_format nbg3_format;
        struct vram_ctl *vram_ctl;

        uint32_t y;

        cons->clear = cons_vdp2_clear;
        cons->reset = cons_vdp2_reset;
        cons->scroll = cons_vdp2_scroll;
        cons->write = cons_vdp2_write;

        cons_reset(cons);

        /* We want to be in VBLANK */
        vdp2_tvmd_display_clear();

        nbg3_format.scf_scrn = SCRN_NBG3;
        nbg3_format.scf_cc_count = SCRN_CCC_PALETTE_16;
        nbg3_format.scf_character_size = 1 * 1;
        nbg3_format.scf_pnd_size = 1; /* 1-word */
        nbg3_format.scf_auxiliary_mode = 1;
        nbg3_format.scf_cp_table = (uint32_t)_nbg3_character_pattern;
        nbg3_format.scf_color_palette = (uint32_t)_nbg3_color_palette;
        nbg3_format.scf_plane_size = 1 * 1;
        nbg3_format.scf_map.plane_a = (uint32_t)_nbg3_planes[0];
        nbg3_format.scf_map.plane_b = (uint32_t)_nbg3_planes[1];
        nbg3_format.scf_map.plane_c = (uint32_t)_nbg3_planes[2];
        nbg3_format.scf_map.plane_d = (uint32_t)_nbg3_planes[3];

        vdp2_scrn_cell_format_set(&nbg3_format);
        vdp2_priority_spn_set(SCRN_NBG3, 7);

        vram_ctl = vdp2_vram_control_get();
        vram_ctl->vram_cycp.pt[3].t7 = VRAM_CTL_CYCP_CHPNDR_NBG3;
        vram_ctl->vram_cycp.pt[3].t6 = VRAM_CTL_CYCP_PNDR_NBG3;
        vram_ctl->vram_cycp.pt[3].t5 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t4 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t3 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t2 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t1 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t0 = VRAM_CTL_CYCP_CPU_RW;
        vdp2_vram_control_set(vram_ctl);

        /* Clear the first unpacked cell of the font */
        for (y = 0; y < FONT_H; y++) {
                _nbg3_character_pattern[y] = font_unpacked[0];
        }

        memcpy(_nbg3_color_palette, palette, FONT_NCOLORS * sizeof(uint16_t));

        /* Clear */
        cons->clear(cons, 0, COLS, 0, ROWS);

        vdp2_scrn_display_set(SCRN_NBG3, /* transparent = */ true);
        vdp2_tvmd_display_set();
}

static void
cons_vdp2_clear(struct cons *cons, int32_t col_start, int32_t col_end,
    int32_t row_start, int32_t row_end)
{
        uint16_t character_number;
        uint16_t palette_number;

        character_number = VDP2_PN_CONFIG_1_CHARACTER_NUMBER((uint32_t)_nbg3_character_pattern);
        palette_number = VDP2_PN_CONFIG_1_PALETTE_NUMBER((uint32_t)_nbg3_color_palette);

        int32_t col;
        int32_t row;

        for (row = row_start; row < row_end; row++) {
                for (col = col_start; col < col_end; col++) {
                        _nbg3_planes[0][col + (row << 6)] =
                            character_number | palette_number;
                }
        }
}

static void
cons_vdp2_reset(struct cons *cons)
{
        _character_no = ((cons->cursor.row % ROWS) * COLS) + cons->cursor.col + 1;
}

static void
cons_vdp2_scroll(struct cons *cons __attribute__ ((unused)))
{
        static int32_t scvy = 0;

        scvy += FONT_H;
        vdp2_scrn_scv_y_set(SCRN_NBG3, scvy, 0);
}

static void
cons_vdp2_write(struct cons *cons, int c, uint8_t fg, uint8_t bg)
{
        static const uint32_t color_tbl[] = {
                0x00000000, 0x11111111, 0x22222222, 0x33333333,
                0x44444444, 0x55555555, 0x66666666, 0x77777777,
                0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB,
                0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF
        };

        uint32_t fg_mask;
        uint32_t bg_mask;

        fg_mask = color_tbl[fg];
        bg_mask = color_tbl[bg];

        /* Expand cell */
        int32_t y;

        for (y = FONT_H - 1; y >= 0; y--) {
                uint32_t row;

                row = font_unpacked[font[y + (c << 3)]];

                _nbg3_character_pattern[y + (_character_no << 3)] =
                    (row & fg_mask) | ((row & bg_mask) ^ bg_mask);
        }

        uint16_t character_number;
        uint16_t palette_number;

        character_number = VDP2_PN_CONFIG_1_CHARACTER_NUMBER((uint32_t)_nbg3_character_pattern);
        palette_number = VDP2_PN_CONFIG_1_PALETTE_NUMBER((uint32_t)_nbg3_color_palette);

        _nbg3_planes[0][cons->cursor.col + (cons->cursor.row << 6)] =
            (character_number + _character_no) | palette_number;

        _character_no++;

        if ((_character_no % (ROWS * COLS)) == 0) {
                _character_no = 1;
        }
}
