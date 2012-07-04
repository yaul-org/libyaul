/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "smpc-internal.h"

/* Command type: Non-resetable system management commands */
uint8_t
smpc_smc_setsmem_call(void)
{

        smpc_smc_call(SMPC_SMC_SETSMEM, SMPC_CMD_ISSUE_TYPE_C, NULL);

        return MEMORY_READ(8, OREG(31));
}
