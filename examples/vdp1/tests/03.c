/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <fixmath.h>
#include <tga.h>

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../common.h"
#include "../fs.h"
#include "../state_machine.h"
#include "../test.h"

#define STATE_SHOW_LOOP                 0
#define STATE_SHOW_SCALED_SPRITE        1
#define STATE_SHOW_ZOOMED_SPRITE        2

#define STATE_ZOOM_MOVE_ORIGIN          0
#define STATE_ZOOM_WAIT                 1
#define STATE_ZOOM_MOVE_ANCHOR          2
#define STATE_ZOOM_RELEASE_BUTTONS      3
#define STATE_ZOOM_SELECT_ANCHOR        4

#define ZOOM_POINT_WIDTH                160
#define ZOOM_POINT_HEIGHT               96
#define ZOOM_POINT_POINTER_SIZE         3
#define ZOOM_POINT_COLOR_SELECT         RGB888_TO_RGB555(  0,   0, 255)
#define ZOOM_POINT_COLOR_WAIT           RGB888_TO_RGB555(  0, 255,   0)
#define ZOOM_POINT_COLOR_HIGHLIGHT      RGB888_TO_RGB555(255,   0,   0)

static struct vdp1_cmdt_polygon polygon[1];
static struct vdp1_cmdt_sprite sprite[2];

static uint32_t angle = 0;

/* File handles */
static void *file_handle[2];

static uint32_t cram[2] = {
        CRAM_OFFSET(0, 1, 0),
        CRAM_OFFSET(1, 0, 0)
};

static uint32_t vram[2];

static int16_vector2_t pointer;
static int16_vector2_t display;
static uint16_t zoom_point_value = 0x0000;
static uint16_t zoom_point_color = RGB888_TO_RGB555(0, 0, 0);
static int16_vector2_t zoom_point;

static struct zoom_point_boundary {
        int16_t w_dir; /* Display width direction */
        int16_t h_dir; /* Display height direction */
        int16_t x_max;
        int16_t y_max;
        int16_t x_min;
        int16_t y_min;
} zoom_point_boundaries[] = {
        { /* Upper left */
                .w_dir = 1,
                .h_dir = -1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH - (ZOOM_POINT_WIDTH / 2),
                .y_max = ZOOM_POINT_HEIGHT + ((SCREEN_HEIGHT - ZOOM_POINT_HEIGHT) / 2)
        }, { /* Upper center */
                .w_dir = 1,
                .h_dir = -1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH,
                .y_max = ZOOM_POINT_HEIGHT + ((SCREEN_HEIGHT - ZOOM_POINT_HEIGHT) / 2)
        }, { /* Upper right */
                .w_dir = -1,
                .h_dir = -1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH - (ZOOM_POINT_WIDTH / 2),
                .y_max = ZOOM_POINT_HEIGHT + ((SCREEN_HEIGHT - ZOOM_POINT_HEIGHT) / 2)
        }, { /* Reserved */
                0
        }, { /* Center left */
                .w_dir = 1,
                .h_dir = 1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH - (ZOOM_POINT_WIDTH / 2),
                .y_max = SCREEN_HEIGHT
        }, { /* Center */
                .w_dir = 1,
                .h_dir = -1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH,
                .y_max = SCREEN_HEIGHT
        }, { /* Center right */
                .w_dir = -1,
                .h_dir = -1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH - (ZOOM_POINT_WIDTH / 2),
                .y_max = SCREEN_HEIGHT
        }, { /* Reserved */
                0
        }, { /* Lower left */
                .w_dir = 1,
                .h_dir = 1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH - (ZOOM_POINT_WIDTH / 2),
                .y_max = ZOOM_POINT_HEIGHT + ((SCREEN_HEIGHT - ZOOM_POINT_HEIGHT) / 2)
        }, { /* Lower center */
                .w_dir = 1,
                .h_dir = 1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH,
                .y_max = ZOOM_POINT_HEIGHT + ((SCREEN_HEIGHT - ZOOM_POINT_HEIGHT) / 2)
        }, { /* Lower right */
                .w_dir = -1,
                .h_dir = 1,
                .x_min = 0,
                .y_min = 0,
                .x_max = SCREEN_WIDTH - (ZOOM_POINT_WIDTH / 2),
                .y_max = ZOOM_POINT_HEIGHT + ((SCREEN_HEIGHT - ZOOM_POINT_HEIGHT) / 2)
        }
};

