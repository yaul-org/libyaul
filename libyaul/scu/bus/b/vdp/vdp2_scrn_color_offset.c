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
vdp2_scrn_color_offset_set(vdp2_scrn_t scroll_screen, vdp2_scrn_color_offset_t select)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1) ||
               (scroll_screen == VDP2_SCRN_NBG2) ||
               (scroll_screen == VDP2_SCRN_NBG3) ||
               (scroll_screen == VDP2_SCRN_RBG0) ||
               (scroll_screen == VDP2_SCRN_BACK) ||
               (scroll_screen == VDP2_SCRN_SPRITE));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs->clofen &= 0xFFFE;
                _state_vdp2()->regs->clofen |= 0x0001;

                _state_vdp2()->regs->clofsl &= 0xFFFE;
                _state_vdp2()->regs->clofsl |= select << 0;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->clofen &= 0xFFFD;
                _state_vdp2()->regs->clofen |= 0x0002;

                _state_vdp2()->regs->clofsl &= 0xFFFD;
                _state_vdp2()->regs->clofsl |= select << 1;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->regs->clofen &= 0xFFFB;
                _state_vdp2()->regs->clofen |= 0x0004;

                _state_vdp2()->regs->clofsl &= 0xFFFB;
                _state_vdp2()->regs->clofsl |= select << 2;
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->regs->clofen &= 0xFFF7;
                _state_vdp2()->regs->clofen |= 0x0008;

                _state_vdp2()->regs->clofsl &= 0xFFF7;
                _state_vdp2()->regs->clofsl |= select << 3;
                break;
        case VDP2_SCRN_RBG0:
                _state_vdp2()->regs->clofen &= 0xFFEF;
                _state_vdp2()->regs->clofen |= 0x0010;

                _state_vdp2()->regs->clofsl &= 0xFFEF;
                _state_vdp2()->regs->clofsl |= select << 4;
                break;
        case VDP2_SCRN_BACK:
                _state_vdp2()->regs->clofen &= 0xFFDF;
                _state_vdp2()->regs->clofen |= 0x0020;

                _state_vdp2()->regs->clofsl &= 0xFFDF;
                _state_vdp2()->regs->clofsl |= select << 5;
                break;
        case VDP2_SCRN_SPRITE:
                _state_vdp2()->regs->clofen &= 0xFFBF;
                _state_vdp2()->regs->clofen |= 0x0040;

                _state_vdp2()->regs->clofsl &= 0xFFBF;
                _state_vdp2()->regs->clofsl |= select << 6;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_color_offset_unset(vdp2_scrn_t scroll_screen)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1) ||
               (scroll_screen == VDP2_SCRN_NBG2) ||
               (scroll_screen == VDP2_SCRN_NBG3) ||
               (scroll_screen == VDP2_SCRN_RBG0) ||
               (scroll_screen == VDP2_SCRN_BACK) ||
               (scroll_screen == VDP2_SCRN_SPRITE));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs->clofen &= 0xFFFE;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->clofen &= 0xFFFD;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->regs->clofen &= 0xFFFB;
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->regs->clofen &= 0xFFF7;
                break;
        case VDP2_SCRN_RBG0:
                _state_vdp2()->regs->clofen &= 0xFFEF;
                break;
        case VDP2_SCRN_BACK:
                _state_vdp2()->regs->clofen &= 0xFFDF;
                break;
        case VDP2_SCRN_SPRITE:
                _state_vdp2()->regs->clofen &= 0xFFBF;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_color_offset_clear(void)
{
        _state_vdp2()->regs->clofen = 0x0000;
        _state_vdp2()->regs->clofsl = 0x0000;
}

void
vdp2_scrn_color_offset_rgb_set(vdp2_scrn_color_offset_t select,
        const vdp2_scrn_color_offset_rgb_t * const rgb)
{
#ifdef DEBUG
        /* Check range: [-256,256] */
        assert(((rgb->r >= -256) && (rgb->r <= 255)) &&
               ((rgb->g >= -256) && (rgb->g <= 255)) &&
               ((rgb->b >= -256) && (rgb->b <= 255)));
#endif /* DEBUG */

        /* Each color component in the color offset has a range [-256,255] Lower
         * bound: -(2**(9-1)) & upper bound: (2**9)-1 */
        switch (select) {
        case VDP2_SCRN_COLOR_OFFSET_A:
                _state_vdp2()->regs->coar = rgb->r & 0x01FF;
                _state_vdp2()->regs->coag = rgb->g & 0x01FF;
                _state_vdp2()->regs->coab = rgb->b & 0x01FF;
                break;
        case VDP2_SCRN_COLOR_OFFSET_B:
                _state_vdp2()->regs->cobr = rgb->r & 0x01FF;
                _state_vdp2()->regs->cobg = rgb->g & 0x01FF;
                _state_vdp2()->regs->cobb = rgb->b & 0x01FF;
                break;
        default:
                break;
        }
}
