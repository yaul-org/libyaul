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

#include <internal.h>

void __noreturn
__assert_func(const char *file, int line, const char *func,
    const char *failed_expr)
{
        static char buffer[4096];

        bool has_func_str;
        has_func_str = (func != NULL) || (*func != '\0');

        (void)sprintf(buffer,
            "[H[2JAssertion \"%s\" failed: file \"%s\", line %d%s%s\n",
            failed_expr,
            file,
            line,
            ((has_func_str) ? ", function: " : ""),
            ((has_func_str) ? func : ""));

        _internal_exception_show(buffer);
}
