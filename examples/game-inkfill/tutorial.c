/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2.h>
#include <vdp2/pn.h>

#include <tga.h>

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "fs.h"
#include "tutorial.h"
#include "globals.h"
#include "scene.h"

static bool _changed = false;

void
tutorial_init(void)
{
        static uint16_t blcs_color[] = {
                0x8000
        };

        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        struct scrn_bm_format cfg;
        struct vram_ctl *vram_ctl;

        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear();

        cfg.bm_scrn = SCRN_NBG0;
        cfg.bm_ccc = 32768;
        cfg.bm_bs = SCRN_BM_BMSZ_512_256;
        cfg.bm_pb = VRAM_ADDR_4MBIT(0, 0x00000);
        cfg.bm_sp = 0;
        cfg.bm_spn = 0;
        cfg.bm_scc = 0;

        vdp2_scrn_bm_format_set(&cfg);
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

        fh = fs_open("/TUTOR.TGA");
        fs_load(fh, (uint8_t *)0x00200000);

        tga_t tga;
        int ret;

        ret = tga_read(&tga, (uint8_t *)0x00200000, (uint16_t *)VRAM_ADDR_4MBIT(0, 0x00000), NULL);
        assert(ret == TGA_FILE_OK);

        vdp2_scrn_display_set(SCRN_NBG0, /* no_trans = */ false);
        vdp2_tvmd_display_set();

        _changed = false;
}

void
tutorial_update(void)
{
        if (_changed) {
                return;
        }

        if (g_digital.connected == 1) {
                if (g_digital.held.button.start) {
                        _changed = true;
                        scene_load("game");
                }
        }
}

void
tutorial_draw(void)
{
}

void
tutorial_exit(void)
{
        vdp2_scrn_display_clear(SCRN_NBG0, /* no_trans = */ false);
}
