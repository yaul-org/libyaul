/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <stdlib.h>

#include <dbgio.h>

#include <internal.h>

#define ASSERTION_MAX_COUNT 3

void __noreturn
_assert(const char * restrict file, const char * restrict line,
    const char * restrict func,
    const char * restrict failed_expr)
{
        static uint32_t assertions = 0;

        if (assertions >= ASSERTION_MAX_COUNT) {
                abort();
        }

        if (assertions == 0) {
                _internal_reset();

                dbgio_dev_default_init(DBGIO_DEV_VDP2_SIMPLE);

                dbgio_buffer("[H[2J");
        }

        if (assertions > 0) {
                dbgio_buffer("\n");
        }

        assertions++;

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

        dbgio_flush();

        abort();
}
