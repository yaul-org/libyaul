/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <stdlib.h>

#include <vdp.h>

#include <dbgio.h>

#include <internal.h>

#define ASSERTION_MAX_COUNT 3

void __noreturn
_assert(const char * restrict file, const char * restrict line,
    const char * restrict func,
    const char * restrict failed_expr)
{
        /* In the case where we fail an assertion within _assert() */
        static uint32_t assertion_count = 0;

        if (assertion_count >= ASSERTION_MAX_COUNT) {
                abort();
        }

        if (assertion_count == 0) {
                _internal_reset();

                dbgio_dev_deinit();
                dbgio_dev_default_init(DBGIO_DEV_VDP2_SIMPLE);

                if (vdp2_tvmd_display()) {
                        vdp2_tvmd_vblank_out_wait();
                }

                vdp2_tvmd_vblank_in_wait();

                dbgio_dev_font_load();

                dbgio_buffer("[H[2J");
        }

        if (assertion_count > 0) {
                dbgio_buffer("\n");
        }

        assertion_count++;

        dbgio_buffer("Assertion \"");
        dbgio_buffer(failed_expr);
        dbgio_buffer("\" failed");

        dbgio_buffer(": file \"");
        dbgio_buffer(file);
        dbgio_buffer("\"");

        dbgio_buffer(", line ");
        dbgio_buffer(line);

        if ((func != NULL) && (*func != '\0')) {
                dbgio_buffer(", function: ");
                dbgio_buffer(func);
        }

        dbgio_buffer("\n");

        if (vdp2_tvmd_display()) {
                vdp2_tvmd_vblank_out_wait();
        }

        vdp2_tvmd_vblank_in_wait();

        dbgio_flush();
        vdp2_sync_commit();

        abort();
}
