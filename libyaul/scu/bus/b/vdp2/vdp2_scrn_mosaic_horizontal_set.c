/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_mosaic_horizontal_set(uint32_t horizontal)
{

        if ((horizontal < 1) || (horizontal > 16)) {
                return;
        }

        vdp2_state.buffered_regs.mzctl &= 0xF0FF;
        vdp2_state.buffered_regs.mzctl |= (horizontal - 1) << 8;

        /* Write to memory */
        MEMORY_WRITE(16, VDP2(MZCTL), vdp2_state.buffered_regs.mzctl);
}
