/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp1/fbcr.h>

#include "vdp1_internal.h"

void
vdp1_fbcr_erase_coordinates_set(unsigned short x1, unsigned short y1, unsigned short x3, unsigned short y3, unsigned short color)
{
        unsigned short bpp;

        /* Obtain the bit depth of the frame buffer. */
        bpp = (MEM_READ(VDP1(MODR)) & 0x1) ? 4 : 3;

        /* Upper-left coordinates. */
        MEM_POKE(VDP1(EWLR), ((x1 >> bpp) << 9) | (y1 - 1));

        /* Lower-right coordinates. */
        MEM_POKE(VDP1(EWLR), ((x3 >> bpp) << 9) | (y3 - 1));

        MEM_POKE(VDP1(EWDR), color);
}
