/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "smpc_internal.h"

void
smpc_init(void)
{
        /* Set the "SMPC" control mode. */
        MEM_POKE(DDR1, 0x00);
        MEM_POKE(EXLE1, 0x00);
        MEM_POKE(IOSEL1, 0x00);
}
