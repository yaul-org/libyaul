/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <vdp2.h>

#include "../drivers.h"

static void cons_vdp2_write(struct cons *);

static struct scrn_cell_format _nbg3_format;

void
cons_vdp2_init(struct cons *cons)
{
        cons->write = cons_vdp2_write;

        /* We want to be in VBLANK */
        vdp2_tvmd_display_clear();

        _nbg3_format.scf_scroll_screen = SCRN_NBG3;
        _nbg3_format.scf_cc_count = SCRN_CCC_PALETTE_16;
        _nbg3_format.scf_character_size = 1 * 1;
        _nbg3_format.scf_pnd_size = 1; /* 1-word */
        _nbg3_format.scf_auxiliary_mode = 0;
        _nbg3_format.scf_cp_table = VRAM_ADDR_4MBIT(3, 0x00000);
        _nbg3_format.scf_color_palette = CRAM_MODE_1_OFFSET(0, 0, 0);
        _nbg3_format.scf_plane_size = 2 * 1;
        _nbg3_format.scf_map.plane_a = VRAM_ADDR_4MBIT(3, 0x04000);
        _nbg3_format.scf_map.plane_b = VRAM_ADDR_4MBIT(3, 0x08000);
        _nbg3_format.scf_map.plane_c = VRAM_ADDR_4MBIT(3, 0x04000);
        _nbg3_format.scf_map.plane_d = VRAM_ADDR_4MBIT(3, 0x08000);

        vdp2_scrn_cell_format_set(&_nbg3_format);
        vdp2_scrn_priority_set(SCRN_NBG3, 7);

        struct vram_ctl *vram_ctl;

        vram_ctl = vdp2_vram_control_get();
        /* VRAM cycle pattern register becomes valid only for T0~T3, and
         * becomes invalid for T4~T7 when changing screen resolution. */
        vram_ctl->vram_cycp.pt[3].t0 = VRAM_CTL_CYCP_PNDR_NBG3;
        vram_ctl->vram_cycp.pt[3].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[3].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[3].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[3].t4 = VRAM_CTL_CYCP_CHPNDR_NBG3;
        vram_ctl->vram_cycp.pt[3].t5 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[3].t6 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[3].t7 = VRAM_CTL_CYCP_NO_ACCESS;
        vdp2_vram_control_set(vram_ctl);

        /* Clear the first unpacked cell of the font */
        volatile uint16_t *cp;
        cp = (uint16_t *)_nbg3_format.scf_cp_table;
        uint16_t *font_16;
        font_16 = (uint16_t *)&font[0];
        uint32_t i;
        for (i = 0; i < (FONT_SIZE / sizeof(uint16_t)); i++) {
                cp[i] = font_16[i];
        }

        (void)memcpy((uint16_t *)_nbg3_format.scf_color_palette, palette,
            FONT_COLOR_COUNT * sizeof(uint16_t));

        /* Clear console (plane) */
        volatile uint16_t *a_page0;
        a_page0 = (uint16_t *)_nbg3_format.scf_map.plane_a;
        volatile uint16_t *a_page1;
        a_page1 = (uint16_t *)(_nbg3_format.scf_map.plane_a +
            SCRN_CALCULATE_PAGE_SIZE(&_nbg3_format));

        uint32_t page_width;
        page_width = SCRN_CALCULATE_PAGE_WIDTH(&_nbg3_format);
        uint32_t page_height;
        page_height = SCRN_CALCULATE_PAGE_HEIGHT(&_nbg3_format);

        uint16_t pnd;
        pnd = SCRN_PND_CONFIG_0(
                _nbg3_format.scf_cp_table,
                _nbg3_format.scf_color_palette,
                /* vf = */ 0,
                /* hf = */ 0);
        uint32_t page_y;
        for (page_y = 0; page_y < page_height; page_y++) {
                uint32_t page_x;
                for (page_x = 0; page_x < page_width; page_x++) {
                        a_page0[page_x + (page_y * page_width)] = pnd;
                        a_page1[page_x + (page_y * page_width)] = pnd;
                }
        }

        vdp2_scrn_display_set(SCRN_NBG3, /* transparent = */ true);
        /* Turn display back on */
        vdp2_tvmd_display_set();
}

static void
cons_vdp2_write(struct cons *cons)
{
        uint32_t page_width;
        page_width = SCRN_CALCULATE_PAGE_WIDTH(&_nbg3_format);

        volatile uint16_t *a_page0;
        a_page0 = (uint16_t *)_nbg3_format.scf_map.plane_a;
        volatile uint16_t *a_page1;
        a_page1 = (uint16_t *)(_nbg3_format.scf_map.plane_a +
            SCRN_CALCULATE_PAGE_SIZE(&_nbg3_format));

        uint32_t col;
        for (col = 0; col < cons->cols; col++) {
                uint32_t row;
                for (row = 0; row < cons->rows; row++) {
                        struct cons_buffer *cb;
                        cb = &cons->buffer[col + (row * cons->cols)];

                        uint16_t pnd;
                        pnd = SCRN_PND_CONFIG_0(
                                _nbg3_format.scf_cp_table + (cb->glyph * 0x20),
                                _nbg3_format.scf_color_palette,
                                /* vf = */ 0,
                                /* hf = */ 0);

                        if (col < page_width) {
                                a_page0[col + (row * page_width)] = pnd;
                        } else {
                                a_page1[(col - page_width) + (row * page_width)] = pnd;
                        }
                }
        }
}
