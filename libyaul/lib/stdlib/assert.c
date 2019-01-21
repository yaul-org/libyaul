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
#include <stdint.h>
#include <string.h>

#include <cpu/intc.h>
#include <cpu/dmac.h>

#include <scu/dma.h>

#include <internal.h>

void __noreturn
__assert_func(const char *file, int line, const char *func,
    const char *failed_expr)
{
        static char buffer[4096];
        static bool asserted = false;

        bool has_func_str;
        has_func_str = (func != NULL) || (*func != '\0');

        /* Avoid adding code to clear the screen if we've asserted more than
         * once */
        const char *clear_screen;
        clear_screen = (!asserted) ? "" : "[H[2J";

        (void)sprintf(buffer,
            "%s"
            "Assertion \"%s\" failed: file \"%s\", line %d%s%s\n",
            clear_screen,
            failed_expr,
            file,
            line,
            ((has_func_str) ? ", function: " : ""),
            ((has_func_str) ? func : ""));

        /* If we crash within an assert, then let's fallback to simply copying
         * the assert buffer to L-WRAM */
        if (asserted) {
                cpu_intc_mask_set(15);

                cpu_dmac_stop();
                scu_dma_stop();

                (void)memcpy((void *)0x20200000, buffer, sizeof(buffer));

                abort();
        }

        asserted = true;

        _internal_exception_show(buffer);
}
