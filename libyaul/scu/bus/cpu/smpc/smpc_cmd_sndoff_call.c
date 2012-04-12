/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

uint8_t
smpc_cmd_sndoff_call(void)
{
        /* Disable the "Motorola MC68EC000." */
        smpc_cmd_call(0x07);

        return MEM_READ(OREG(31));
}
