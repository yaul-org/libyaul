/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_mosaic_clear(void)
{

        vdp2_regs.mzctl &= 0xFFE0;

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(MZCTL), vdp2_regs.mzctl);
}
