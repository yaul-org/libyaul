/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cd-block.h>
#include <smpc/smc.h>

#include "cd-block-internal.h"

void
cd_block_init(void)
{

        smpc_smc_cdoff_call();
        smpc_smc_cdon_call();

        cd_block_cmd_abort_file();
        cd_block_cmd_init_cd_system();
}
