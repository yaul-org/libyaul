/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <vdp2/tvmd.h>

#include "vdp2_internal.h"

void
vdp2_tvmd_vblank_in_wait(void)
{

        /* Spin if we're in VBLANK-OUT (scan). Wait for VBLANK-IN
         * (retrace) */
        for (; (MEM_READ(VDP2(TVSTAT)) & 0x0008) == 0; );
}
