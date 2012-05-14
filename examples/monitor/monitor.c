/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include <vdp2.h>
#include <vdp2/pn.h>
#include <smpc.h>
#include <smpc/smc.h>
#include <smpc/peripheral.h>

#include <monitor/monitor.h>

#include "lut.h"

#define RGB(r, g, b)    (0x8000 | ((r) & 0x001F) | (((g) & 0x001F)  << 5) | (((b) & 0x001F) << 10))

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
bitmap_screen(void)
{
        struct scrn_bm_format cfg;
        struct vram_ctl *vram_ctl;

        cfg.bm_scrn = SCRN_NBG0;
        cfg.bm_bs = SCRN_BM_BMSZ_512_256;
        cfg.bm_sp = 0;
        cfg.bm_scc = 0;
        cfg.bm_spn = 0;

        vdp2_scrn_ccc_set(SCRN_NBG0, SCRN_CCC_CHC_32768);
        vdp2_scrn_bm_format_set(&cfg);
        vdp2_priority_spn_set(SCRN_NBG0, 1);

        vram_ctl = vdp2_vram_control_get();
        vram_ctl->vram_cycp.pt[1].t7 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t6 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t5 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[1].t4 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[1].t3 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[1].t2 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[1].t1 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[1].t0 = VRAM_CTL_CYCP_CPU_RW;
        vdp2_vram_control_set(vram_ctl);

        vdp2_scrn_display_set(SCRN_NBG0, /* no_trans = */ false);
}

static void
bitmap_update_screen(void)
{
        uint8_t r;
        uint8_t g;
        uint8_t b;

        uint32_t x;
        uint32_t y;
        static uint32_t z = 0;

        uint16_t *bitmap;

        bitmap = (uint16_t *)VRAM_ADDR_4MBIT(0, 0x00000);
        for (y = 0; y < 256; y++) {
                for (x = 0; x < 512; x++) {
                        r = lut_sin[(0) & 0x1FF];
                        g = lut_sin[(0) & 0x1FF];
                        b = lut_sin[(0) & 0x1FF];
                        bitmap[x + (y << 9)] = RGB((x * y) % 15, x % 15, x % 15);
                }
        }
        z++;
        z &= 16;
}

