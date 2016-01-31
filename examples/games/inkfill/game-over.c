/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "fs.h"
#include "game-over.h"
#include "globals.h"
#include "scene.h"

static bool changed = false;

void
game_over_init(void)
{
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(3, 0x01FFFE),
            COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0, 0, 0));

        struct scrn_bitmap_format nbg0_format;
        struct vram_ctl *vram_ctl;

        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear();

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

        fh = fs_open("/DEAD.TGA");
        fs_load(fh, (uint8_t *)0x00201000);

        tga_t tga;
        tga.tga_options.msb = false;
        tga.tga_options.transparent_pixel = COLOR_RGB888_TO_RGB555(0, 255, 0);

        int ret;

        uint8_t *ptr;
        ptr = (uint8_t *)0x00201000;

        ret = tga_read(&tga, ptr);
        assert(ret == TGA_FILE_OK);
        uint32_t amount;
        amount = tga_image_decode(&tga, (void *)VRAM_ADDR_4MBIT(0, 0x00000));
        assert(amount > 0);

        vdp2_scrn_display_set(SCRN_NBG0, /* transparent = */ false);
        vdp2_tvmd_display_set();

        changed = false;
}

void
game_over_update(void)
{
        if (changed) {
                return;
        }

        sleep(3);

        changed = true;
        scene_load("title");
}

void
game_over_draw(void)
{
}

void
game_over_exit(void)
{
        vdp2_scrn_display_clear();
}
