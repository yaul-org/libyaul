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
cd_block_cmd_abort_file(void)
{
        struct cd_block_regs regs;
        struct cd_block_regs status;

        /* Abort file */
        regs.hirq_mask = 0x0200;
        regs.cr1 = 0x7500;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((cd_block_cmd_execute(&regs, &status)) < 0)
                return;
}
