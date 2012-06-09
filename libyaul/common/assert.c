/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <alloca.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <vdp2/scrn.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>
#include <vdp2.h>

#include <cons/vdp2.h>

#include "stack.h"

static void spin(void);

void __attribute__ ((noreturn))
_assert(const char *s, const char *cond, const char *file, uint32_t line, const char *function)
{
        static char buf[16384];

        uint16_t blcs_color[] = {
                0x80E0 /* Green */
        };

        struct cons cons;

        (void)sprintf(buf, "%s:%lu: %s: Assertion `%s' failed: %s\n"
            "[1B[1;44mStack backtrace[m\n%s",
            file, line, function, cond, s, stack_backtrace());

        /* Reset the VDP2 */
        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(0, 0x00000),
            blcs_color, 0);
        vdp2_tvmd_display_set(); /* Turn display ON */

        cons_vdp2_init(&cons);
        cons_write(&cons, buf);

        spin();
}

static void __attribute__ ((noreturn))
spin(void)
{
        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }
}
