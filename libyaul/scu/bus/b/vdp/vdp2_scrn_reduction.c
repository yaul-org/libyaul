/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp-internal.h"

void
vdp2_scrn_reduction_set(vdp2_scrn_t scroll_screen, vdp2_scrn_reduction_t reduction)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1));

        assert((reduction == VDP2_SCRN_REDUCTION_NONE) ||
               (reduction == VDP2_SCRN_REDUCTION_HALF) ||
               (reduction == VDP2_SCRN_REDUCTION_QUARTER));
#endif /* DEBUG */

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
vdp2_scrn_reduction_x_set(vdp2_scrn_t scroll_screen, q0_3_8_t scale)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs->zmxin0 = Q0_3_8_INT(scale);
                _state_vdp2()->regs->zmxdn0 = Q0_3_8_FRAC(scale) << 8;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->zmxin1 = Q0_3_8_INT(scale);
                _state_vdp2()->regs->zmxdn1 = Q0_3_8_FRAC(scale) << 8;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_reduction_y_set(vdp2_scrn_t scroll_screen, q0_3_8_t scale)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs->zmyin0 = Q0_3_8_INT(scale);
                _state_vdp2()->regs->zmydn0 = Q0_3_8_FRAC(scale) << 8;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->zmyin1 = Q0_3_8_INT(scale);
                _state_vdp2()->regs->zmydn1 = Q0_3_8_FRAC(scale) << 8;
                break;
        default:
                break;
        }
}
