/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "test.h"

#define STATE_SCALE_INVALID             -1
#define STATE_SCALE_MOVE_ORIGIN         0
#define STATE_SCALE_WAIT                1
#define STATE_SCALE_WAIT_DIR            2
#define STATE_SCALE_MOVE_ANCHOR         3
#define STATE_SCALE_RELEASE_BUTTONS     4
#define STATE_SCALE                     5
#define STATE_SCALE_DIR                 6
#define STATE_SCALE_BACK                7

#define SCALE_POINTER_SIZE              3
#define SCALE_POINTER_COLOR_SELECT      RGB888_TO_RGB555(  0,   0, 255)
#define SCALE_POINTER_COLOR_WAIT        RGB888_TO_RGB555(255,   0,   0)
#define SCALE_POINTER_COLOR_HIGHLIGHT   RGB888_TO_RGB555(  0, 255,   0)

/* Dependent on size of sprite */
#define SCALE_WIDTH     64
#define SCALE_HEIGHT    64

static int32_t state_scale = STATE_SCALE_INVALID;
static int16_vector2_t scale_points[2];
static int16_vector2_t *scale_point = NULL;
static int16_vector2_t pointer = INT16_VECTOR2_INITIALIZER(0, 0);
static uint16_t pointer_color = RGB888_TO_RGB555(0, 0, 0);
static uint16_t captured_buttons = 0xFFFF;
static uint32_t delay_frames = 0;
static fix16_t scaled = 0;

/* Lookup table for exponential decay (N(t)=N_0*e^(-Dt))
 *   where N_0 is the scaled size, D is the decay rate, and t=[0,60). */
static const fix16_t scale_back[] = {
        F16(1.00000f), /* t = 0 */
        F16(0.91310f), /* t = 1 */
        F16(0.83375f), /* t = 2 */
        F16(0.76130f), /* t = 3 */
        F16(0.69514f), /* t = 4 */
        F16(0.63474f), /* t = 5 */
        F16(0.57958f), /* t = 6 */
        F16(0.52921f), /* t = 7 */
        F16(0.48323f), /* t = 8 */
        F16(0.44123f), /* t = 9 */
        F16(0.40289f), /* t = 10 */
        F16(0.36788f), /* t = 11 */
        F16(0.33591f), /* t = 12 */
        F16(0.30672f), /* t = 13 */
        F16(0.28007f), /* t = 14 */
        F16(0.25573f), /* t = 15 */
        F16(0.23351f), /* t = 16 */
        F16(0.21321f), /* t = 17 */
        F16(0.19469f), /* t = 18 */
        F16(0.17777f), /* t = 19 */
        F16(0.16232f), /* t = 20 */
        F16(0.14822f), /* t = 21 */
        F16(0.13534f), /* t = 22 */
        F16(0.12357f), /* t = 23 */
        F16(0.11284f), /* t = 24 */
        F16(0.10303f), /* t = 25 */
        F16(0.09408f), /* t = 26 */
        F16(0.08590f), /* t = 27 */
        F16(0.07844f), /* t = 28 */
        F16(0.07162f), /* t = 29 */
        F16(0.06540f), /* t = 30 */
        F16(0.05971f), /* t = 31 */
        F16(0.05453f), /* t = 32 */
        F16(0.04979f), /* t = 33 */
        F16(0.04546f), /* t = 34 */
        F16(0.04151f), /* t = 35 */
        F16(0.03790f), /* t = 36 */
        F16(0.03461f), /* t = 37 */
        F16(0.03160f), /* t = 38 */
        F16(0.02886f), /* t = 39 */
        F16(0.02635f), /* t = 40 */
        F16(0.02406f), /* t = 41 */
        F16(0.02197f), /* t = 42 */
        F16(0.02006f), /* t = 43 */
        F16(0.01832f), /* t = 44 */
        F16(0.01672f), /* t = 45 */
        F16(0.01527f), /* t = 46 */
        F16(0.01394f), /* t = 47 */
        F16(0.01273f), /* t = 48 */
        F16(0.01163f), /* t = 49 */
        F16(0.01062f), /* t = 50 */
        F16(0.00969f), /* t = 51 */
        F16(0.00885f), /* t = 52 */
        F16(0.00808f), /* t = 53 */
        F16(0.00738f), /* t = 54 */
        F16(0.00674f), /* t = 55 */
        F16(0.00615f), /* t = 56 */
        F16(0.00562f), /* t = 57 */
        F16(0.00513f), /* t = 58 */
        F16(0.00468f) /* t = 59 */
};

