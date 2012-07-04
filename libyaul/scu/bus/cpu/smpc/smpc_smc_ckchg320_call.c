/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "smpc-internal.h"

uint8_t
smpc_smc_ckchg320_call(void)
{
        /* Switch the SATURN system clock from 352 mode to 320 mode iff
         * not called from slave "SH-2" */
        smpc_smc_call(SMPC_SMC_CKCHG320, SMPC_CMD_ISSUE_TYPE_A, NULL);

        return MEMORY_READ(8, OREG(31));
}
