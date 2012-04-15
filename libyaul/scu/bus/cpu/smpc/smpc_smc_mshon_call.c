/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

/* Command type: resetable system management commands. */
uint8_t
smpc_cmd_sshon_call(void)
{
        /* Enable the "SH-2" master CPU. */
        smpc_cmd_call(SMPC_SMC_MSHON, SMPC_CMD_ISSUE_TYPE_A, NULL);

        return MEM_READ(OREG(31));
}
