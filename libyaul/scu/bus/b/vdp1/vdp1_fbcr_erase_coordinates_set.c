/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp1/fbcr.h>

#include "vdp1_internal.h"

void
vdp1_fbcr_erase_coordinates_set(uint16_t x1, uint16_t y1, uint16_t x3, uint16_t y3, uint16_t color)
{
        uint16_t bpp;

        /* Obtain the bit depth of the frame buffer. */
        bpp = (MEM_READ(VDP1(MODR)) & 0x1) ? 4 : 3;

        /* Upper-left coordinates. */
        MEM_POKE(VDP1(EWLR), ((x1 >> bpp) << 9) | (y1 - 1));

        /* Lower-right coordinates. */
        MEM_POKE(VDP1(EWLR), ((x3 >> bpp) << 9) | (y3 - 1));

        MEM_POKE(VDP1(EWDR), color);
}
