/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/smc.h>

#include "smpc-internal.h"

uint8_t
smpc_smc_cdon_call(void)
{
        /* Enable the SH-1 CD-block */
        smpc_smc_call(SMPC_SMC_CDON, SMPC_CMD_ISSUE_TYPE_B, NULL);

        return MEMORY_READ(8, OREG(31));
}
