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

#include "test.h"
#include "common.h"
#include "fs.h"

static struct vdp1_cmdt_sprite *sprite;

static uint32_t angle = 0;

static int16_vector2_t vertex[4];

static void rotate_z(int16_vector2_t *, int16_vector2_t *);

void
test_04_init(void)
{
        init();

        sprite = (struct vdp1_cmdt_sprite *)malloc(
                sizeof(struct vdp1_cmdt_sprite));
        assert(sprite != NULL);

        void *fh;
        fh = fs_open("/TESTS/03/SCALE.TGA");

        uint8_t *ptr;
        ptr = (uint8_t *)0x00200000;

        fs_read(fh, ptr);
        fs_close(fh);

        tga_t tga;
        int status;
        status = tga_read(&tga, ptr, (void *)CHAR(0),
            (uint16_t *)CRAM_OFFSET(0, 1, 0));
        assert(status == TGA_FILE_OK);

        angle = 0;

        vertex[0].x = -128;
        vertex[0].y = -128;

        vertex[1].x = 128;
        vertex[1].y = -128;

        vertex[2].x = 128;
        vertex[2].y = 128;

        vertex[3].x = -128;
        vertex[3].y = 128;
}

void
test_04_update(void)
{
        int16_vector2_t *out_vertex[4];

        out_vertex[0] = (int16_vector2_t *)&sprite[0].cs_vertex.a;
        out_vertex[1] = (int16_vector2_t *)&sprite[0].cs_vertex.b;
        out_vertex[2] = (int16_vector2_t *)&sprite[0].cs_vertex.c;
        out_vertex[3] = (int16_vector2_t *)&sprite[0].cs_vertex.d;

        rotate_z(&vertex[0], out_vertex[0]);
        rotate_z(&vertex[1], out_vertex[1]);
        rotate_z(&vertex[2], out_vertex[2]);
        rotate_z(&vertex[3], out_vertex[3]);

        angle++;

        sprite[0].cs_type = CMDT_TYPE_DISTORTED_SPRITE;
        sprite[0].cs_mode.cc_mode = 0;
        sprite[0].cs_mode.color_mode = 0;
        sprite[0].cs_color_bank = 1;
        sprite[0].cs_mode.end_code = true;
        sprite[0].cs_mode.transparent_pixel = false;
        sprite[0].cs_mode.high_speed_shrink = false;
        sprite[0].cs_char = CHAR(0);
        sprite[0].cs_color_bank = 1;
        sprite[0].cs_width = 64;
        sprite[0].cs_height = 64;

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_local_coord_set(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                vdp1_cmdt_sprite_draw(&sprite[0]);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

void
test_04_draw(void)
{
        cons_flush(&cons);

        vdp1_cmdt_list_commit();
}

void
test_04_exit(void)
{
        free(sprite);
}

static void
rotate_z(int16_vector2_t *v, int16_vector2_t *w)
{
        int32_t sin;
        int32_t cos;

        sin = MATH_SIN(2 * angle);
        cos = MATH_COS(2 * angle);

        int16_t x;
        x = ((v->x * (cos)) >> 12) - ((v->y * (sin)) >> 12);

        int16_t y;
        y = ((v->x * (sin)) >> 12) + ((v->y * (cos)) >> 12);

        w->x = x;
        w->y = y;
}
