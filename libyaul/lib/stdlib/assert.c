/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <vdp1.h>
#include <vdp2.h>

#include <cons.h>

void __noreturn
__assert_func(const char *file, int line, const char *func,
    const char *failed_expr)
{
        static char buf[4096];

        (void)sprintf(buf,
            "[H[2JAssertion \"%s\" failed: file \"%s\", line %d%s%s\n",
            failed_expr, file, line,
            (func ? ", function: " : ""),
            (func ? func : ""));

        cpu_intc_disable();

        /* Reset the VDP2 */
        vdp2_init();
        vdp2_tvmd_display_res_set(TVMD_INTERLACE_NONE, TVMD_HORZ_NORMAL_A, TVMD_VERT_240);
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(0, 0x01FFFE),
                COLOR_RGB555(0, 7, 0));
        vdp2_tvmd_display_set();

        /* Reset the VDP1 */
        vdp1_init();

        cons_init(CONS_DRIVER_VDP2, 40, 30);
        cons_buffer(buf);

        vdp2_tvmd_vblank_out_wait();
        vdp2_tvmd_vblank_in_wait();
        cons_flush();

        abort();
}
