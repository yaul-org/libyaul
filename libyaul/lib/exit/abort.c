/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>

#include <sys/cdefs.h>

#include <cpu/intc.h>

/* To avoid from LTO discarding abort (as it's considered a builtin by GCC) */
void __noreturn __used
abort(void)
{
        cpu_intc_mask_set(15);

        while (true) {
        }

        __builtin_unreachable();
}
