/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_resenab_call(void)
{
        /* Allow the user to press the reset button. */
        smpc_cmd_call(0x19);

        return MEM_READ(OREG(31));
}
