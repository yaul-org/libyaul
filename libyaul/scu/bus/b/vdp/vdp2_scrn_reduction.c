/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_reduction_set(vdp2_scrn_t scroll_screen, vdp2_scrn_reduction_t reduction)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.zmctl &= 0xFFFC;
                _state_vdp2()->shadow_regs.zmctl |= reduction;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.zmctl &= 0xFCFF;
                _state_vdp2()->shadow_regs.zmctl |= reduction << 8;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_reduction_x_set(vdp2_scrn_t scroll_screen, fix16_t scale)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.zm0.x = scale;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.zm1.x = scale;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_reduction_y_set(vdp2_scrn_t scroll_screen, fix16_t scale)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.zm0.y = scale;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.zm1.y = scale;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_reduction_xy_set(vdp2_scrn_t scroll_screen, const fix16_vec2_t *scale)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.zm0 = *scale;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.zm1 = *scale;
                break;
        default:
                break;
        }
}
