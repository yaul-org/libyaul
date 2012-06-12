/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp1.h>

#include "vdp1_internal.h"

void
vdp1_init(void)
{
        /* Initialize the processor to sane values. */
        MEM_POKE(VDP1(TVMR), 0x0000);
        MEM_POKE(VDP1(ENDR), 0x0000);
        MEM_POKE(VDP1(FBCR), 0x0000);
        MEM_POKE(VDP1(EWLR), 0x0000);
        MEM_POKE(VDP1(EWRR), 0x50df);
        MEM_POKE(VDP1(EWDR), 0x8000);
        MEM_POKE(VDP1(PTMR), 0x0002);

        /* Stop processing command tables. */
        MEM_POKE(CMD_TABLE(0, 0), 0x8000);
}
