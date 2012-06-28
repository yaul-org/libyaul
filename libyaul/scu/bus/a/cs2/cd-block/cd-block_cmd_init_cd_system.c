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
cd_block_cmd_init_cd_system(void)
{
        struct cd_block_regs regs;
        struct cd_block_regs status;


        /* Initialize CD system */
        regs.hirq_mask = 0x0000;
        regs.cr1 = 0x0400;
        regs.cr2 = 0x1000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x040F;

        if ((cd_block_cmd_execute(&regs, &status)) < 0)
                return;
}
