/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp1/fbcr.h>

#include "vdp1_internal.h"

void
vdp1_fbcr_interlace_set(enum fbcr_interlace_type mode)
{

        /* Only NTSC and PAL format is able to have interlace mode. */
        if ((MEM_READ(VDP1(MODR)) & 0x0006) == 0x0000)
                MEM_POKE(VDP1(FBCR), mode);
}
