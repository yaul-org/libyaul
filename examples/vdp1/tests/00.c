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

static struct vdp1_cmdt_polygon *polygon = NULL;
static uint32_t angle = 0;

void
test_00_init(void)
{
        angle = 0;

        polygon = (struct vdp1_cmdt_polygon *)malloc(
                sizeof(struct vdp1_cmdt_polygon));
        assert(polygon != NULL);
}

void
test_00_update(void)
{
        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        test_exit();
                }
        }

        int16_t x;
        x = ((64) * MATH_COS((4 * angle))) >> 12;

        int16_t y;
        y = -((64) * MATH_SIN((4 * angle))) >> 12;

        polygon[0].cp_color = RGB888_TO_RGB555(255, 255, 0);
        polygon[0].cp_mode.transparent_pixel = true;
        polygon[0].cp_mode.end_code = true;
        polygon[0].cp_vertex.a.x = 24 + x - 1;
        polygon[0].cp_vertex.a.y = -24 + y;
        polygon[0].cp_vertex.b.x = 24 + x - 1;
        polygon[0].cp_vertex.b.y = 24 + y - 1;
        polygon[0].cp_vertex.c.x = -24 + x;
        polygon[0].cp_vertex.c.y = 24 + y - 1;
        polygon[0].cp_vertex.d.x = -24 + x;
        polygon[0].cp_vertex.d.y = -24 + y;

        angle++;

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_local_coord_set(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                vdp1_cmdt_polygon_draw(&polygon[0]);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

void
test_00_draw(void)
{
        vdp1_cmdt_list_commit();
}

void
test_00_exit(void)
{
        free(polygon);

        cons_write(&cons, "[H[2J");
}
