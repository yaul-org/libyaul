/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <libyaul.h>

#include <tga.h>

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "fs.h"
#include "globals.h"
#include "scene.h"
#include "title.h"

static bool changed = false;

void
title_init(void)
{
        static uint16_t back_screen_color[] = {
                0x8000
        };

        vdp2_scrn_back_screen_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            back_screen_color, 0);

        struct vram_ctl *vram_ctl;

        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear();

        vdp2_scrn_display_clear();

        struct scrn_bitmap_format nbg0_format;

        nbg0_format.sbf_scroll_screen = SCRN_NBG0;
        nbg0_format.sbf_cc_count = SCRN_CCC_RGB_32768;
        nbg0_format.sbf_bitmap_size.width = 512;
        nbg0_format.sbf_bitmap_size.height = 256;
        nbg0_format.sbf_bitmap_pattern = VRAM_ADDR_4MBIT(0, 0x00000);

        vdp2_scrn_bitmap_format_set(&nbg0_format);
        vdp2_priority_spn_set(SCRN_NBG0, 7);

        vram_ctl = vdp2_vram_control_get();
        vram_ctl->vram_cycp.pt[0].t7 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[0].t6 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[0].t5 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[0].t4 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[0].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        vram_ctl->vram_cycp.pt[1].t7 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t6 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t5 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t4 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t0 = VRAM_CTL_CYCP_NO_ACCESS;
        vdp2_vram_control_set(vram_ctl);

        void *fh;

        fh = fs_open("/TITLE.TGA");
        fs_load(fh, (uint8_t *)0x00201000);

        tga_t tga;
        int ret;

        ret = tga_read(&tga, (uint8_t *)0x00201000,
            (uint16_t *)VRAM_ADDR_4MBIT(0, 0x00000), NULL);
        assert(ret == TGA_FILE_OK);

        vdp2_scrn_display_set(SCRN_NBG0, /* transparent = */ false);
        vdp2_tvmd_display_set();

        changed = false;
}

void
title_update(void)
{
        if (changed) {
                return;
        }

        if (g_digital.connected == 1) {
                if (g_digital.held.button.start) {
                        changed = true;
                        scene_load("tutorial");
                }
        }
}

void
title_draw(void)
{
}

void
title_exit(void)
{
        vdp2_scrn_display_clear();
}
