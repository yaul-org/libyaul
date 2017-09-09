/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "smpc-internal.h"

#include <sys/cdefs.h>

#include <stdbool.h>

void __noreturn
smpc_smc_sysres_call(void)
{
        /* Completely soft reboot the entire system */
        smpc_smc_call(SMPC_SMC_SYSRES, SMPC_CMD_ISSUE_TYPE_A, NULL);

        while (true) {
        }
}
