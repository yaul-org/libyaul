/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_sshon_call(void)
{
        /* Enable the "SH-2" slave CPU iff not called from slave "SH-2." */
        smpc_cmd_call(SMPC_SMC_SSHON, SMPC_CMD_ISSUE_TYPE_B, NULL);

        return MEM_READ(OREG(31));
}
