/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/tvmd.h>

#include "vdp-internal.h"

void
vdp2_tvmd_display_clear(void)
{
        _state_vdp2()->regs.tvmd &= 0x7EFF;

        /* Change the DISP bit during VBLANK */
        vdp2_tvmd_vblank_in_wait();

        MEMORY_WRITE(16, VDP2(TVMD), _state_vdp2()->regs.tvmd);
}
