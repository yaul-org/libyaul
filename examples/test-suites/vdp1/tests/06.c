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

#include "../test.h"
#include "../common.h"
#include "../fs.h"

static struct vdp1_cmdt_polygon polygon[1];
static struct vdp1_cmdt_sprite sprite[5];

static void *file_handle[2];

static uint32_t cram[] = {
        CRAM_MODE_1_OFFSET(0, 1, 0),
        0
};

static uint32_t vram[2];

void
test_06_init(void)
{
        test_init();

        uint32_t vram_addr;
        vram_addr = CHAR(0);

        size_t file_size;

        file_handle[0] = fs_open("/TESTS/06/ECD.TGA");
        file_handle[1] = fs_open("/TESTS/06/HEADER.TGA");

        uint32_t sprite_idx;
        for (sprite_idx = 0; sprite_idx < 2; sprite_idx++) {
                void *fh;
                fh = file_handle[sprite_idx];
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
                amount = tga_cmap_decode(&tga, (uint16_t *)cram[sprite_idx]);
                if ((tga.tga_type == TGA_IMAGE_TYPE_CMAP) ||
                    (tga.tga_type == TGA_IMAGE_TYPE_RLE_CMAP)) {
                        assert(amount > 0);
                }

                vram[sprite_idx] = vram_addr;

                vram_addr += file_size;
        }
}

void
test_06_update(void)
{
        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        test_exit();
                }
        }

        polygon[0].cp_color = RGB888_TO_RGB555(255, 255,   0);
        polygon[0].cp_mode.transparent_pixel = true;
        polygon[0].cp_mode.end_code = true;
        polygon[0].cp_vertex.a.x = 128 - 1;
        polygon[0].cp_vertex.a.y = -64;
        polygon[0].cp_vertex.b.x = 128 - 1;
        polygon[0].cp_vertex.b.y = 64 - 1;
        polygon[0].cp_vertex.c.x = -128;
        polygon[0].cp_vertex.c.y = 64 - 1;
        polygon[0].cp_vertex.d.x = -128;
        polygon[0].cp_vertex.d.y = -64;

        sprite[0].cs_type = CMDT_TYPE_NORMAL_SPRITE;
        sprite[0].cs_mode.cc_mode = 0;
        sprite[0].cs_mode.color_mode = 0;
        sprite[0].cs_color_bank = 1;
        sprite[0].cs_mode.end_code = true;
        sprite[0].cs_mode.transparent_pixel = true;
        sprite[0].cs_char = vram[0];
        sprite[0].cs_color_bank = 1;
        sprite[0].cs_position.x = -128;
        sprite[0].cs_position.y = -64;
        sprite[0].cs_width = 64;
        sprite[0].cs_height = 128;

        sprite[1].cs_type = CMDT_TYPE_NORMAL_SPRITE;
        sprite[1].cs_mode.cc_mode = 0;
        sprite[1].cs_mode.color_mode = 0;
        sprite[1].cs_color_bank = 1;
        sprite[1].cs_mode.end_code = true;
        sprite[1].cs_mode.transparent_pixel = false;
        sprite[1].cs_char = vram[0];
        sprite[1].cs_color_bank = 1;
        sprite[1].cs_position.x = -64;
        sprite[1].cs_position.y = -64;
        sprite[1].cs_width = 64;
        sprite[1].cs_height = 128;

        sprite[2].cs_type = CMDT_TYPE_NORMAL_SPRITE;
        sprite[2].cs_mode.cc_mode = 0;
        sprite[2].cs_mode.color_mode = 0;
        sprite[2].cs_color_bank = 1;
        sprite[2].cs_mode.end_code = false;
        sprite[2].cs_mode.transparent_pixel = true;
        sprite[2].cs_char = vram[0];
        sprite[2].cs_color_bank = 1;
        sprite[2].cs_position.x = 0;
        sprite[2].cs_position.y = -64;
        sprite[2].cs_width = 64;
        sprite[2].cs_height = 128;

        sprite[3].cs_type = CMDT_TYPE_NORMAL_SPRITE;
        sprite[3].cs_mode.cc_mode = 0;
        sprite[3].cs_mode.color_mode = 0;
        sprite[3].cs_color_bank = 1;
        sprite[3].cs_mode.end_code = false;
        sprite[3].cs_mode.transparent_pixel = false;
        sprite[3].cs_char = vram[0];
        sprite[3].cs_color_bank = 1;
        sprite[3].cs_position.x = 64;
        sprite[3].cs_position.y = -64;
        sprite[3].cs_width = 64;
        sprite[3].cs_height = 128;

        sprite[4].cs_type = CMDT_TYPE_NORMAL_SPRITE;
        sprite[4].cs_mode.cc_mode = 0;
        sprite[4].cs_mode.color_mode = 5;
        sprite[4].cs_mode.transparent_pixel = true;
        sprite[4].cs_mode.end_code = false;
        sprite[4].cs_char = vram[1];
        sprite[4].cs_position.x = -128;
        sprite[4].cs_position.y = -112;
        sprite[4].cs_width = 256;
        sprite[4].cs_height = 64;

        vdp1_cmdt_list_begin(0); {
                struct vdp1_cmdt_local_coord local_coord;
                local_coord.lc_coord.x = SCREEN_WIDTH / 2;
                local_coord.lc_coord.y = SCREEN_HEIGHT / 2;

                vdp1_cmdt_local_coord_set(&local_coord);

                vdp1_cmdt_polygon_draw(&polygon[0]);
                vdp1_cmdt_sprite_draw(&sprite[0]);
                vdp1_cmdt_sprite_draw(&sprite[1]);
                vdp1_cmdt_sprite_draw(&sprite[2]);
                vdp1_cmdt_sprite_draw(&sprite[3]);
                vdp1_cmdt_sprite_draw(&sprite[4]);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

void
test_06_draw(void)
{
        cons_flush(&cons);

        vdp1_cmdt_list_commit();
}

void
test_06_exit(void)
{
}
