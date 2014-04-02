/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vdp2/scrn.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>
#include <vdp2.h>

#include <cons.h>

void __attribute__ ((noreturn))
__assert_func(const char *file, int line, const char *func,
    const char *failed_expr)
{
        static char buf[4096];
        static struct cons cons;

        static uint16_t single_color[] = {
                0x80E0 /* Color green */
        };

        (void)snprintf(buf, 4096,
            "[2J[HAssertion \"%s\" failed: file \"%s\", line %d%s%s\n",
            failed_expr, file, line,
            (func ? ", function: " : ""),
            (func ? func : ""));

        /* Reset the VDP2 */
        vdp2_init();

        vdp2_scrn_back_screen_set(/* single_color = */ true,
            VRAM_ADDR_4MBIT(3, 0x01FFFE), single_color, 1);

        cons_init(&cons, CONS_DRIVER_VDP2);
        cons_buffer(&cons, buf);

        /* Wait for VBLANK */
        vdp2_tvmd_vblank_out_wait();
        vdp2_tvmd_vblank_in_wait();

        cons_write(&cons);

        abort();
}
