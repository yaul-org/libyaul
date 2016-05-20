/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#include "fs.h"

#include <assert.h>
#include <stdbool.h>

static struct smpc_peripheral_digital digital_pad;
static uint32_t tick = 0;

static void vblank_in_handler(irq_mux_handle_t *);
static void vblank_out_handler(irq_mux_handle_t *);

static void hardware_init(void);

static void config_0(void);

int
main(void)
{
        hardware_init();

        config_0();

        fix16_t scale;
        scale = F16(0.0f);

        while (true) {
                vdp2_tvmd_vblank_out_wait(); {
                        scale = fix16_add(scale, F16(0.25f));
                }

                vdp2_tvmd_vblank_in_wait(); {
                        vdp2_scrn_reduction_x_set(SCRN_NBG1, scale);
                        vdp2_scrn_reduction_y_set(SCRN_NBG1, scale);
                }
        }
}

static void
hardware_init(void)
{
        /* VDP1 */
        vdp1_init();

        /* VDP2 */
        vdp2_init();
        vdp2_tvmd_display_res_set(TVMD_INTERLACE_NONE, TVMD_HORZ_NORMAL_A,
            TVMD_VERT_224);
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(2, 0x01FFFE),
            COLOR_RGB555(0, 0, 7));

        /* SMPC */
        smpc_init();
        smpc_peripheral_init();

        /* Disable interrupts */
        cpu_intc_disable();

        irq_mux_t *vblank_in;
        irq_mux_t *vblank_out;

        vblank_in = vdp2_tvmd_vblank_in_irq_get();
        irq_mux_handle_add(vblank_in, vblank_in_handler, NULL);

        vblank_out = vdp2_tvmd_vblank_out_irq_get();
        irq_mux_handle_add(vblank_out, vblank_out_handler, NULL);

        /* Enable interrupts */
        cpu_intc_enable();
}

static void
vblank_in_handler(irq_mux_handle_t *irq_mux __unused)
{
        smpc_peripheral_digital_port(1, &digital_pad);
}

static void
vblank_out_handler(irq_mux_handle_t *irq_mux __unused)
{
        if ((vdp2_tvmd_vcount_get()) == 0) {
                tick = (tick & 0xFFFFFFFF) + 1;
        }
}

