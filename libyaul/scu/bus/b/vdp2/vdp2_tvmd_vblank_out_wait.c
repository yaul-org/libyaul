/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

void
vdp2_tvmd_vblank_out_wait(void)
{

        /* Spin if we're in VBLANK-IN (retrace). Wait for VBLANK-OUT
         * (scan) */
        for (; (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0008) == 0x0008; );
}
