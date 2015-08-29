/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "test.h"

#define STATE_ZOOM_MOVE_INVALID         -1
#define STATE_ZOOM_MOVE_ORIGIN          0
#define STATE_ZOOM_WAIT                 1
#define STATE_ZOOM_MOVE_ANCHOR          2
#define STATE_ZOOM_RELEASE_BUTTONS      3
#define STATE_ZOOM_SELECT_ANCHOR        4

#define ZOOM_POINT_WIDTH                160
#define ZOOM_POINT_HEIGHT               96
#define ZOOM_POINT_POINTER_SIZE         3
#define ZOOM_POINT_COLOR_SELECT         RGB888_TO_RGB555(  0,   0, 255)
#define ZOOM_POINT_COLOR_WAIT           RGB888_TO_RGB555(255,   0,   0)
#define ZOOM_POINT_COLOR_HIGHLIGHT      RGB888_TO_RGB555(  0, 255,   0)

static uint32_t state_zoom = STATE_ZOOM_MOVE_INVALID;
static int16_vector2_t pointer = INT16_VECTOR2_INITIALIZER(0, 0);
static int16_vector2_t display = INT16_VECTOR2_INITIALIZER(0, 0);
static int16_vector2_t zoom_point = INT16_VECTOR2_INITIALIZER(0, 0);;
static uint16_t zoom_point_value = 0x0000;
static uint16_t zoom_point_color = RGB888_TO_RGB555(0, 0, 0);
static uint16_t captured_buttons = 0xFFFF;
static uint32_t delay_frames = 0;

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

TEST_PROTOTYPE_DECLARE(scaled_sprite_01, init)
{
        void *file_handle;
        file_handle = fs_open("/TESTS/03/ZOOM.TGA");

        uint8_t *ptr;
        ptr = (uint8_t *)0x00201000;

        fs_read(file_handle, ptr);
        fs_close(file_handle);

        tga_t tga;
        int status;
        status = tga_read(&tga, ptr);
        assert(status == TGA_FILE_OK);
        uint32_t amount;
        amount = tga_image_decode(&tga, (void *)CHAR(0));
        assert(amount > 0);
        amount = tga_cmap_decode(&tga, (uint16_t *)CRAM_MODE_1_OFFSET(1, 0, 0));
        assert(amount > 0);

        state_zoom = STATE_ZOOM_MOVE_ORIGIN;
}

TEST_PROTOTYPE_DECLARE(scaled_sprite_01, update)
{

        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        test_exit();
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

                struct zoom_point_boundary *zp_boundary;
                uint32_t zp_idx;

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
                        } else if ((digital_pad.held.button.a) != 0) {
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
                        } else if ((digital_pad.held.button.a) != 0) {
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

                        if ((digital_pad.pressed.button.up) != 0) {
                                if (((display.y + dh_dir) >= zp_boundary->y_min) &&
                                    ((display.y + dh_dir) <= zp_boundary->y_max)) {
                                        display.y = display.y + dh_dir;
                                }
                        }

                        if ((digital_pad.pressed.button.down) != 0) {
                                if (((display.y - dh_dir) >= zp_boundary->y_min) &&
                                    ((display.y - dh_dir) <= zp_boundary->y_max)) {
                                        display.y = display.y - dh_dir;
                                }
                        }

                        if ((digital_pad.pressed.button.left) != 0) {
                                if (((display.x - dw_dir) >= zp_boundary->x_min) &&
                                    ((display.x - dw_dir) <= zp_boundary->x_max)) {
                                        display.x = display.x - dw_dir;
                                }
                        }

                        if ((digital_pad.pressed.button.right) != 0) {
                                if (((display.x + dw_dir) >= zp_boundary->x_min) &&
                                    ((display.x + dw_dir) <= zp_boundary->x_max)) {
                                        display.x = display.x + dw_dir;
                                }
                        }

                        if ((digital_pad.held.button.b) != 0) {
                                state_zoom = STATE_ZOOM_MOVE_ORIGIN;
                        }
                        break;
                }
        }

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

                struct vdp1_cmdt_polygon polygon_pointer;
                memset(&polygon_pointer, 0x00, sizeof(struct vdp1_cmdt_polygon));
                polygon_pointer.cp_color = zoom_point_color;
                polygon_pointer.cp_mode.transparent_pixel = true;
                polygon_pointer.cp_vertex.a.x = ZOOM_POINT_POINTER_SIZE + pointer.x - 1;
                polygon_pointer.cp_vertex.a.y = -ZOOM_POINT_POINTER_SIZE + pointer.y;
                polygon_pointer.cp_vertex.b.x = ZOOM_POINT_POINTER_SIZE + pointer.x - 1;
                polygon_pointer.cp_vertex.b.y = ZOOM_POINT_POINTER_SIZE + pointer.y - 1;
                polygon_pointer.cp_vertex.c.x = -ZOOM_POINT_POINTER_SIZE + pointer.x;
                polygon_pointer.cp_vertex.c.y = ZOOM_POINT_POINTER_SIZE + pointer.y - 1;
                polygon_pointer.cp_vertex.d.x = -ZOOM_POINT_POINTER_SIZE + pointer.x;
                polygon_pointer.cp_vertex.d.y = -ZOOM_POINT_POINTER_SIZE + pointer.y;

                struct vdp1_cmdt_sprite scaled_sprite;
                memset(&scaled_sprite, 0x00, sizeof(struct vdp1_cmdt_sprite));
                scaled_sprite.cs_type = CMDT_TYPE_SCALED_SPRITE;
                scaled_sprite.cs_zoom_point.enable = true;
                scaled_sprite.cs_zoom_point.raw = zoom_point_value;
                scaled_sprite.cs_mode.cc_mode = 0;
                scaled_sprite.cs_mode.color_mode = 4;
                scaled_sprite.cs_mode.transparent_pixel = true;
                scaled_sprite.cs_color_bank = 1;
                scaled_sprite.cs_char = CHAR(0);
                scaled_sprite.cs_width = ZOOM_POINT_WIDTH;
                scaled_sprite.cs_height = ZOOM_POINT_HEIGHT;
                scaled_sprite.cs_zoom.point.x = zoom_point.x;
                scaled_sprite.cs_zoom.point.y = zoom_point.y;
                scaled_sprite.cs_zoom.display.x = display.x;
                scaled_sprite.cs_zoom.display.y = display.y;

                vdp1_cmdt_system_clip_coord_set(&system_clip);
                vdp1_cmdt_user_clip_coord_set(&user_clip);
                vdp1_cmdt_local_coord_set(&local_coord);
                vdp1_cmdt_sprite_draw(&scaled_sprite);
                vdp1_cmdt_polygon_draw(&polygon_pointer);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

TEST_PROTOTYPE_DECLARE(scaled_sprite_01, draw)
{
        vdp1_cmdt_list_commit();
}

TEST_PROTOTYPE_DECLARE(scaled_sprite_01, exit)
{
}
