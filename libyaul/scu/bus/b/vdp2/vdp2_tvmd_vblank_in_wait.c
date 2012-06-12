/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2_internal.h"

void
vdp2_tvmd_vblank_in_wait(void)
{

        uint16_t exten;

        /* Latch the vertical and horizontal count register */
        exten = MEM_READ(VDP2(EXTEN));
        exten &= 0xFDFF;
        MEM_POKE(VDP2(EXTEN), exten);

        /* Spin if we're in VBLANK-OUT (scan). Wait for VBLANK-IN
         * (retrace) */
        for (; (MEM_READ(VDP2(TVSTAT)) & 0x0008) == 0; );
}
