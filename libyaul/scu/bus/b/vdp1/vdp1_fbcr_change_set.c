/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp1/fbcr.h>

#include "vdp1-internal.h"

void
vdp1_fbcr_change_set(void)
{
        uint16_t fbcr;
        uint16_t modr;

        modr = MEMORY_READ(16, VDP1(MODR));
        fbcr = modr & 0x00E0;
        fbcr >>= 3;

        /* Write to the FBCR register after VBLANK-OUT interrupt */
        fbcr |= 0x0008;
        MEMORY_WRITE(16, VDP1(FBCR), fbcr);
}
