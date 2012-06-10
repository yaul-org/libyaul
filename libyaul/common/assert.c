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

void
_assert(const char *s, const char *cond, const char *file, uint32_t line, const char *function,
    const char *bt)
{
        static char buf[16384];
        static struct cons cons;

        static uint16_t blcs_color[] = {
                0x80E0 /* Color green */
        };

        (void)sprintf(buf, "%s:%lu: %s: Assertion `%s' failed: %s\n\n"
            "Stack backtrace\n\n%s",
            file, line, function, cond, s, bt);

        /* Reset the VDP2 */
        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        cons_vdp2_init(&cons);
        cons_write(&cons, buf);

        /* XXX
         * Refactor into using the 'abort' system-call
         */
        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }
}
