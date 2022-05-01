/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <stdbool.h>

#include <cpu/intc.h>

#include <vdp2.h>

#include <smpc.h>

/* To avoid from LTO discarding abort (as it's considered a builtin by GCC) */
void __noreturn __used
abort(void)
{
        /* Disable interrupts */
        cpu_intc_mask_set(15);

        /* This does not execute cleanup functions registered with 'atexit' or
         * 'on_exit' */
#ifdef DEBUG
#ifdef SPIN_ON_ABORT
        while (true) {
                /* atexit(); */
        }
#else
        smpc_smc_sysres_call();
        bios_execute();
#endif /* SPIN_ON_ABORT */
#endif /* DEBUG */

        __builtin_unreachable();
}
