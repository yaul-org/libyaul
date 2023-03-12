/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_mosaic_set(vdp2_scrn_t scrn_mask)
{
    /* If performing mosaic processing in NBG0 or NBG1, the vertical
     * cell-scroll function cannot be used.
     *
     * Mosaic processing is then ignored for NBG0 or NBG1 */
    if ((scrn_mask & (VDP2_SCRN_RBG0_PA | VDP2_SCRN_RBG0_PB)) != VDP2_SCRN_NONE) {
        scrn_mask &= ~(VDP2_SCRN_RBG0_PA | VDP2_SCRN_RBG0_PB);
        scrn_mask |= VDP2_SCRN_RBG0;
    }

    _state_vdp2()->shadow_regs.mzctl &= 0xFFE0;
    _state_vdp2()->shadow_regs.mzctl |= scrn_mask;
}

void
vdp2_scrn_mosaic_vertical_set(uint8_t vertical)
{
    vertical = clamp(vertical, 1, 16);

    _state_vdp2()->shadow_regs.mzctl &= 0x0FFF;
    _state_vdp2()->shadow_regs.mzctl |= (vertical - 1) << 12;
}

void
vdp2_scrn_mosaic_horizontal_set(uint8_t horizontal)
{
    horizontal = clamp(horizontal, 1, 16);

    _state_vdp2()->shadow_regs.mzctl &= 0xF0FF;
    _state_vdp2()->shadow_regs.mzctl |= (horizontal - 1) << 8;
}
