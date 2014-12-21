/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>

#include <vdp2.h>
#include <vdp2/pn.h>

#include "lut.h"

#define RGB888_TO_RGB555(r, g, b) ((((b) >> 3) << 10) | (((g) >> 3) << 5) | ((r) >> 3))

static uint32_t *line_scroll_tb = (uint32_t *)VRAM_ADDR_4MBIT(2, 0x1F000);

static uint16_t *_nbg1_planes[4] = {
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x1C000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x1C000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x1C000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x1C000)
};
static uint32_t *_nbg1_character = (uint32_t *)VRAM_ADDR_4MBIT(2, 0x1EFC0);

static void
line_scroll(void)
{
        static uint16_t ofs = 0;

        struct scrn_ls_format ls;

        ls.ls_scrn = SCRN_NBG1;
        ls.ls_lsta = (uint32_t)line_scroll_tb | (ofs & (2048 - 1));
        ls.ls_int = 0;
        ls.ls_fun = SCRN_LS_N1SCX;

        vdp2_scrn_ls_set(&ls);
        ofs += 4;
}

static void
line_scroll_screen(void)
{
        struct scrn_cell_format nbg1_format;
        struct vram_ctl *vram_ctl;

        nbg1_format.scf_scroll_screen = SCRN_NBG1;
        nbg1_format.scf_cc_count = SCRN_CCC_PALETTE_16;
        nbg1_format.scf_character_size= 1 * 1;
        nbg1_format.scf_pnd_size = 1; /* 1 word */
        nbg1_format.scf_auxiliary_mode = 1;
        nbg1_format.scf_cp_table = (uint32_t)_nbg1_character;
        nbg1_format.scf_plane_size = 1 * 1;
        nbg1_format.scf_map.plane_a = (uint32_t)_nbg1_planes[0];
        nbg1_format.scf_map.plane_b = (uint32_t)_nbg1_planes[1];
        nbg1_format.scf_map.plane_c = (uint32_t)_nbg1_planes[2];
        nbg1_format.scf_map.plane_d = (uint32_t)_nbg1_planes[3];

        vdp2_scrn_cell_format_set(&nbg1_format);
        vdp2_priority_spn_set(SCRN_NBG1, 7);

        vram_ctl = vdp2_vram_control_get();
        vram_ctl->vram_cycp.pt[2].t7 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[2].t6 = VRAM_CTL_CYCP_PNDR_NBG1;
        vram_ctl->vram_cycp.pt[2].t5 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t4 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t0 = VRAM_CTL_CYCP_NO_ACCESS;
        vdp2_vram_control_set(vram_ctl);

        /* Set palette */
        {
                uint16_t *cram;
                cram = (uint16_t *)CRAM_BANK(0, 0);

                /* Black color #0 */
                *cram++ = RGB888_TO_RGB555(0, 0, 0);
                /* White color #1 */
                *cram = RGB888_TO_RGB555(255, 255, 255);
        }

        /* Draw cells */
        {
                uint32_t *vram;
                vram = _nbg1_character;

                uint32_t i;

                *vram++ = 0x11111111;
                for (i = 1; i < 7; i++) {
                        *vram++ = 0x10000001;
                }
                *vram++ = 0x11111111;
                for (i = 0; i < 8; i++) {
                        *vram++ = 0x00000000;
                }
        }

        /* Set up the map */
        {
                uint16_t *pnt;
                pnt = (uint16_t *)VRAM_ADDR_4MBIT(2, 0x1C000);

                uint16_t number;
                number = VDP2_PN_CONFIG_1_CHARACTER_NUMBER((uint32_t)_nbg1_character);

                uint32_t x;
                uint32_t y;
                for (y = 0; y < 64; y++) {
                        for (x = 0; x < 64; x++) {
                                pnt[x + (y << 6)] = number | ((x + y) % 2);
                        }
                }
        }

        /* Set up the line scroll table */
        {
                uint32_t y;
                for (y = 0; y < 512; y++) {
                        line_scroll_tb[y] = (lut[y]) << 16;
                        line_scroll_tb[y + 512] = (lut[y]) << 16;
                }
        }

        vdp2_scrn_display_set(SCRN_NBG1, /* no_trans = */ false);
}

int
main(void)
{
        uint16_t blcs_color[] = {
                RGB888_TO_RGB555(255, 0, 0)
        };

        vdp2_init();
        vdp2_scrn_back_screen_set(/* single_color = */ true,
            VRAM_ADDR_4MBIT(3, 0x1FFFE), blcs_color, 0);

        line_scroll_screen();

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                line_scroll();
        }

        return 0;
}
