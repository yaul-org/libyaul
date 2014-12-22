/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>

#include "vdp2-internal.h"

void
vdp2_tvmd_display_clear(void)
{
        uint16_t tvmd;

        tvmd = MEMORY_READ(16, VDP2(TVMD));
        tvmd &= 0x7FFF;

        /* Change the DISP bit during VBLANK */
        vdp2_tvmd_vblank_in_wait();

        MEMORY_WRITE(16, VDP2(TVMD), tvmd);
}
