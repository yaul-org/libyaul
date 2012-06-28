/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc-internal.h"

/* Command type: Resetable system management commands */
uint8_t
smpc_smc_sndon_call(void)
{
        /* Enable the "Motorola MC68EC000" */
        smpc_smc_call(SMPC_SMC_SNDON, SMPC_CMD_ISSUE_TYPE_B, NULL);

        return MEMORY_READ(8, OREG(31));
}
