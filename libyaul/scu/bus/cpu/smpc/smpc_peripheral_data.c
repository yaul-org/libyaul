/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
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
