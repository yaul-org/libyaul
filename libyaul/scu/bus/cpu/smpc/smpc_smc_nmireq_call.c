/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "smpc-internal.h"

uint8_t
smpc_smc_nmireq_call(void)
{
        /* Send "NMI" request to master "SH-2" */
        smpc_smc_call(SMPC_SMC_NMIREQ, SMPC_CMD_ISSUE_TYPE_A, NULL);

        return MEMORY_READ(8, OREG(31));
}
