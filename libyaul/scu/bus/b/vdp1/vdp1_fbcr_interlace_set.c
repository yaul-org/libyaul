/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp1/fbcr.h>

#include "vdp1-internal.h"

void
vdp1_fbcr_interlace_set(enum fbcr_interlace_type mode)
{
        uint16_t modr;

        /* Only NTSC and PAL format is able to have interlace mode. */
        modr = MEMORY_READ(16, VDP1(MODR));
        if ((modr & 0x0006) == 0x0000)
                MEMORY_WRITE(16, VDP1(FBCR), mode);
}
