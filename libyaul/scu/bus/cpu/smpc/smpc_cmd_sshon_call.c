/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_sshon_call(void)
{
        /* Enable the "SH-2" slave CPU. */
        smpc_cmd_call(0x02);

        return MEM_READ(OREG(31));
}