static uint32_t state_zoom;

static struct state_machine state_machine;

static void state_00_init(struct state_context *);
static void state_00_update(struct state_context *);
static void state_00_draw(struct state_context *);

static void state_01_init(struct state_context *);
static void state_01_update(struct state_context *);
static void state_01_draw(struct state_context *);

void
test_03_init(void)
{
        test_init();

        memset(sprite, 0x00,
            2 * sizeof(struct vdp1_cmdt_sprite));
        memset(polygon, 0x00, sizeof(struct vdp1_cmdt_polygon));

        uint32_t vram_addr;
        vram_addr = CHAR(0);

        size_t file_size;

        file_handle[0] = fs_open("/TESTS/03/SCALE.TGA");
        file_handle[1] = fs_open("/TESTS/03/ZOOM.TGA");

        uint32_t type_idx;
        for (type_idx = 0; type_idx < 2; type_idx++) {
                void *fh;
                fh = file_handle[type_idx];
                file_size = common_round_pow2(fs_size(fh));

                uint8_t *ptr;
                ptr = (uint8_t *)0x00200000;

                fs_read(fh, ptr);
                fs_close(fh);

                tga_t tga;
                int status;
                status = tga_read(&tga, ptr);
                assert(status == TGA_FILE_OK);
                uint32_t amount;
                amount = tga_image_decode(&tga, (void *)vram_addr);
                assert(amount > 0);
                amount = tga_cmap_decode(&tga, (uint16_t *)cram[type_idx]);
                if ((tga.tga_type == TGA_IMAGE_TYPE_CMAP) ||
                    (tga.tga_type == TGA_IMAGE_TYPE_RLE_CMAP)) {
                        assert(amount > 0);
                }
                vram[type_idx] = vram_addr;

                vram_addr += file_size;
        }

        state_machine_init(&state_machine);

        state_machine_add_state(&state_machine, "show-scaled-sprite",
            STATE_SHOW_SCALED_SPRITE,
            state_00_init,
            state_00_update,
            state_00_draw,
            NULL,
            NULL);
        state_machine_add_state(&state_machine, "show-zoomed-sprite",
            STATE_SHOW_ZOOMED_SPRITE,
            state_01_init,
            state_01_update,
            state_01_draw,
            NULL,
            NULL);

        state_machine_transition(&state_machine, STATE_SHOW_SCALED_SPRITE);
}

void
test_03_update(void)
{
        state_machine_handler_update(&state_machine);
}

void
test_03_draw(void)
{
        state_machine_handler_draw(&state_machine);
}

void
test_03_exit(void)
{
}

static void
state_00_init(struct state_context *state_context __unused)
{
        text[0] = '\0';
        cons_buffer(&cons, "[H[2J");

        angle = 0;
}

