/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/smc.h>

void
smpc_peripheral_data(void)
{
        /*
         * Send "INTBACK" "SMPC" command (300us after VBLANK-IN)
         * Set to 15-byte mode; optimized
         */
        smpc_smc_intback_call(0x00, 0x0A);
}
