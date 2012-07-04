/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp1/fbcr.h>

#include "vdp1-internal.h"

void
vdp1_fbcr_erase_mode_set(enum fbcr_erase_mode_type mode)
{
        uint16_t tvmr;

        tvmr = MEMORY_READ(16, VDP1(MODR));

        /* Allow frame buffer to be erased during V-BLANK. */
        MEMORY_WRITE(16, VDP1(TVMR), (tvmr & 0x7) | ((tvmr & 0x8) ^ mode));
}
