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

static struct vdp1_cmdt_line line[1];

void
test_05_init(void)
{
        test_init();

        memset(line, 0x00, sizeof(struct vdp1_cmdt_line));
}

void
test_05_update(void)
{
        if (digital_pad.connected == 1) {
                if (digital_pad.held.button.start) {
                        test_exit();
                }
        }

        line[0].cl_color = RGB888_TO_RGB555(255, 255,   0);
        line[0].cl_mode.transparent_pixel = true;
        line[0].cl_mode.end_code = true;
        line[0].cl_vertex.a.x = -128;
        line[0].cl_vertex.a.y = 0;
        line[0].cl_vertex.b.x = 128 - 1;
        line[0].cl_vertex.b.y = 0;

        vdp1_cmdt_list_begin(0); {
                vdp1_cmdt_local_coord_set(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
                vdp1_cmdt_line_draw(&line[0]);
                vdp1_cmdt_end();
        } vdp1_cmdt_list_end(0);
}

void
test_05_draw(void)
{
        cons_flush(&cons);

        vdp1_cmdt_list_commit();
}

void
test_05_exit(void)
{
}
