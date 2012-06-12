/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_ckchg352_call(void)
{
        /* Switch the SATURN system clock from 320 mode to 352 mode iff
         * not called from slave "SH-2." */
        smpc_cmd_call(SMPC_SMC_CKCHG352, SMPC_CMD_ISSUE_TYPE_A, NULL);

        return MEM_READ(OREG(31));
}
