/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#define FIXMATH_NO_OVERFLOW 1

#include <fixmath.h>
#include <tga.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../test.h"
#include "../common.h"
#include "../fs.h"

#define FCM     (1 << 1)
#define FCT     (1 << 0)
#define VBE     (1 << 3)

#define BEF     (1 << 0)
#define CEF     (1 << 1)

static struct vdp1_cmdt_polygon polygon[2];

static void hblank_in_handler(irq_mux_handle_t *);
static void vblank_in_handler(irq_mux_handle_t *);
static void vblank_out_handler(irq_mux_handle_t *);

static uint32_t scanline = 0;
static bool logic_done = false;
static bool erase_write = false;
static bool start = false;

static uint16_t end_frame_cnt = 0;
static uint16_t frame_cnt = 0;
static fix16_t frame_delta = F16(0.0f);

void
move(void)
{
        static int16_t x = 0;
        static int16_t y = 0;
        static int16_t dir_x = 1;
        static int16_t dir_y = 1;

        polygon[0].cp_color = RGB888_TO_RGB555(255,   0,   0);
        polygon[0].cp_mode.transparent_pixel = true;
        polygon[0].cp_mode.end_code = true;
        polygon[0].cp_vertex.a.x = x + 16 - 1;
        polygon[0].cp_vertex.a.y = y + -16;
        polygon[0].cp_vertex.b.x = x + 16 - 1;
        polygon[0].cp_vertex.b.y = y + 16 - 1;
        polygon[0].cp_vertex.c.x = x + -16;
        polygon[0].cp_vertex.c.y = y + 16 - 1;
        polygon[0].cp_vertex.d.x = x + -16;
        polygon[0].cp_vertex.d.y = y + -16;

        if ((x < -((SCREEN_WIDTH / 2) - 16))) {
                dir_x = 1;
        } else if ((x > ((SCREEN_WIDTH / 2) - 16))) {
                dir_x = -1;
        }

        if ((y < -((SCREEN_HEIGHT / 2) - 16))) {
                dir_y = 1;
        } else if ((y > ((SCREEN_HEIGHT / 2) - 16))) {
                dir_y = -1;
        }

        x = x + dir_x;
        y = y + dir_y;

        polygon[1].cp_color = 0x0000;
        polygon[1].cp_mode.transparent_pixel = true;
        polygon[1].cp_mode.end_code = false;
        polygon[1].cp_vertex.a.x = 0;
        polygon[1].cp_vertex.a.y = SCREEN_HEIGHT - 33 - 1;
        polygon[1].cp_vertex.b.x = SCREEN_WIDTH - 1;
        polygon[1].cp_vertex.b.y = SCREEN_HEIGHT - 33 - 1;
        polygon[1].cp_vertex.c.x = SCREEN_WIDTH - 1;
        polygon[1].cp_vertex.c.y = SCREEN_HEIGHT - 1;
        polygon[1].cp_vertex.d.x = 0;
        polygon[1].cp_vertex.d.y = SCREEN_HEIGHT - 1;

        vdp1_cmdt_list_begin(0); {
                struct vdp1_cmdt_local_coord local_coord;
                local_coord.lc_coord.x = SCREEN_WIDTH / 2;
                local_coord.lc_coord.y = SCREEN_HEIGHT / 2;

                struct vdp1_cmdt_system_clip_coord system_clip;
                system_clip.scc_coord.x = SCREEN_WIDTH - 1;
                system_clip.scc_coord.y = SCREEN_HEIGHT - 1;

                struct vdp1_cmdt_user_clip_coord user_clip;
                user_clip.ucc_coords[0].x = 0;
                user_clip.ucc_coords[0].y = 0;
                user_clip.ucc_coords[1].x = SCREEN_WIDTH - 1;
                user_clip.ucc_coords[1].y = SCREEN_HEIGHT - 1;

                vdp1_cmdt_system_clip_coord_set(&system_clip);
                vdp1_cmdt_user_clip_coord_set(&user_clip);

                local_coord.lc_coord.x = 0;
                local_coord.lc_coord.y = 0;
                vdp1_cmdt_local_coord_set(&local_coord);
                vdp1_cmdt_polygon_draw(&polygon[1]);

                local_coord.lc_coord.x = SCREEN_WIDTH / 2;
                local_coord.lc_coord.y = SCREEN_HEIGHT / 2;
                vdp1_cmdt_local_coord_set(&local_coord);
                vdp1_cmdt_polygon_draw(&polygon[0]);

                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);

        cons_buffer(&cons, "[H[2J");
        (void)sprintf(text, "\n"
            "%s\n"
            "scanline = %i\n",
            (((MEMORY_READ(16, VDP2(TVSTAT)) & 0x0001) == 0x0000)
                ? "NTSC"
                : "PAL"),
            (int)scanline);
        cons_buffer(&cons, text);

        (void)sprintf(text, "\nx = %i\n"
            "y = %i\n",
            (int)x, (int)y);
        cons_buffer(&cons, text);

        (void)sprintf(text, "frame_cnt = %i\n"
            "end_frame_cnt = %i\n"
            "frame_cnt - end_frame_cnt = %i\n"
            "frame_delta = ",
            (int)frame_cnt,
            (int)end_frame_cnt,
            (int)(frame_cnt - end_frame_cnt));
        cons_buffer(&cons, text);

        frame_delta = fix16_mul(
                fix16_div(fix16_from_int(end_frame_cnt),
                    fix16_from_int(frame_cnt)), F16(60.0f));

        fix16_to_str(frame_delta, text, 2);
        cons_buffer(&cons, text);
        cons_buffer(&cons, " FPS\n");
}

