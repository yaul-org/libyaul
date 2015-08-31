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

void __noreturn
abort(void)
{
        /* Disable interrupts */
        cpu_intc_disable();

        /* This does not execute cleanup functions registered with
         * 'atexit' or 'on_exit' */
        while (true) {
                vdp2_tvmd_vblank_out_wait();
                vdp2_tvmd_vblank_in_wait();
        }
}
