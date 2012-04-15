/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_cdoff_call(void)
{
        /* Disable the "SH-1" CD block. */
        smpc_cmd_call(SMPC_SMC_CDOFF, SMPC_CMD_ISSUE_TYPE_B, NULL);

        return MEM_READ(OREG(31));
}
