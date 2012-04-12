/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_ckchg320_call(void)
{
        /* ??? */
        smpc_cmd_call(0x0F);

        return MEM_READ(OREG(31));
}
