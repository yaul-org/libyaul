/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc-internal.h"

/* Command type: Resetable system management commands */
uint8_t
smpc_smc_sshon_call(void)
{
        /* Enable the "SH-2" master CPU */
        smpc_smc_call(SMPC_SMC_MSHON, SMPC_CMD_ISSUE_TYPE_A, NULL);

        return MEMORY_READ(8, OREG(31));
}
