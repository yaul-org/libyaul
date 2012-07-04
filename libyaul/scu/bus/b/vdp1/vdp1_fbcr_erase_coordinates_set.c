/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp1/fbcr.h>

#include "vdp1-internal.h"

void
vdp1_fbcr_erase_coordinates_set(uint16_t x1, uint16_t y1, uint16_t x3, uint16_t y3, uint16_t color)
{
        uint16_t bpp;
        uint16_t modr;

        /* Obtain the bit depth of the frame buffer. */
        modr = MEMORY_READ(16, VDP1(MODR));
        bpp = ((modr & 0x0001) == 0x0001) ? 4 : 3;

        /* Upper-left coordinates. */
        MEMORY_WRITE(16, VDP1(EWLR), ((x1 >> bpp) << 9) | (y1 - 1));

        /* Lower-right coordinates. */
        MEMORY_WRITE(16, VDP1(EWLR), ((x3 >> bpp) << 9) | (y3 - 1));

        MEMORY_WRITE(16, VDP1(EWDR), color);
}
