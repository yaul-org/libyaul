/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc-internal.h"

uint8_t
smpc_smc_sshon_call(void)
{
        /* Enable the "SH-2" slave CPU iff not called from slave
         * "SH-2" */
        smpc_smc_call(SMPC_SMC_SSHON, SMPC_CMD_ISSUE_TYPE_B, NULL);

        return MEMORY_READ(8, OREG(31));
}
