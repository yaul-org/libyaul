/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

void
vdp2_tvmd_vblank_in_wait(void)
{

        uint16_t exten;

        /* Latch the vertical and horizontal count register */
        exten = MEMORY_READ(16, VDP2(EXTEN));
        exten &= 0xFDFF;
        MEMORY_WRITE(16, VDP2(EXTEN), exten);

        /* Spin if we're in VBLANK-OUT (re-trace/blank). Wait for VBLANK-IN
         * (scan/draw) */
        for (; (MEMORY_READ(16, VDP2(TVSTAT)) & 0x0008) == 0x0000; );
}
