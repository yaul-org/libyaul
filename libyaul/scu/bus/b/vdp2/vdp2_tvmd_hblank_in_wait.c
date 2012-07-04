/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

void
vdp2_tvmd_hblank_in_wait(void)
{

        /* Spin if we're in HBLANK-OUT (scan). Wait for HBLANK-IN
         * (retrace) */
        for (; (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0004) == 0x0000; );
}
