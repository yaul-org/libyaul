/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_mosaic_horizontal_set(uint32_t horizontal)
{

        vdp2_regs.mzctl &= 0x0FFF;
        vdp2_regs.mzctl |= (horizontal & 0xF) << 12;

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(MZCTL), vdp2_regs.mzctl);
}
