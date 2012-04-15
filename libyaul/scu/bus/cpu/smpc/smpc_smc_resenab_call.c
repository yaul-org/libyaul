/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

/* Command type: resetable system management commands. */
uint8_t
smpc_cmd_resenab_call(void)
{
        /* Allow the user to press the reset button. */
        smpc_cmd_call(SMPC_SMC_RESENAB, SMPC_CMD_ISSUE_TYPE_B, NULL);

        return MEM_READ(OREG(31));
}