static void
config_0(void)
{
        struct scrn_cell_format format;

        uint16_t *color_palette;
        color_palette = (uint16_t *)CRAM_MODE_1_OFFSET(0, 2, 0);

        uint16_t *planes[4];
        planes[0] = (uint16_t *)VRAM_ADDR_4MBIT(0, 0x08000);
        planes[1] = (uint16_t *)VRAM_ADDR_4MBIT(0, 0x10000);
        planes[2] = (uint16_t *)VRAM_ADDR_4MBIT(0, 0x18000);
        planes[3] = (uint16_t *)VRAM_ADDR_4MBIT(0, 0x20000);
        uint16_t *cpd;
        cpd = (uint16_t *)VRAM_ADDR_4MBIT(2, 0x00000);

        format.scf_scroll_screen = SCRN_NBG1;
        format.scf_cc_count = SCRN_CCC_PALETTE_16;
        format.scf_character_size = 1 * 1;
        format.scf_pnd_size = 1; /* 1-word */
        format.scf_auxiliary_mode = 1;
        format.scf_plane_size = 2 * 2;
        format.scf_cp_table = (uint32_t)cpd;
        format.scf_color_palette = (uint32_t)color_palette;
        format.scf_map.plane_a = (uint32_t)planes[0];
        format.scf_map.plane_b = (uint32_t)planes[1];
        format.scf_map.plane_c = (uint32_t)planes[2];
        format.scf_map.plane_d = (uint32_t)planes[3];

        struct vram_ctl *vram_ctl;
        vram_ctl = vdp2_vram_control_get();

        vram_ctl->vram_cycp.pt[0].t7 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[0].t6 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[0].t5 = VRAM_CTL_CYCP_CHPNDR_NBG2;
        vram_ctl->vram_cycp.pt[0].t4 = VRAM_CTL_CYCP_CHPNDR_NBG2;
        vram_ctl->vram_cycp.pt[0].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        vram_ctl->vram_cycp.pt[1].t7 = VRAM_CTL_CYCP_PNDR_NBG1;
        vram_ctl->vram_cycp.pt[1].t6 = VRAM_CTL_CYCP_PNDR_NBG1;
        vram_ctl->vram_cycp.pt[1].t5 = VRAM_CTL_CYCP_PNDR_NBG2;
        vram_ctl->vram_cycp.pt[1].t4 = VRAM_CTL_CYCP_PNDR_NBG2;
        vram_ctl->vram_cycp.pt[1].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear(); {
                uint32_t i;
                uint32_t j;
                for (j = 0; j < 4096; j++) {
                        for (i = 0; i < 16; i++) {
                                uint32_t x;
                                x = j & 0x0F;
                                cpd[i + (j * (32 / 2))] = (x << 12) | (x << 8) | (x << 4) | x;
                        }
                }

                color_palette[ 0] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(255, 255, 255);
                color_palette[ 1] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(255,   0,   0);
                color_palette[ 2] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(  0, 255,   0);
                color_palette[ 3] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(  0,   0, 255);
                color_palette[ 4] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(255, 255,   0);
                color_palette[ 5] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(255,   0, 255);
                color_palette[ 6] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(  0,   0,   0);
                color_palette[ 7] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(  0, 255, 255);
                color_palette[ 8] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(127, 127,   0);
                color_palette[ 9] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(  0, 127, 127);
                color_palette[10] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(  0, 127,   0);
                color_palette[11] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(  0,   0, 127);
                color_palette[12] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(127,   0,   0);
                color_palette[13] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(127, 127,   0);
                color_palette[14] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(127, 127, 127);
                color_palette[15] = COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(127,   0, 127);

                uint32_t page_width;
                page_width = SCRN_CALCULATE_PAGE_WIDTH(&format);
                uint32_t page_height;
                page_height = SCRN_CALCULATE_PAGE_HEIGHT(&format);
                uint32_t page_size;
                page_size = SCRN_CALCULATE_PAGE_SIZE(&format);

                uint16_t *a_pages[4];
                a_pages[0] = &planes[0][0];
                a_pages[1] = &planes[0][1 * (page_size / 2)];
                a_pages[2] = &planes[0][2 * (page_size / 2)];
                a_pages[3] = &planes[0][3 * (page_size / 2)];

                uint16_t *b_pages[4];
                b_pages[0] = &planes[1][0];
                b_pages[1] = &planes[1][1 * (page_size / 2)];
                b_pages[2] = &planes[1][2 * (page_size / 2)];
                b_pages[3] = &planes[1][3 * (page_size / 2)];

                uint16_t *c_pages[4];
                c_pages[0] = &planes[2][0];
                c_pages[1] = &planes[2][1 * (page_size / 2)];
                c_pages[2] = &planes[2][2 * (page_size / 2)];
                c_pages[3] = &planes[2][3 * (page_size / 2)];

                uint16_t *d_pages[4];
                d_pages[0] = &planes[3][0];
                d_pages[1] = &planes[3][1 * (page_size / 2)];
                d_pages[2] = &planes[3][2 * (page_size / 2)];
                d_pages[3] = &planes[3][3 * (page_size / 2)];

                uint32_t page_x;
                uint32_t page_y;
                for (page_y = 0; page_y < page_height; page_y++) {
                        for (page_x = 0; page_x < page_width; page_x++) {
                                uint16_t page_idx;
                                page_idx = page_x + (page_width * page_y);

                                uint16_t pnd;
                                pnd = SCRN_PND_CONFIG_1((uint32_t)cpd, (uint32_t)color_palette);

                                a_pages[0][page_idx] = pnd | 0;
                                a_pages[1][page_idx] = pnd | 1;
                                a_pages[2][page_idx] = pnd | 2;
                                a_pages[3][page_idx] = pnd | 3;

                                b_pages[0][page_idx] = pnd | 4;
                                b_pages[1][page_idx] = pnd | 5;
                                b_pages[2][page_idx] = pnd | 6;
                                b_pages[3][page_idx] = pnd | 7;

                                c_pages[0][page_idx] = pnd | 8;
                                c_pages[1][page_idx] = pnd | 9;
                                c_pages[2][page_idx] = pnd | 10;
                                c_pages[3][page_idx] = pnd | 11;

                                d_pages[0][page_idx] = pnd | 12;
                                d_pages[1][page_idx] = pnd | 13;
                                d_pages[2][page_idx] = pnd | 14;
                                d_pages[3][page_idx] = pnd | 15;
                        }
                }

                vdp2_vram_control_set(vram_ctl);

                vdp2_scrn_cell_format_set(&format);
                vdp2_scrn_priority_set(SCRN_NBG1, 7);
                vdp2_scrn_scroll_x_set(SCRN_NBG1, F16(0.0f));
                vdp2_scrn_scroll_y_set(SCRN_NBG1, F16(0.0f));
                vdp2_scrn_reduction_set(SCRN_NBG1, SCRN_REDUCTION_QUARTER);

                vdp2_scrn_display_set(SCRN_NBG1, /* transparent = */ true);
        } vdp2_tvmd_display_set();
}
