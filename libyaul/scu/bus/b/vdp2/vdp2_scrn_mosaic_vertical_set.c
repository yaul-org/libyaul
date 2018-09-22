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

        _internal_state_vdp2.buffered_regs.mzctl &= 0x0FFF;
        _internal_state_vdp2.buffered_regs.mzctl |= (vertical - 1) << 12;
}
