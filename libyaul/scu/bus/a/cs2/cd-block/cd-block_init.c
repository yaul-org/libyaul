/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
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
