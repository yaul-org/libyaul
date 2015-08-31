/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "test.h"

TEST_PROTOTYPE_DECLARE(normal_sprite_00, init)
{
        void *file_handle;
        file_handle = fs_open("/TESTS/02/COLOR_MODE0.TGA");

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
}

TEST_PROTOTYPE_DECLARE(normal_sprite_00, update)
{
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

                static struct vdp1_cmdt_sprite normal_sprite;
                normal_sprite.cs_type = CMDT_TYPE_NORMAL_SPRITE;
                normal_sprite.cs_mode.cc_mode = 0;
                normal_sprite.cs_mode.color_mode = 1;
                normal_sprite.cs_mode.transparent_pixel = true;
                normal_sprite.cs_clut = CLUT(16, 0);
                normal_sprite.cs_char = CHAR(0);
                normal_sprite.cs_width = 64;
                normal_sprite.cs_height = 64;
                normal_sprite.cs_position.x = 0;
                normal_sprite.cs_position.y = 0;

                vdp1_cmdt_system_clip_coord_set(&system_clip);
                vdp1_cmdt_user_clip_coord_set(&user_clip);
                vdp1_cmdt_local_coord_set(&local_coord);
                vdp1_cmdt_sprite_draw(&normal_sprite);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

TEST_PROTOTYPE_DECLARE(normal_sprite_00, draw)
{
        vdp1_cmdt_list_commit();
}

TEST_PROTOTYPE_DECLARE(normal_sprite_00, exit)
{
}
