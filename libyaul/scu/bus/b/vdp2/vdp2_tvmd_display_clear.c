/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2_internal.h"

void
vdp2_tvmd_display_clear(void)
{
        uint16_t tvmd;

        tvmd = MEM_READ(VDP2(TVMD));
        tvmd &= 0x7FFF;

        MEM_POKE(VDP2(TVMD), tvmd);
}
