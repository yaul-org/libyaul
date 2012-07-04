/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp1/fbcr.h>

#include "vdp1-internal.h"

void
vdp1_fbcr_bpp_set(uint8_t bpp)
{
        uint16_t tvmr;

        tvmr = MEMORY_READ(16, VDP1(MODR));

        /* Write to memory. */
        MEMORY_WRITE(16, VDP1(TVMR), (tvmr & 0x7) | ((bpp == 8) ? 1 : 0));
}
