/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc-internal.h"

/* Command type: Non-resetable system management commands */
uint8_t
smpc_smc_intback_call(uint8_t ireg0, uint8_t ireg1)
{
        /*
         * Issue the INTBACK command 300 microseconds after VBLANK-IN
         * and before VBLANK-OUT
         *
         * Peripheral data collection starts when the SMPC detects
         * VBLANK-OUT
         */
        uint8_t cmd_parameters[3];

        /* Fetch peripheral data "SMPC" status */
        cmd_parameters[0] = ireg0 & 0x01;
        cmd_parameters[1] = ireg1 & 0x7F;

        /* When issuing the "INTBACK" command, set this value. */
        cmd_parameters[2] = 0xF0;

        /* Retreive the "SMPC" status and or peripheral data iff not
         * called from slave "SH-2" */
        smpc_smc_call(SMPC_SMC_INTBACK, SMPC_CMD_ISSUE_TYPE_D, cmd_parameters);

        return 0;
}
