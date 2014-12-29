/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <vdp2.h>

#include "cons.h"

/* The dimensions of cons is 40x28. Each cell is 32-bytes. Therefore,
 * 40*28*(32-bytes) (0x8C00 bytes) is needed for the character pattern
 * data. So, offset 0xA000 is on a 0x2000 byte boundary. */
static uint16_t *_nbg3_planes[4] = {
        /* VRAM B1 */
        (uint16_t *)VRAM_ADDR_4MBIT(3, 0x02000),
        /* VRAM B1 */
        (uint16_t *)VRAM_ADDR_4MBIT(3, 0x02000),
        /* VRAM B1 */
        (uint16_t *)VRAM_ADDR_4MBIT(3, 0x02000),
        /* VRAM B1 */
        (uint16_t *)VRAM_ADDR_4MBIT(3, 0x02000)
};
/* CRAM */
static uint32_t *_nbg3_color_palette = (uint32_t *)CRAM_NBG3_OFFSET(0, 0, 0);
static uint16_t _nbg3_character_number =
    VDP2_PN_CONFIG_1_CHARACTER_NUMBER(VRAM_ADDR_4MBIT(3, 0x00000));
/* VRAM B1 */
static uint32_t *_nbg3_character_pattern =
    (uint32_t *)VRAM_ADDR_4MBIT(3, 0x00000);
static uint16_t _nbg3_palette_number =
    VDP2_PN_CONFIG_1_PALETTE_NUMBER(CRAM_NBG3_OFFSET(0, 0, 0));

static void cons_vdp2_reset(struct cons *);
static void cons_vdp2_write(struct cons *);

void
cons_vdp2_init(struct cons *cons)
{

        cons->write = cons_vdp2_write;

        /* We want to be in VBLANK */
        vdp2_tvmd_display_clear();

        struct scrn_cell_format nbg3_format;

        nbg3_format.scf_scroll_screen = SCRN_NBG3;
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

        struct vram_ctl *vram_ctl;

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
        uint32_t i;

        for (i = 0; i < (8192 / sizeof(_nbg3_character_pattern)); i++) {
                _nbg3_character_pattern[i] = font[i];
        }

        memcpy(_nbg3_color_palette, palette, FONT_NCOLORS * sizeof(uint16_t));

        /* Clear */
        int32_t col;
        int32_t row;

        for (row = 0; row < CONS_ROWS; row++) {
                for (col = 0; col < CONS_COLS; col++) {
                        _nbg3_planes[0][col + (row << 6)] =
                            _nbg3_character_number | _nbg3_palette_number;
                }
        }

        vdp2_scrn_display_set(SCRN_NBG3, /* transparent = */ true);
        vdp2_tvmd_display_set();
}

static void
cons_vdp2_write(struct cons *cons)
{
        uint32_t col;
        uint32_t row;

        for (col = 0; col < CONS_COLS; col++) {
                for (row = 0; row < CONS_ROWS; row++) {
                        struct cons_buffer *cons_buffer;

                        cons_buffer = &cons->buffer[col + (row * CONS_COLS)];

                        uint16_t character_number;
                        character_number =
                            _nbg3_character_number + cons_buffer->glyph;
                        _nbg3_planes[0][col + (row << 6)] =
                            character_number | _nbg3_palette_number;
                }
        }
}
