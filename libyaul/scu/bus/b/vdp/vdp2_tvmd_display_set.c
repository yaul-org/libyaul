/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu.h>
#include <vdp2/tvmd.h>

#include "vdp-internal.h"

void
vdp2_tvmd_display_set(void)
{
        _state_vdp2()->regs.tvmd |= 0x8000;

        /* Change the DISP bit during VBLANK-IN */
        vdp2_tvmd_vblank_in_wait();

        MEMORY_WRITE(16, VDP2(TVMD), _state_vdp2()->regs.tvmd);
}
