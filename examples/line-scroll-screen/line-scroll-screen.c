/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>

#include <vdp2.h>
#include <vdp2/pn.h>

#include "lut_sin.h"

#define RGB(r, g, b) (0x8000 | ((r) & 0x001F) | (((g) & 0x001F)  << 5) | (((b) & 0x001F) << 10))

static uint32_t *line_scroll_tb = (uint32_t *)VRAM_ADDR_4MBIT(2, 0x1F000);

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
        struct scrn_ch_format cfg;
        struct vram_ctl *vram_ctl;

        uint16_t *cram;
        uint16_t *pnt;
        uint32_t *character;
        uint16_t pntv;
        uint32_t x;
        uint32_t y;
        uint32_t character_ofs;

        character_ofs = 0x1EFC0;
        character = (uint32_t *)VRAM_ADDR_4MBIT(2, character_ofs);
        pnt = (uint16_t *)VRAM_ADDR_4MBIT(2, 0x1C000);
        cram = (uint16_t *)CRAM_BANK(0, 0);

        cfg.ch_scrn = SCRN_NBG1;
        cfg.ch_cs = 1 * 1; /* 1x1 cells */
        cfg.ch_pnds = 1; /* 1 word */
        cfg.ch_cnsm = 1; /* Character number supplement mode: 1 */
        cfg.ch_sp = 0;
        cfg.ch_scc = 0;
        cfg.ch_spn = 0;
        cfg.ch_scn = (uint32_t)character;
        cfg.ch_pls = 1 * 1; /* 1x1 plane size */
        cfg.ch_map[0] = (uint32_t)pnt;
        cfg.ch_map[1] = (uint32_t)pnt;
        cfg.ch_map[2] = (uint32_t)pnt;
        cfg.ch_map[3] = (uint32_t)pnt;

        vdp2_scrn_ccc_set(SCRN_NBG1, SCRN_CCC_CHC_16);
        vdp2_scrn_ch_format_set(&cfg);
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

        /* Black color #0 */
        *cram++ = RGB(0, 0, 0);
        /* White color #1 */
        *cram = RGB(31, 31, 31);

        /* Draw two tiles */
        y = 0;
        *character++ = 0x11111111;
        for (y = 1; y < 7; y++)
                *character++ = 0x10000001;
        *character++ = 0x11111111;
        for (y = 0; y < 8; y++)
                *character++ = 0x00000000;
        character = (uint32_t *)VRAM_ADDR_4MBIT(2, character_ofs);

        /* Set up the map */
        for (y = 0; y < 64; y++) {
                for (x = 0; x < 64; x++) {
                        pntv = PN_CHARACTER_NO((uint32_t)character);
                        pnt[x + (y << 6)] = pntv | ((x + y) % 2);
                }
        }

        for (y = 0; y < 512; y++) {
                line_scroll_tb[y] = (lut_sin[y]) << 16;
                line_scroll_tb[y + 512] = (lut_sin[y]) << 16;
        }

        vdp2_scrn_display_set(SCRN_NBG1, /* no_trans = */ false);
}

int
main(void)
{
        uint16_t blcs_color[] = {
                RGB(0, 15, 15)
        };

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        line_scroll_screen();

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                line_scroll();
        }

        return 0;
}
