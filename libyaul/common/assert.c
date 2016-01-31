/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <common.h>

#include <vdp1.h>
#include <vdp2.h>

#include <cons.h>

void __noreturn
__assert_func(const char *file, int line, const char *func,
    const char *failed_expr)
{
        static char buf[4096];

        (void)snprintf(buf, 4096,
            "[2J[HAssertion \"%s\" failed: file \"%s\", line %d%s%s\n",
            failed_expr, file, line,
            (func ? ", function: " : ""),
            (func ? func : ""));

        /* Reset the VDP2 */
        vdp2_init();

        /* Reset the VDP1 */
        vdp1_init();
        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(3, 0x01FFFE), 0x80E0);

        cons_init(CONS_DRIVER_VDP2);
        cons_write(buf);

        abort();
}
