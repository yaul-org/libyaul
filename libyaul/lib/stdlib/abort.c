/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <stdbool.h>

#include <cpu/intc.h>

#include <vdp2.h>

#include <smpc.h>

#if defined(DEBUG) || defined(RELEASE_INTERNAL)
#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
#include <usb-cartridge.h>
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
#include <arp.h>
#elif HAVE_DEV_CARTRIDGE != 0
#error "Invalid `HAVE_DEV_CARTRIDGE' value"
#endif
#endif /* defined(DEBUG) || defined(RELEASE_INTERNAL) */

void __noreturn
abort(void)
{
        /* Disable interrupts */
        cpu_intc_disable();

        /* This does not execute cleanup functions registered with
         * 'atexit' or 'on_exit' */
#if defined(DEBUG) || defined(RELEASE_INTERNAL)
#if HAVE_DEV_CARTRIDGE == 0 /* No dev cartridge */
        while (true) {
                vdp2_tvmd_vblank_out_wait();
                vdp2_tvmd_vblank_in_wait();
        }
#elif HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        usb_cartridge_return();
#elif HAVE_DEV_CARTRIDGE == 2 /* Datel Action Replay cartridge */
        arp_return();
#else
#error "Invalid `HAVE_DEV_CARTRIDGE' value"
#endif
#else /* !defined(DEBUG) && !defined(RELEASE_INTERNAL) */
        smpc_smc_sysres_call();
#endif /* defined(DEBUG) || defined(RELEASE_INTERNAL) */
}
