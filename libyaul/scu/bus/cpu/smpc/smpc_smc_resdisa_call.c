/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc-internal.h"

uint8_t
smpc_smc_resdisa_call(void)
{
        /* Disable the user from pressing the reset button */
        smpc_smc_call(SMPC_SMC_RESDISA, SMPC_CMD_ISSUE_TYPE_B, NULL);

        return MEMORY_READ(8, OREG(31));
}
