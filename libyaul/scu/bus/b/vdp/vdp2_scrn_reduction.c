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
                _state_vdp2()->regs->zmctl &= 0xFFFC;
                _state_vdp2()->regs->zmctl |= reduction;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->zmctl &= 0xFCFF;
                _state_vdp2()->regs->zmctl |= reduction << 8;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_reduction_x_set(vdp2_scrn_t scroll_screen, fix16_t scale)
{
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1));

        fix16_vec2_t *zm_reg;

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                zm_reg = (fix16_vec2_t *)&_state_vdp2()->regs->zmxin0;
                break;
        case VDP2_SCRN_NBG1:
                zm_reg = (fix16_vec2_t *)&_state_vdp2()->regs->zmxin1;
                break;
        default:
                return;
        }

        zm_reg->x = scale;
}

void
vdp2_scrn_reduction_y_set(vdp2_scrn_t scroll_screen, fix16_t scale)
{
        fix16_vec2_t *zm_reg;

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                zm_reg = (fix16_vec2_t *)&_state_vdp2()->regs->zmxin0;
                break;
        case VDP2_SCRN_NBG1:
                zm_reg = (fix16_vec2_t *)&_state_vdp2()->regs->zmxin1;
                break;
        default:
                return;
        }

        zm_reg->y = scale;
}
