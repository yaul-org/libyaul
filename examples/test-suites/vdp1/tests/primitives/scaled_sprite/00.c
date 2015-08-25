/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "test.h"

/* Globals */

static int16_vector2_t scale[2];

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
        amount = tga_cmap_decode(&tga, (uint16_t *)CLUT(0, 0));
        assert(amount > 0);

        scale[0].x = 0;
        scale[0].y = 0;

        scale[1].x = 0;
        scale[1].y = 0;
}

TEST_PROTOTYPE_DECLARE(scaled_sprite_00, update)
{
        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        test_exit();
                }

                if (digital_pad.held.button.r) {
                        scale[0].x = 0;
                        scale[0].y = 0;

                        scale[1].x = 0;
                        scale[1].y = 0;
                } else {
                        if (digital_pad.pressed.button.a) {
                                if (digital_pad.pressed.button.left) {
                                        scale[0].x--;
                                }
                                if (digital_pad.pressed.button.right) {
                                        scale[0].x++;
                                }
                                if (digital_pad.pressed.button.up) {
                                        scale[0].y--;
                                }
                                if (digital_pad.pressed.button.down) {
                                        scale[0].y++;
                                }
                        }
                        if (digital_pad.pressed.button.c) {
                                if (digital_pad.pressed.button.left) {
                                        scale[1].x--;
                                }
                                if (digital_pad.pressed.button.right) {
                                        scale[1].x++;
                                }
                                if (digital_pad.pressed.button.up) {
                                        scale[1].y--;
                                }
                                if (digital_pad.pressed.button.down) {
                                        scale[1].y++;
                                }
                        }
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

                struct vdp1_cmdt_sprite scaled_sprite;
                scaled_sprite.cs_type = CMDT_TYPE_SCALED_SPRITE;
                scaled_sprite.cs_mode.cc_mode = 0;
                scaled_sprite.cs_mode.color_mode = 1;
                scaled_sprite.cs_color_bank = 1;
                scaled_sprite.cs_mode.transparent_pixel = false;
                scaled_sprite.cs_mode.high_speed_shrink = false;
                scaled_sprite.cs_clut = CLUT(0, 0);
                scaled_sprite.cs_char = CHAR(0);
                scaled_sprite.cs_vertex.a.x = scale[0].x - 32 - 1;
                scaled_sprite.cs_vertex.a.y = scale[0].y - 32 - 1;
                scaled_sprite.cs_vertex.c.x = scale[1].x + 32 - 1;
                scaled_sprite.cs_vertex.c.y = scale[1].y + 32 - 1;
                scaled_sprite.cs_width = 64;
                scaled_sprite.cs_height = 64;

                vdp1_cmdt_system_clip_coord_set(&system_clip);
                vdp1_cmdt_user_clip_coord_set(&user_clip);
                vdp1_cmdt_local_coord_set(&local_coord);
                vdp1_cmdt_sprite_draw(&scaled_sprite);
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
