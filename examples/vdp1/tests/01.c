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

static struct vdp1_cmdt_polyline *polyline = NULL;
static int angle = 0;

void
test_01_init(void)
{
        init();

        angle = 0;

        polyline = (struct vdp1_cmdt_polyline *)malloc(
                sizeof(struct vdp1_cmdt_polyline));
        assert(polyline != NULL);
}

void
test_01_update(void)
{
        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        return;
                }
        }

        int16_t x;
        x = (64 * MATH_COS(4 * angle)) >> 12;

        int16_t y;
        y = -(64 * MATH_SIN(4 * angle)) >> 12;

        polyline[0].cp_color = RGB888_TO_RGB555(255, 255, 0);
        polyline[0].cp_mode.transparent_pixel = true;
        polyline[0].cp_mode.end_code = true;
        polyline[0].cp_vertex.a.x = 24 + x - 1;
        polyline[0].cp_vertex.a.y = -24 + y;
        polyline[0].cp_vertex.b.x = 24 + x - 1;
        polyline[0].cp_vertex.b.y = 24 + y - 1;
        polyline[0].cp_vertex.c.x = -24 + x;
        polyline[0].cp_vertex.c.y = 24 + y - 1;
        polyline[0].cp_vertex.d.x = -24 + x;
        polyline[0].cp_vertex.d.y = -24 + y;

        (void)sprintf(text, "[H[2Jx = %3i\ny = %3i\n",
            (int)x, (int)y);
        cons_buffer(&cons, text);

        angle++;
}

void
test_01_draw(void)
{
        cons_flush(&cons);

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_local_coord_set(320 / 2, 224 / 2);
                vdp1_cmdt_polyline_draw(&polyline[0]);
        } vdp1_cmdt_list_end(0);
}

void
test_01_exit(void)
{
        free(polyline);

        cons_write(&cons, "[H[2J");
}
