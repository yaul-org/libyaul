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

#include <vdp2/tvmd.h>
#include <vdp2/scrn.h>
#include <vdp2.h>

#include "monitor.h"

#define BLCS_COL(x) (0x0001FFFE + (x))

static void spin(void);

void __attribute__ ((noreturn))
panic(const char *s, const char *cond, const char *file, uint32_t line, const char *function)
{
        char *buf;

        uint16_t blcs_color[] = {
                0x80E0 /* Green */
        };

        buf = alloca(1024 + strlen(s) + 1);
        (void)sprintf(buf, "%s:%lu: %s: Assertion `%s' failed: %s",
            file, line, function, cond, s);

        /* Reset the VDP2 */
        vdp2_init();
        vdp2_tvmd_ed_set(); /* Turn display ON */
        vdp2_scrn_blcs_set(/* lcclmd = */ false, 3, BLCS_COL(0), blcs_color);

        monitor_init();

        /* Wait until we can draw */
        while (vdp2_tvmd_vblank_status_get() == 0);
        while (vdp2_tvmd_vblank_status_get());
        vt100_write(monitor, buf);

        spin();
}

static void __attribute__ ((noreturn))
spin(void)
{
        while (true) {
                while (vdp2_tvmd_vblank_status_get() == 0);
                while (vdp2_tvmd_vblank_status_get());
        }
}
