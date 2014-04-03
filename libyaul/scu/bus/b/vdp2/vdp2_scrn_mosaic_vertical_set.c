/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_mosaic_vertical_set(uint32_t vertical)
{

        vdp2_regs.mzctl &= 0xF0FF;
        vdp2_regs.mzctl |= (vertical & 0xF) << 8;

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(MZCTL), vdp2_regs.mzctl);
}
