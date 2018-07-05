/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_mosaic_vertical_set(uint32_t vertical)
{
        if ((vertical < 1) || (vertical > 16)) {
                return;
        }

        vdp2_state.buffered_regs.mzctl &= 0x0FFF;
        vdp2_state.buffered_regs.mzctl |= (vertical - 1) << 12;
}