TEST_PROTOTYPE_DECLARE(scaled_sprite_00, init)
{
        void *file_handle;
        file_handle = fs_open("/TESTS/03/SCALE.TGA");

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
        amount = tga_cmap_decode(&tga, (uint16_t *)CLUT(16, 0));
        assert(amount > 0);

        state_scale = STATE_SCALE_MOVE_ORIGIN;
}

TEST_PROTOTYPE_DECLARE(scaled_sprite_00, update)
{
        bool dirs_pressed;
        dirs_pressed = false;
        bool scale_pressed;
        scale_pressed = false;

        bool upper_left;
        upper_left = false;
        bool bottom_right;
        bottom_right = false;

        fix16_t scale;
        scale = F16(0.0f);

        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        test_exit();
                }

                dirs_pressed =
                    (digital_pad.pressed.raw & PERIPHERAL_DIGITAL_DIRECTIONS) != 0;
                scale_pressed = digital_pad.pressed.button.a ||
                    digital_pad.pressed.button.c;
        }

        switch (state_scale) {
        case STATE_SCALE_MOVE_ORIGIN:
                pointer.x = 0;
                pointer.y = 0;

                pointer_color = SCALE_POINTER_COLOR_SELECT;

                scale_points[0].x = 0;
                scale_points[0].y = 0;

                scale_points[1].x = 0;
                scale_points[1].y = 0;

                scale_point = NULL;

                delay_frames = 0;

                if (dirs_pressed) {
                        captured_buttons = digital_pad.pressed.raw;
                        state_scale = STATE_SCALE_WAIT_DIR;
                } else if (((digital_pad.pressed.button.a) != 0) ||
                           ((digital_pad.pressed.button.c) != 0)) {
                        state_scale = STATE_SCALE_WAIT;
                }
                break;
        case STATE_SCALE_WAIT:
                if (scale_pressed) {
                        captured_buttons = digital_pad.pressed.raw;
                }

                delay_frames++;

                if (delay_frames > 8) {
                        delay_frames = 0;
                        state_scale = STATE_SCALE;
                } else if (!scale_pressed) {
                        delay_frames = 0;
                        state_scale = STATE_SCALE_MOVE_ORIGIN;
                }
                break;
        case STATE_SCALE_WAIT_DIR:
                if (dirs_pressed) {
                        captured_buttons = digital_pad.pressed.raw;
                }

                delay_frames++;

                if (delay_frames > 10) {
                        delay_frames = 0;
                        state_scale = STATE_SCALE_MOVE_ANCHOR;
                } else if (!dirs_pressed) {
                        delay_frames = 0;
                        state_scale = STATE_SCALE_MOVE_ORIGIN;
                }
                break;
        case STATE_SCALE_MOVE_ANCHOR:
                if (((captured_buttons & PERIPHERAL_DIGITAL_UP) != 0) &&
                    ((captured_buttons & PERIPHERAL_DIGITAL_LEFT) != 0)) {
                        pointer.x = -SCALE_WIDTH / 2;
                        pointer.y = -SCALE_HEIGHT / 2;
                }

                if (((captured_buttons & PERIPHERAL_DIGITAL_RIGHT) != 0) &&
                    ((captured_buttons & PERIPHERAL_DIGITAL_DOWN) != 0)){
                        pointer.x = SCALE_WIDTH / 2;
                        pointer.y = SCALE_HEIGHT / 2;
                }

                upper_left = (pointer.x < 0) && (pointer.y < 0);
                bottom_right = (pointer.x > 0) && (pointer.y >0);

                if (upper_left) {
                        scale_point = &scale_points[1];
                } else if (bottom_right) {
                        scale_point = &scale_points[0];
                }

                captured_buttons = digital_pad.pressed.raw;

                if (!dirs_pressed) {
                        state_scale = STATE_SCALE_MOVE_ORIGIN;
                } else if (((digital_pad.pressed.button.a) != 0) ||
                           ((digital_pad.pressed.button.c) != 0)) {
                        state_scale = STATE_SCALE_RELEASE_BUTTONS;
                }
                break;
        case STATE_SCALE_RELEASE_BUTTONS:
                pointer_color = SCALE_POINTER_COLOR_WAIT;

                if (!dirs_pressed) {
                        state_scale = STATE_SCALE_DIR;
                }
                break;
        case STATE_SCALE:
                pointer_color = SCALE_POINTER_COLOR_HIGHLIGHT;

                if (!scale_pressed) {
                        scaled = -fix16_from_int(scale_points[0].x);
                        delay_frames = 0;
                        state_scale = STATE_SCALE_BACK;
                } else {
                        scale_points[0].x--;
                        scale_points[0].y--;
                        scale_points[1].x++;
                        scale_points[1].y++;
                }
                break;
        case STATE_SCALE_DIR:
                pointer_color = SCALE_POINTER_COLOR_HIGHLIGHT;

                if ((digital_pad.pressed.button.up) != 0) {
                        scale_point->y--;
                }

                if ((digital_pad.pressed.button.down) != 0) {
                        scale_point->y++;
                }

                if ((digital_pad.pressed.button.left) != 0) {
                        scale_point->x--;;
                }

                if ((digital_pad.pressed.button.right) != 0) {
                        scale_point->x++;
                }

                if ((digital_pad.held.button.b) != 0) {
                        state_scale = STATE_SCALE_MOVE_ORIGIN;
                }
                break;
        case STATE_SCALE_BACK:
                pointer_color = SCALE_POINTER_COLOR_WAIT;

                if ((delay_frames == 60) ||
                    ((scale_points[0].x <= 0) && (scale_points[0].y <= 0) &&
                     (scale_points[1].x <= 0) && (scale_points[1].y <= 0))) {
                        scale_points[0].x = 0;
                        scale_points[0].y = 0;
                        scale_points[1].x = 0;
                        scale_points[1].y = 0;

                        delay_frames = 0;
                        state_scale = STATE_SCALE_MOVE_ORIGIN;
                } else {
                        scale = fix16_mul(scaled, scale_back[delay_frames]);

                        scale_points[0].x = -fix16_to_int(scale);
                        scale_points[0].y = -fix16_to_int(scale);
                        scale_points[1].x = fix16_to_int(scale);
                        scale_points[1].y = fix16_to_int(scale);

                        delay_frames++;
                }
                break;
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

                struct vdp1_cmdt_sprite scaled_sprite;
                memset(&scaled_sprite, 0x00, sizeof(struct vdp1_cmdt_sprite));
                scaled_sprite.cs_type = CMDT_TYPE_SCALED_SPRITE;
                scaled_sprite.cs_mode.cc_mode = 0;
                scaled_sprite.cs_mode.color_mode = 1;
                scaled_sprite.cs_mode.transparent_pixel = true;
                scaled_sprite.cs_mode.high_speed_shrink = false;
                scaled_sprite.cs_clut = CLUT(16, 0);
                scaled_sprite.cs_char = CHAR(0);
                scaled_sprite.cs_width = SCALE_WIDTH;
                scaled_sprite.cs_height = SCALE_HEIGHT;
                scaled_sprite.cs_vertex.a.x = scale_points[0].x - ((SCALE_WIDTH / 2) - 1);
                scaled_sprite.cs_vertex.a.y = scale_points[0].y - ((SCALE_HEIGHT / 2) - 1);
                scaled_sprite.cs_vertex.c.x = scale_points[1].x + ((SCALE_WIDTH / 2) - 1);
                scaled_sprite.cs_vertex.c.y = scale_points[1].y + ((SCALE_HEIGHT / 2) - 1);

                static struct vdp1_cmdt_polygon polygon_pointer;
                memset(&polygon_pointer, 0x00, sizeof(struct vdp1_cmdt_polygon));
                polygon_pointer.cp_color = pointer_color;
                polygon_pointer.cp_mode.transparent_pixel = true;
                polygon_pointer.cp_vertex.a.x = SCALE_POINTER_SIZE + pointer.x - 1;
                polygon_pointer.cp_vertex.a.y = -SCALE_POINTER_SIZE + pointer.y;
                polygon_pointer.cp_vertex.b.x = SCALE_POINTER_SIZE + pointer.x - 1;
                polygon_pointer.cp_vertex.b.y = SCALE_POINTER_SIZE + pointer.y - 1;
                polygon_pointer.cp_vertex.c.x = -SCALE_POINTER_SIZE + pointer.x;
                polygon_pointer.cp_vertex.c.y = SCALE_POINTER_SIZE + pointer.y - 1;
                polygon_pointer.cp_vertex.d.x = -SCALE_POINTER_SIZE + pointer.x;
                polygon_pointer.cp_vertex.d.y = -SCALE_POINTER_SIZE + pointer.y;

                vdp1_cmdt_system_clip_coord_set(&system_clip);
                vdp1_cmdt_user_clip_coord_set(&user_clip);
                vdp1_cmdt_local_coord_set(&local_coord);
                vdp1_cmdt_sprite_draw(&scaled_sprite);
                vdp1_cmdt_polygon_draw(&polygon_pointer);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

TEST_PROTOTYPE_DECLARE(scaled_sprite_00, draw)
{
        vdp1_cmdt_list_commit();
}

TEST_PROTOTYPE_DECLARE(scaled_sprite_00, exit)
{
}
