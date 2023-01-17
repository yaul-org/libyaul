/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_coff_set(vdp2_scrn_t scroll_screen, vdp2_scrn_coff_t select)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.clofen &= 0xFFFE;
                _state_vdp2()->shadow_regs.clofen |= 0x0001;

                _state_vdp2()->shadow_regs.clofsl &= 0xFFFE;
                _state_vdp2()->shadow_regs.clofsl |= select;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.clofen &= 0xFFFD;
                _state_vdp2()->shadow_regs.clofen |= 0x0002;

                _state_vdp2()->shadow_regs.clofsl &= 0xFFFD;
                _state_vdp2()->shadow_regs.clofsl |= select << 1;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->shadow_regs.clofen &= 0xFFFB;
                _state_vdp2()->shadow_regs.clofen |= 0x0004;

                _state_vdp2()->shadow_regs.clofsl &= 0xFFFB;
                _state_vdp2()->shadow_regs.clofsl |= select << 2;
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->shadow_regs.clofen &= 0xFFF7;
                _state_vdp2()->shadow_regs.clofen |= 0x0008;

                _state_vdp2()->shadow_regs.clofsl &= 0xFFF7;
                _state_vdp2()->shadow_regs.clofsl |= select << 3;
                break;
        case VDP2_SCRN_RBG0:
        case VDP2_SCRN_RBG0_PA:
        case VDP2_SCRN_RBG0_PB:
                _state_vdp2()->shadow_regs.clofen &= 0xFFEF;
                _state_vdp2()->shadow_regs.clofen |= 0x0010;

                _state_vdp2()->shadow_regs.clofsl &= 0xFFEF;
                _state_vdp2()->shadow_regs.clofsl |= select << 4;
                break;
        case VDP2_SCRN_BACK:
                _state_vdp2()->shadow_regs.clofen &= 0xFFDF;
                _state_vdp2()->shadow_regs.clofen |= 0x0020;

                _state_vdp2()->shadow_regs.clofsl &= 0xFFDF;
                _state_vdp2()->shadow_regs.clofsl |= select << 5;
                break;
        case VDP2_SCRN_SPRITE:
                _state_vdp2()->shadow_regs.clofen &= 0xFFBF;
                _state_vdp2()->shadow_regs.clofen |= 0x0040;

                _state_vdp2()->shadow_regs.clofsl &= 0xFFBF;
                _state_vdp2()->shadow_regs.clofsl |= select << 6;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_coff_unset(vdp2_scrn_t scroll_screen)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.clofen &= 0xFFFE;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.clofen &= 0xFFFD;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->shadow_regs.clofen &= 0xFFFB;
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->shadow_regs.clofen &= 0xFFF7;
                break;
        case VDP2_SCRN_RBG0:
        case VDP2_SCRN_RBG0_PA:
        case VDP2_SCRN_RBG0_PB:
                _state_vdp2()->shadow_regs.clofen &= 0xFFEF;
                break;
        case VDP2_SCRN_BACK:
                _state_vdp2()->shadow_regs.clofen &= 0xFFDF;
                break;
        case VDP2_SCRN_SPRITE:
                _state_vdp2()->shadow_regs.clofen &= 0xFFBF;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_coff_clear(void)
{
        _state_vdp2()->shadow_regs.clofen = 0x0000;
        _state_vdp2()->shadow_regs.clofsl = 0x0000;
}

void
vdp2_scrn_coff_rgb_set(vdp2_scrn_coff_t select,
    const vdp2_scrn_coff_rgb_t *rgb)
{
        /* Range: [-256,256] */

        /* Each color component in the color offset has a range [-256,255] Lower
         * bound: -(2**(9-1)) & upper bound: (2**9)-1 */
        switch (select) {
        case VDP2_SCRN_COFF_A:
                _state_vdp2()->shadow_regs.coar = rgb->r & 0x01FF;
                _state_vdp2()->shadow_regs.coag = rgb->g & 0x01FF;
                _state_vdp2()->shadow_regs.coab = rgb->b & 0x01FF;
                break;
        case VDP2_SCRN_COFF_B:
                _state_vdp2()->shadow_regs.cobr = rgb->r & 0x01FF;
                _state_vdp2()->shadow_regs.cobg = rgb->g & 0x01FF;
                _state_vdp2()->shadow_regs.cobb = rgb->b & 0x01FF;
                break;
        default:
                break;
        }
}