static void
line_scroll_screen(void)
{
        struct scrn_ch_format cfg;
        struct vram_ctl *vram_ctl;

        uint16_t *pnt;
        uint32_t *character;
        uint32_t x;
        uint32_t y;
        uint32_t character_ofs;

        character_ofs = 0x1EFC0;
        character = (uint32_t *)VRAM_ADDR_4MBIT(2, character_ofs);
        pnt = (uint16_t *)VRAM_ADDR_4MBIT(2, 0x1C000);

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

        vdp2_scrn_ccc_set(SCRN_NBG2, SCRN_CCC_CHC_16);
        vdp2_priority_spn_set(SCRN_NBG1, 1);
        vdp2_scrn_ch_format_set(&cfg);

        vram_ctl = vdp2_vram_control_get();
        vram_ctl->vram_cycp.pt[2].t7 = VRAM_CTL_CYCP_PNDR_NBG1;
        vram_ctl->vram_cycp.pt[2].t6 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[2].t5 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[2].t4 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[2].t3 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[2].t2 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[2].t1 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[2].t0 = VRAM_CTL_CYCP_CPU_RW;
        vdp2_vram_control_set(vram_ctl);

        /* Draw two tiles */
        y = 0;
        *character++ = 0xFFFFFFFF;
        for (y = 1; y < 7; y++)
                *character++ = 0xF00FF00F;
        *character++ = 0xFFFFFFFF;
        for (y = 0; y < 8; y++)
                *character++ = 0x00000000;
        character = (uint32_t *)VRAM_ADDR_4MBIT(2, character_ofs);

        /* Set up the map */
        for (y = 0; y < 64; y++) {
                for (x = 0; x < 64; x++)
                        pnt[x + (y << 6)] = PN_CHARACTER_NO((uint32_t)character) | ((x + y) % 2);
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
        static char text[4096];

        /* Exactly 224 entries (one for every scanline */
        static uint16_t blcs_color[] = {
                RGB(0x00, 0x08, 0x00), RGB(0x00, 0x08, 0x01), RGB(0x01, 0x08, 0x02), RGB(0x01, 0x08, 0x03),
                RGB(0x02, 0x08, 0x04), RGB(0x02, 0x08, 0x05), RGB(0x03, 0x08, 0x06), RGB(0x03, 0x08, 0x07),
                RGB(0x04, 0x08, 0x08), RGB(0x04, 0x08, 0x09), RGB(0x05, 0x08, 0x0A), RGB(0x05, 0x08, 0x0B),
                RGB(0x06, 0x08, 0x0C), RGB(0x06, 0x08, 0x0D), RGB(0x07, 0x08, 0x0E), RGB(0x07, 0x08, 0x0F),
                RGB(0x08, 0x08, 0x10), RGB(0x08, 0x08, 0x11), RGB(0x09, 0x08, 0x12), RGB(0x09, 0x08, 0x13),
                RGB(0x0A, 0x08, 0x14), RGB(0x0A, 0x08, 0x15), RGB(0x0B, 0x08, 0x16), RGB(0x0B, 0x08, 0x17),
                RGB(0x0C, 0x08, 0x18), RGB(0x0C, 0x08, 0x19), RGB(0x0D, 0x08, 0x1A), RGB(0x0D, 0x08, 0x1B),
                RGB(0x0E, 0x08, 0x1C), RGB(0x0E, 0x08, 0x1D), RGB(0x0F, 0x08, 0x1E), RGB(0x0F, 0x08, 0x1F),
                RGB(0x00, 0x08, 0x1F), RGB(0x00, 0x08, 0x1E), RGB(0x01, 0x08, 0x1D), RGB(0x01, 0x08, 0x1C),
                RGB(0x02, 0x08, 0x1B), RGB(0x02, 0x08, 0x1A), RGB(0x03, 0x08, 0x19), RGB(0x03, 0x08, 0x18),
                RGB(0x04, 0x08, 0x17), RGB(0x04, 0x08, 0x16), RGB(0x05, 0x08, 0x15), RGB(0x05, 0x08, 0x14),
                RGB(0x06, 0x08, 0x13), RGB(0x06, 0x08, 0x12), RGB(0x07, 0x08, 0x11), RGB(0x07, 0x08, 0x10),
                RGB(0x08, 0x08, 0x0F), RGB(0x08, 0x08, 0x0E), RGB(0x09, 0x08, 0x0D), RGB(0x09, 0x08, 0x0C),
                RGB(0x0A, 0x08, 0x0B), RGB(0x0A, 0x08, 0x0A), RGB(0x0B, 0x08, 0x09), RGB(0x0B, 0x08, 0x08),
                RGB(0x0C, 0x08, 0x07), RGB(0x0C, 0x08, 0x06), RGB(0x0D, 0x08, 0x05), RGB(0x0D, 0x08, 0x04),
                RGB(0x0E, 0x08, 0x03), RGB(0x0E, 0x08, 0x02), RGB(0x0F, 0x08, 0x01), RGB(0x0F, 0x08, 0x00),
                RGB(0x00, 0x08, 0x00), RGB(0x00, 0x08, 0x01), RGB(0x01, 0x08, 0x02), RGB(0x01, 0x08, 0x03),
                RGB(0x02, 0x08, 0x04), RGB(0x02, 0x08, 0x05), RGB(0x03, 0x08, 0x06), RGB(0x03, 0x08, 0x07),
                RGB(0x04, 0x08, 0x08), RGB(0x04, 0x08, 0x09), RGB(0x05, 0x08, 0x0A), RGB(0x05, 0x08, 0x0B),
                RGB(0x06, 0x08, 0x0C), RGB(0x06, 0x08, 0x0D), RGB(0x07, 0x08, 0x0E), RGB(0x07, 0x08, 0x0F),
                RGB(0x08, 0x08, 0x10), RGB(0x08, 0x08, 0x11), RGB(0x09, 0x08, 0x12), RGB(0x09, 0x08, 0x13),
                RGB(0x0A, 0x08, 0x14), RGB(0x0A, 0x08, 0x15), RGB(0x0B, 0x08, 0x16), RGB(0x0B, 0x08, 0x17),
                RGB(0x0C, 0x08, 0x18), RGB(0x0C, 0x08, 0x19), RGB(0x0D, 0x08, 0x1A), RGB(0x0D, 0x08, 0x1B),
                RGB(0x0E, 0x08, 0x1C), RGB(0x0E, 0x08, 0x1D), RGB(0x0F, 0x08, 0x1E), RGB(0x0F, 0x08, 0x1F),
                RGB(0x00, 0x08, 0x1F), RGB(0x00, 0x08, 0x1E), RGB(0x01, 0x08, 0x1D), RGB(0x01, 0x08, 0x1C),
                RGB(0x02, 0x08, 0x1B), RGB(0x02, 0x08, 0x1A), RGB(0x03, 0x08, 0x19), RGB(0x03, 0x08, 0x18),
                RGB(0x04, 0x08, 0x17), RGB(0x04, 0x08, 0x16), RGB(0x05, 0x08, 0x15), RGB(0x05, 0x08, 0x14),
                RGB(0x06, 0x08, 0x13), RGB(0x06, 0x08, 0x12), RGB(0x07, 0x08, 0x11), RGB(0x07, 0x08, 0x10),
                RGB(0x08, 0x08, 0x0F), RGB(0x08, 0x08, 0x0E), RGB(0x09, 0x08, 0x0D), RGB(0x09, 0x08, 0x0C),
                RGB(0x0A, 0x08, 0x0B), RGB(0x0A, 0x08, 0x0A), RGB(0x0B, 0x08, 0x09), RGB(0x0B, 0x08, 0x08),
                RGB(0x0C, 0x08, 0x07), RGB(0x0C, 0x08, 0x06), RGB(0x0D, 0x08, 0x05), RGB(0x0D, 0x08, 0x04),
                RGB(0x0E, 0x08, 0x03), RGB(0x0E, 0x08, 0x02), RGB(0x0F, 0x08, 0x01), RGB(0x0F, 0x08, 0x00),
                RGB(0x00, 0x08, 0x00), RGB(0x00, 0x08, 0x01), RGB(0x01, 0x08, 0x02), RGB(0x01, 0x08, 0x03),
                RGB(0x02, 0x08, 0x04), RGB(0x02, 0x08, 0x05), RGB(0x03, 0x08, 0x06), RGB(0x03, 0x08, 0x07),
                RGB(0x04, 0x08, 0x08), RGB(0x04, 0x08, 0x09), RGB(0x05, 0x08, 0x0A), RGB(0x05, 0x08, 0x0B),
                RGB(0x06, 0x08, 0x0C), RGB(0x06, 0x08, 0x0D), RGB(0x07, 0x08, 0x0E), RGB(0x07, 0x08, 0x0F),
                RGB(0x08, 0x08, 0x10), RGB(0x08, 0x08, 0x11), RGB(0x09, 0x08, 0x12), RGB(0x09, 0x08, 0x13),
                RGB(0x0A, 0x08, 0x14), RGB(0x0A, 0x08, 0x15), RGB(0x0B, 0x08, 0x16), RGB(0x0B, 0x08, 0x17),
                RGB(0x0C, 0x08, 0x18), RGB(0x0C, 0x08, 0x19), RGB(0x0D, 0x08, 0x1A), RGB(0x0D, 0x08, 0x1B),
                RGB(0x0E, 0x08, 0x1C), RGB(0x0E, 0x08, 0x1D), RGB(0x0F, 0x08, 0x1E), RGB(0x0F, 0x08, 0x1F),
                RGB(0x00, 0x08, 0x1F), RGB(0x00, 0x08, 0x1E), RGB(0x01, 0x08, 0x1D), RGB(0x01, 0x08, 0x1C),
                RGB(0x02, 0x08, 0x1B), RGB(0x02, 0x08, 0x1A), RGB(0x03, 0x08, 0x19), RGB(0x03, 0x08, 0x18),
                RGB(0x04, 0x08, 0x17), RGB(0x04, 0x08, 0x16), RGB(0x05, 0x08, 0x15), RGB(0x05, 0x08, 0x14),
                RGB(0x06, 0x08, 0x13), RGB(0x06, 0x08, 0x12), RGB(0x07, 0x08, 0x11), RGB(0x07, 0x08, 0x10),
                RGB(0x08, 0x08, 0x0F), RGB(0x08, 0x08, 0x0E), RGB(0x09, 0x08, 0x0D), RGB(0x09, 0x08, 0x0C),
                RGB(0x0A, 0x08, 0x0B), RGB(0x0A, 0x08, 0x0A), RGB(0x0B, 0x08, 0x09), RGB(0x0B, 0x08, 0x08),
                RGB(0x0C, 0x08, 0x07), RGB(0x0C, 0x08, 0x06), RGB(0x0D, 0x08, 0x05), RGB(0x0D, 0x08, 0x04),
                RGB(0x0E, 0x08, 0x03), RGB(0x0E, 0x08, 0x02), RGB(0x0F, 0x08, 0x01), RGB(0x0F, 0x08, 0x00),
                RGB(0x00, 0x08, 0x00), RGB(0x00, 0x08, 0x01), RGB(0x01, 0x08, 0x02), RGB(0x01, 0x08, 0x03),
                RGB(0x02, 0x08, 0x04), RGB(0x02, 0x08, 0x05), RGB(0x03, 0x08, 0x06), RGB(0x03, 0x08, 0x07),
                RGB(0x04, 0x08, 0x08), RGB(0x04, 0x08, 0x09), RGB(0x05, 0x08, 0x0A), RGB(0x05, 0x08, 0x0B),
                RGB(0x06, 0x08, 0x0C), RGB(0x06, 0x08, 0x0D), RGB(0x07, 0x08, 0x0E), RGB(0x07, 0x08, 0x0F),
                RGB(0x08, 0x08, 0x10), RGB(0x08, 0x08, 0x11), RGB(0x09, 0x08, 0x12), RGB(0x09, 0x08, 0x13),
                RGB(0x0A, 0x08, 0x14), RGB(0x0A, 0x08, 0x15), RGB(0x0B, 0x08, 0x16), RGB(0x0B, 0x08, 0x17),
                RGB(0x0C, 0x08, 0x18), RGB(0x0C, 0x08, 0x19), RGB(0x0D, 0x08, 0x1A), RGB(0x0D, 0x08, 0x1B),
                RGB(0x0E, 0x08, 0x1C), RGB(0x0E, 0x08, 0x1D), RGB(0x0F, 0x08, 0x1E), RGB(0x0F, 0x08, 0x1F)
        };

        uint32_t mask;

        vdp2_init();
        /* smpc_init(); */

        vdp2_tvmd_blcs_set(/* lcclmd = */ true, VRAM_ADDR_4MBIT(2, 0x1BE40),
            blcs_color, sizeof(blcs_color));

        monitor_init();
        line_scroll_screen();
        bitmap_screen();

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                line_scroll();
                bitmap_update_screen();

                (void)sprintf(text,
                    "[1;41mPORT: %d        PORT: %d\n"
                    "TYPE: 0x%02X     TYPE: 0x%02X\n"
                    "SIZE: %dB       SIZE: %dB\n"
                    "DATA[ 0]: 0x%02X DATA[ 0]: 0x%02X\n"
                    "DATA[ 1]: 0x%02X DATA[ 1]: 0x%02X\n"
                    "DATA[ 2]: 0x%02X DATA[ 2]: 0x%02X\n"
                    "DATA[ 3]: 0x%02X DATA[ 3]: 0x%02X\n"
                    "DATA[ 4]: 0x%02X DATA[ 4]: 0x%02X\n"
                    "DATA[ 5]: 0x%02X DATA[ 5]: 0x%02X\n"
                    "DATA[ 6]: 0x%02X DATA[ 6]: 0x%02X\n"
                    "[1;42mDATA[ 7]: 0x%02X DATA[ 7]: 0x%02X[0;0m\n"
                    "[1;41mDATA[ 8]: 0x%02X DATA[ 8]: 0x%02X[0;0m\n"
                    "[1;41mDATA[ 9]: 0x%02X DATA[ 9]: 0x%02X[0;0m\n"
                    "[1;43mDATA[10]: 0x%02X DATA[10]: 0x%02X[0;0m\n"
                    "[1;46mDATA[11]: 0x%02X DATA[11]: 0x%02X[0;0m\n"
                    "[1;45mDATA[12]: 0x%02X DATA[12]: 0x%02X[0;0m\n"
                    "[1;44mDATA[13]: 0x%02X DATA[13]: 0x%02X[0;0m\n"
                    "[1;42mDATA[14]: 0x%02X DATA[14]: 0x%02X[0;0m\a",
                    smpc_peripheral_port1.info.port_no,
                    smpc_peripheral_port2.info.port_no,
                    smpc_peripheral_port1.info.type,
                    smpc_peripheral_port2.info.type,
                    smpc_peripheral_port1.info.size,
                    smpc_peripheral_port2.info.size,
                    smpc_peripheral_port1.info.data[0],
                    smpc_peripheral_port2.info.data[0],
                    smpc_peripheral_port1.info.data[1],
                    smpc_peripheral_port2.info.data[1],
                    smpc_peripheral_port1.info.data[2],
                    smpc_peripheral_port2.info.data[2],
                    smpc_peripheral_port1.info.data[3],
                    smpc_peripheral_port2.info.data[3],
                    smpc_peripheral_port1.info.data[4],
                    smpc_peripheral_port2.info.data[4],
                    smpc_peripheral_port1.info.data[5],
                    smpc_peripheral_port2.info.data[5],
                    smpc_peripheral_port1.info.data[6],
                    smpc_peripheral_port2.info.data[6],
                    smpc_peripheral_port1.info.data[7],
                    smpc_peripheral_port2.info.data[7],
                    smpc_peripheral_port1.info.data[8],
                    smpc_peripheral_port2.info.data[8],
                    smpc_peripheral_port1.info.data[9],
                    smpc_peripheral_port2.info.data[9],
                    smpc_peripheral_port1.info.data[10],
                    smpc_peripheral_port2.info.data[10],
                    smpc_peripheral_port1.info.data[11],
                    smpc_peripheral_port2.info.data[11],
                    smpc_peripheral_port1.info.data[12],
                    smpc_peripheral_port2.info.data[12],
                    smpc_peripheral_port1.info.data[13],
                    smpc_peripheral_port2.info.data[13],
                    smpc_peripheral_port1.info.data[14],
                    smpc_peripheral_port2.info.data[14]);
                vt100_write(monitor, text);
        }

        return 0;
}
