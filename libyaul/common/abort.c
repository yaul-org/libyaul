/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>

#include <common.h>

#include <cpu/intc.h>

#include <vdp2.h>

#if defined(DEBUG) || defined(RELEASE_INTERNAL)
#include <usb-cartridge.h>
#else
#include <smpc.h>
#endif /* defined(DEBUG) || defined(RELEASE_INTERNAL) */

void __noreturn
abort(void)
{
        /* Disable interrupts */
        cpu_intc_disable();

        /* This does not execute cleanup functions registered with
         * 'atexit' or 'on_exit' */
#if defined(DEBUG) || defined(RELEASE_INTERNAL)
#if HAVE_DEV_CARTRIDGE == 0
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
#else
        /* We reboot for error */
        smpc_smc_sysres_call();
#endif /* defined(DEBUG) || debug(RELEASE_INTERNAL) */

        while (true) {
        }
}
