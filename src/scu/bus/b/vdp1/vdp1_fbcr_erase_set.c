/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp1/fbcr.h>

#include "vdp1_internal.h"

void
vdp1_fbcr_erase_set(void)
{
        unsigned short fbcr;

        fbcr = (MEM_READ(VDP1(MODR)) & 0xe0) >> 3;

        /* Write to the FBCR register after VBLANK-OUT interrupt. */
        MEM_POKE(VDP1(FBCR), fbcr | 0x0002);
}
