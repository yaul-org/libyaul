/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_intback_call(void)
{
        /* Retreive the SMPC status and peripheral data. */
        smpc_cmd_call(0x10);

        return MEM_READ(OREG(31));
}
