/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/vram.h>

#include "vdp2_internal.h"

void
vdp2_vram_cycle_pattern_set(uint32_t *timers)
{

        MEM_POKE(VDP2(CYCA0L), timers[0] & 0xFFFF);
        MEM_POKE(VDP2(CYCA0U), (timers[0] >> 16) & 0xFFFF);

        MEM_POKE(VDP2(CYCA1L), timers[1] & 0xFFFF);
        MEM_POKE(VDP2(CYCA1U), (timers[1] >> 16) & 0xFFFF);

        MEM_POKE(VDP2(CYCB0L), timers[2] & 0xFFFF);
        MEM_POKE(VDP2(CYCB0U), (timers[2] >> 16) & 0xFFFF);

        MEM_POKE(VDP2(CYCB1L), timers[3] & 0xffff);
        MEM_POKE(VDP2(CYCB1U), (timers[3] >> 16) & 0xFFFF);
}
