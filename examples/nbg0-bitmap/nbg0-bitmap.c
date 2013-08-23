/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2.h>
#include <vdp2/pn.h>
#include <smpc.h>
#include <smpc/smc.h>
#include <smpc/peripheral.h>

#include <fs/romdisk/romdisk.h>

#include <tga.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

extern uint8_t root_romdisk[];

int
main(void)
{
        struct scrn_bm_format cfg;
        struct vram_ctl *vram_ctl;

        uint32_t pos;

        uint16_t blcs_color[] = {
                0x8000
        };

        pos = 0;

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        cfg.bm_scrn = SCRN_NBG0;
        cfg.bm_bs = SCRN_BM_BMSZ_512_256;
        cfg.bm_pb = VRAM_ADDR_4MBIT(0, 0x00000);
        cfg.bm_sp = 0;
        cfg.bm_spn = 0;
        cfg.bm_scc = 0;

        vdp2_scrn_ccc_set(SCRN_NBG0, SCRN_CCC_CHC_32768);
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

        void *romdisk;
        void *fh; /* File handle */

        romdisk_init();

        romdisk = romdisk_mount("/", root_romdisk);
        assert(romdisk != NULL);

        fh = romdisk_open(romdisk, "/BITMAP.TGA", O_RDONLY);
        assert(fh != NULL);

        uint8_t *tga_file;
        tga_t tga;
        int ret;
        size_t len;

        tga_file = (uint8_t *)0x00200000;
        assert(tga_file != NULL);

        len = romdisk_read(fh, tga_file, romdisk_total(fh));
        assert(len == romdisk_total(fh));

        ret = tga_load(&tga, tga_file, (uint16_t *)VRAM_ADDR_4MBIT(0, 0x00000), NULL);
        assert(ret == TGA_FILE_OK);

        vdp2_scrn_display_set(SCRN_NBG0, /* no_trans = */ false);

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();

                vdp2_scrn_scv_x_set(SCRN_NBG0, pos, 0);
                vdp2_scrn_scv_y_set(SCRN_NBG0, 512 - pos, 0);

                pos++;
        }

        return 0;
}