static void
state_00_update(struct state_context *state_context __unused)
{
        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        test_exit();
                } else if (digital_pad.held.button.l) {
                        state_machine_transition(&state_machine,
                            STATE_SHOW_ZOOMED_SPRITE);
                        return;
                }
        }

        float x;
        x = (int16_t)((-128 * MATH_COS(4 * angle)) >> 12);

        float y;
        y = (int16_t)((-128 * MATH_COS(2 * angle)) >> 12);

        angle++;

        sprite[0].cs_type = CMDT_TYPE_SCALED_SPRITE;
        sprite[0].cs_mode.cc_mode = 0;
        sprite[0].cs_mode.color_mode = 0;
        sprite[0].cs_color_bank = 1;
        sprite[0].cs_mode.transparent_pixel = false;
        sprite[0].cs_mode.high_speed_shrink = false;
        sprite[0].cs_char = vram[0];
        sprite[0].cs_vertex.a.x = x;
        sprite[0].cs_vertex.a.y = y;
        sprite[0].cs_vertex.c.x = -x;
        sprite[0].cs_vertex.c.y = -y;
        sprite[0].cs_width = 64;
        sprite[0].cs_height = 64;

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_local_coord_set(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                vdp1_cmdt_sprite_draw(&sprite[0]);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

static void
state_00_draw(struct state_context *state_context __unused)
{
        cons_flush(&cons);

        vdp1_cmdt_list_commit();
}

static void
state_01_init(struct state_context *state_context __unused)
{
        text[0] = '\0';
        cons_buffer(&cons, "[H[2J");

        state_zoom = STATE_ZOOM_MOVE_ORIGIN;
}

static void
state_01_update(struct state_context *state_context __unused)
{
        static uint16_t captured_buttons = 0xFFFF;
        static uint32_t delay_frames = 0;

        cons_buffer(&cons, "[H[2J");

        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        return;
                } else if (digital_pad.held.button.l) {
                        state_machine_transition(&state_machine,
                            STATE_SHOW_SCALED_SPRITE);
                        return;
                }

                bool dirs_pressed;
                dirs_pressed =
                    (digital_pad.pressed.raw & PERIPHERAL_DIGITAL_DIRECTIONS) != 0;

                bool x_center;
                bool x_left;
                bool x_right;
                bool y_center;
                bool y_up;
                bool y_down;

                uint32_t zp_idx;
                struct zoom_point_boundary *zp_boundary;

                int16_t dw_dir;
                int16_t dh_dir;

                switch (state_zoom) {
                case STATE_ZOOM_MOVE_ORIGIN:
                        pointer.x = 0;
                        pointer.y = 0;

                        display.x = ZOOM_POINT_WIDTH;
                        display.y = ZOOM_POINT_HEIGHT;

                        zoom_point_value = CMDT_ZOOM_POINT_CENTER;
                        zoom_point.x = 0;
                        zoom_point.y = 0;
                        zoom_point_color = ZOOM_POINT_COLOR_SELECT;

                        delay_frames = 0;

                        if (dirs_pressed) {
                                captured_buttons = digital_pad.pressed.raw;
                                state_zoom = STATE_ZOOM_WAIT;
                        } else if ((digital_pad.held.raw & PERIPHERAL_DIGITAL_A) != 0) {
                                state_zoom = STATE_ZOOM_RELEASE_BUTTONS;
                        }
                        break;
                case STATE_ZOOM_WAIT:
                        if (dirs_pressed) {
                                captured_buttons = digital_pad.pressed.raw;
                        }

                        delay_frames++;

                        if (delay_frames > 9) {
                                delay_frames = 0;
                                state_zoom = STATE_ZOOM_MOVE_ANCHOR;
                        } else if (!dirs_pressed) {
                                delay_frames = 0;
                                state_zoom = STATE_ZOOM_MOVE_ORIGIN;
                        }
                        break;
                case STATE_ZOOM_MOVE_ANCHOR:
                        if ((captured_buttons & PERIPHERAL_DIGITAL_LEFT) != 0) {
                                pointer.x = -display.x / 2;
                        }

                        if ((captured_buttons & PERIPHERAL_DIGITAL_RIGHT) != 0) {
                                pointer.x = display.x / 2;
                        }

                        if ((captured_buttons & PERIPHERAL_DIGITAL_UP) != 0) {
                                pointer.y = -display.y / 2;
                        }

                        if ((captured_buttons & PERIPHERAL_DIGITAL_DOWN) != 0) {
                                pointer.y = display.y / 2;
                        }

                        /* Determine the zoom point */
                        x_center = pointer.x == 0;
                        x_left = pointer.x < 0;
                        x_right = pointer.x > 0;

                        y_center = pointer.y == 0;
                        y_up = pointer.y < 0;
                        y_down = pointer.y > 0;

                        if (x_center && y_up) {
                                zoom_point_value = CMDT_ZOOM_POINT_UPPER_CENTER;
                                zoom_point.x = 0;
                                zoom_point.y = -ZOOM_POINT_HEIGHT / 2;
                        } else if (x_center && y_down) {
                                zoom_point_value = CMDT_ZOOM_POINT_LOWER_CENTER;
                                zoom_point.x = 0;
                                zoom_point.y = ZOOM_POINT_HEIGHT / 2;
                        } else if (y_center && x_left) {
                                zoom_point_value = CMDT_ZOOM_POINT_CENTER_LEFT;
                                zoom_point.x = -ZOOM_POINT_WIDTH / 2;
                                zoom_point.y = 0;
                        } else if (y_center && x_right) {
                                zoom_point_value = CMDT_ZOOM_POINT_CENTER_RIGHT;
                                zoom_point.x = ZOOM_POINT_WIDTH / 2;
                                zoom_point.y = 0;
                        } else if (y_up && x_left) {
                                zoom_point_value = CMDT_ZOOM_POINT_UPPER_LEFT;
                                zoom_point.x = -ZOOM_POINT_WIDTH / 2;
                                zoom_point.y = -ZOOM_POINT_HEIGHT / 2;
                        } else if (y_up && x_right) {
                                zoom_point_value = CMDT_ZOOM_POINT_UPPER_RIGHT;
                                zoom_point.x = ZOOM_POINT_WIDTH / 2;
                                zoom_point.y = -ZOOM_POINT_HEIGHT / 2;
                        } else if (y_down && x_left) {
                                zoom_point_value = CMDT_ZOOM_POINT_LOWER_LEFT;
                                zoom_point.x = -ZOOM_POINT_WIDTH / 2;
                                zoom_point.y = ZOOM_POINT_HEIGHT / 2;
                        } else if (y_down && x_right) {
                                zoom_point_value = CMDT_ZOOM_POINT_LOWER_RIGHT;
                                zoom_point.x = ZOOM_POINT_WIDTH / 2;
                                zoom_point.y = ZOOM_POINT_HEIGHT / 2;
                        } else if (x_center && y_center) {
                                zoom_point_value = CMDT_ZOOM_POINT_CENTER;
                                zoom_point.x = 0;
                                zoom_point.y = 0;
                        }

                        captured_buttons = digital_pad.pressed.raw;

                        if (!dirs_pressed) {
                                state_zoom = STATE_ZOOM_MOVE_ORIGIN;
                        } else if ((digital_pad.held.raw & PERIPHERAL_DIGITAL_A) != 0) {
                                state_zoom = STATE_ZOOM_RELEASE_BUTTONS;
                        }
                        break;
                case STATE_ZOOM_RELEASE_BUTTONS:
                        zoom_point_color = ZOOM_POINT_COLOR_WAIT;

                        if (!dirs_pressed) {
                                state_zoom = STATE_ZOOM_SELECT_ANCHOR;
                        }
                        break;
                case STATE_ZOOM_SELECT_ANCHOR:
                        zoom_point_color = ZOOM_POINT_COLOR_HIGHLIGHT;

                        zp_idx = common_log2_down(zoom_point_value) - 5;
                        zp_boundary = &zoom_point_boundaries[zp_idx];

                        dw_dir = zp_boundary->w_dir * 1;
                        dh_dir = zp_boundary->h_dir * 1;

                        if ((digital_pad.pressed.raw & PERIPHERAL_DIGITAL_UP) != 0) {
                                if (((display.y + dh_dir) >= zp_boundary->y_min) &&
                                    ((display.y + dh_dir) <= zp_boundary->y_max)) {
                                        display.y = display.y + dh_dir;
                                }
                        }

                        if ((digital_pad.pressed.raw & PERIPHERAL_DIGITAL_DOWN) != 0) {
                                if (((display.y - dh_dir) >= zp_boundary->y_min) &&
                                    ((display.y - dh_dir) <= zp_boundary->y_max)) {
                                        display.y = display.y - dh_dir;
                                }
                        }

                        if ((digital_pad.pressed.raw & PERIPHERAL_DIGITAL_LEFT) != 0) {
                                if (((display.x - dw_dir) >= zp_boundary->x_min) &&
                                    ((display.x - dw_dir) <= zp_boundary->x_max)) {
                                        display.x = display.x - dw_dir;
                                }
                        }

                        if ((digital_pad.pressed.raw & PERIPHERAL_DIGITAL_RIGHT) != 0) {
                                if (((display.x + dw_dir) >= zp_boundary->x_min) &&
                                    ((display.x + dw_dir) <= zp_boundary->x_max)) {
                                        display.x = display.x + dw_dir;
                                }
                        }

                        if ((digital_pad.held.raw & PERIPHERAL_DIGITAL_B) != 0) {
                                state_zoom = STATE_ZOOM_MOVE_ORIGIN;
                        }
                        break;
                }
        }

        cons_buffer(&cons, text);

        sprite[1].cs_type = CMDT_TYPE_SCALED_SPRITE;
        sprite[1].cs_zoom_point.enable = true;
        sprite[1].cs_zoom_point.raw = zoom_point_value;
        sprite[1].cs_mode.cc_mode = 0;
        sprite[1].cs_mode.color_mode = 4;
        sprite[1].cs_mode.transparent_pixel = true;
        sprite[1].cs_color_bank = 1;
        sprite[1].cs_char = vram[1];
        sprite[1].cs_zoom.point.x = zoom_point.x;
        sprite[1].cs_zoom.point.y = zoom_point.y;
        sprite[1].cs_zoom.display.x = display.x;
        sprite[1].cs_zoom.display.y = display.y;
        sprite[1].cs_width = ZOOM_POINT_WIDTH;
        sprite[1].cs_height = ZOOM_POINT_HEIGHT;

        polygon[0].cp_color = zoom_point_color;
        polygon[0].cp_mode.transparent_pixel = true;
        polygon[0].cp_vertex.a.x = ZOOM_POINT_POINTER_SIZE + pointer.x - 1;
        polygon[0].cp_vertex.a.y = -ZOOM_POINT_POINTER_SIZE + pointer.y;
        polygon[0].cp_vertex.b.x = ZOOM_POINT_POINTER_SIZE + pointer.x - 1;
        polygon[0].cp_vertex.b.y = ZOOM_POINT_POINTER_SIZE + pointer.y - 1;
        polygon[0].cp_vertex.c.x = -ZOOM_POINT_POINTER_SIZE + pointer.x;
        polygon[0].cp_vertex.c.y = ZOOM_POINT_POINTER_SIZE + pointer.y - 1;
        polygon[0].cp_vertex.d.x = -ZOOM_POINT_POINTER_SIZE + pointer.x;
        polygon[0].cp_vertex.d.y = -ZOOM_POINT_POINTER_SIZE + pointer.y;

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_system_clip_coord_set(SCREEN_WIDTH, SCREEN_HEIGHT);
                vdp1_cmdt_user_clip_coord_set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
                vdp1_cmdt_local_coord_set(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                vdp1_cmdt_sprite_draw(&sprite[1]);
                vdp1_cmdt_polygon_draw(&polygon[0]);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

static void
state_01_draw(struct state_context *state_context __unused)
{
        cons_flush(&cons);

        vdp1_cmdt_list_commit();
}
