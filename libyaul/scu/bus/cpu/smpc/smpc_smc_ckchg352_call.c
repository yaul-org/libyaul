/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "smpc-internal.h"

uint8_t
smpc_smc_ckchg352_call(void)
{
        /* Switch the SATURN system clock from 320 mode to 352 mode iff
         * not called from slave "SH-2" */
        smpc_smc_call(SMPC_SMC_CKCHG352, SMPC_CMD_ISSUE_TYPE_A, NULL);

        return MEMORY_READ(8, OREG(31));
}
