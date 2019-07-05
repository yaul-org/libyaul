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

#ifdef DEBUG
#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
#include <usb-cart.h>
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
#include <arp.h>
#elif HAVE_DEV_CARTRIDGE != 0
#error "Invalid `HAVE_DEV_CARTRIDGE' value"
#endif
#endif /* defined(DEBUG) */

void __noreturn
abort(void)
{
        /* Disable interrupts */
        cpu_intc_mask_set(15);

        /* This does not execute cleanup functions registered with
         * 'atexit' or 'on_exit' */
#ifdef DEBUG
#if HAVE_DEV_CARTRIDGE == 0 /* No dev cartridge */
        while (true) {
        }
#elif HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        while (true) {
                /* XXX: Call for waiting for data instead of returning */
        }
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
        while (true) {
                arp_function_nonblock();
        }
#else
#error "Invalid `HAVE_DEV_CARTRIDGE' value"
#endif
#else /* DEBUG */
#ifdef SPIN_ON_ABORT
        while (true) {
        }
#else
        smpc_smc_sysres_call();
#endif /* SPIN_ON_ABORT */
#endif /* DEBUG */

        __builtin_unreachable();
}
