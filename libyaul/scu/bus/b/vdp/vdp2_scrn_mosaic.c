/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp-internal.h"

void
vdp2_scrn_mosaic_set(vdp2_scrn_t scroll_screen)
{
#ifdef DEBUG
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1) ||
               (scroll_screen == VDP2_SCRN_NBG2) ||
               (scroll_screen == VDP2_SCRN_NBG3) ||
               (scroll_screen == VDP2_SCRN_RBG0));
#endif /* DEBUG */

        /* If performing mosaic processing in NBG0 or NBG1, the vertical
         * cell-scroll function cannot be used.
         *
         * Mosaic processing is then ignored for NBG0 or NBG1. */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
#ifdef DEBUG
                /* Make sure that the vertical cell scroll function is
                 * not being used */
                assert((_state_vdp2()->regs->scrctl & 0x0001) == 0x0000);
#endif /* DEBUG */

                _state_vdp2()->regs->mzctl &= 0xFFFE;
                _state_vdp2()->regs->mzctl |= 0x0001;
                break;
        case VDP2_SCRN_NBG1:
#ifdef DEBUG
                /* Make sure that the vertical cell scroll function is
                 * not being used */
                assert((_state_vdp2()->regs->scrctl & 0x0100) == 0x0000);
#endif /* DEBUG */

                _state_vdp2()->regs->mzctl &= 0xFFFD;
                _state_vdp2()->regs->mzctl |= 0x0002;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->regs->mzctl &= 0xFFFB;
                _state_vdp2()->regs->mzctl |= 0x0004;
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->regs->mzctl &= 0xFFF7;
                _state_vdp2()->regs->mzctl |= 0x0008;
                break;
        case VDP2_SCRN_RBG0:
                _state_vdp2()->regs->mzctl &= 0xFFEF;
                _state_vdp2()->regs->mzctl |= 0x0010;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_mosaic_unset(vdp2_scrn_t scroll_screen)
{
#ifdef DEBUG
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1) ||
               (scroll_screen == VDP2_SCRN_NBG2) ||
               (scroll_screen == VDP2_SCRN_NBG3) ||
               (scroll_screen == VDP2_SCRN_RBG0));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->regs->mzctl &= 0xFFFE;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->regs->mzctl &= 0xFFFD;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->regs->mzctl &= 0xFFFB;
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->regs->mzctl &= 0xFFF7;
                break;
        case VDP2_SCRN_RBG0:
                _state_vdp2()->regs->mzctl &= 0xFFEF;
                break;
        default:
                break;
        }
}

void
vdp2_scrn_mosaic_clear(void)
{
        _state_vdp2()->regs->mzctl &= 0xFFE0;
}

void
vdp2_scrn_mosaic_vertical_set(uint8_t vertical)
{
        if ((vertical < 1) || (vertical > 16)) {
                return;
        }

        _state_vdp2()->regs->mzctl &= 0x0FFF;
        _state_vdp2()->regs->mzctl |= (vertical - 1) << 12;
}

void
vdp2_scrn_mosaic_horizontal_set(uint8_t horizontal)
{
        if ((horizontal < 1) || (horizontal > 16)) {
                return;
        }

        _state_vdp2()->regs->mzctl &= 0xF0FF;
        _state_vdp2()->regs->mzctl |= (horizontal - 1) << 8;
}