void
test_07_init(void)
{
        test_init();

        irq_mux_t *hblank_in;
        hblank_in = vdp2_tvmd_hblank_in_irq_get();
        irq_mux_handle_add(hblank_in, hblank_in_handler, NULL);

        irq_mux_t *vblank_in;
        vblank_in = vdp2_tvmd_vblank_in_irq_get();
        irq_mux_handle_add(vblank_in, vblank_in_handler, NULL);

        irq_mux_t *vblank_out;
        vblank_out = vdp2_tvmd_vblank_out_irq_get();
        irq_mux_handle_add(vblank_out, vblank_out_handler, NULL);

        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(FBCR), 0x0000);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0000);

        vdp2_tvmd_vblank_out_wait();
        vdp2_tvmd_vblank_in_wait();

        MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        MEMORY_WRITE(16, VDP1(FBCR), FCM | FCT);
        MEMORY_WRITE(16, VDP1(PTMR), 0x0002);
}

void
test_07_update(void)
{
        start = true;
        logic_done = false;
        move();
        vdp2_tvmd_vblank_in_wait();
        vdp2_tvmd_vblank_out_wait();

        logic_done = true;
}

void
test_07_draw(void)
{
        cons_flush(&cons);
        while (!erase_write);
        vdp1_cmdt_list_commit();
}

void
test_07_exit(void)
{
}

static void
hblank_in_handler(irq_mux_handle_t *irq_mux __unused)
{
}

static void
vblank_in_handler(irq_mux_handle_t *irq_mux __unused)
{
        /* V-BLANK erase/write is started after the completion of the
         * VBLANK-IN interrupt. */
        if (start) {
                scanline = vdp2_tvmd_vcount_get();
                if (!logic_done) {
                        return;
                }

                end_frame_cnt = (end_frame_cnt & 0x7FFF) + 1;

                MEMORY_WRITE(16, VDP1(TVMR), VBE);
                MEMORY_WRITE(16, VDP1(FBCR), FCM | FCT);

                erase_write = true;
        }
}

static void
vblank_out_handler(irq_mux_handle_t *irq_mux __unused)
{
        if (start) {
                frame_cnt = (frame_cnt & 0x7FFF) + 1;

                MEMORY_WRITE(16, VDP1(TVMR), 0x0000);
        }
}
