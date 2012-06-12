/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp1/fbcr.h>

#include "vdp1_internal.h"

void
vdp1_fbcr_erase_mode_set(enum fbcr_erase_mode_type mode)
{
        uint16_t tvmr;

        tvmr = MEM_READ(VDP1(MODR));

        /* Allow frame buffer to be erased during V-BLANK. */
        MEM_POKE(VDP1(TVMR), (tvmr & 0x7) | ((tvmr & 0x8) ^ mode));
}
