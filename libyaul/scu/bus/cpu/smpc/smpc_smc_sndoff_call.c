/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

/* Command type: resetable system management commands. */
uint8_t
smpc_cmd_sndoff_call(void)
{
        /* Disable the "Motorola MC68EC000." */
        smpc_cmd_call(SMPC_SMC_SNDOFF, SMPC_CMD_ISSUE_TYPE_B, NULL);

        return MEM_READ(OREG(31));
}
