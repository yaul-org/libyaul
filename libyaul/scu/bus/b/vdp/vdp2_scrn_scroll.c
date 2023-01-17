/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <stdint.h>

#include "vdp-internal.h"

void
vdp2_scrn_scroll_x_set(vdp2_scrn_t scroll_screen, fix16_t scroll)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.sc0.x = scroll;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.sc1.x = scroll;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->shadow_regs.scn2.x = fix16_int32_to(scroll);
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->shadow_regs.scn3.x = fix16_int32_to(scroll);
                break;
        default:
                break;
        }
}

void
vdp2_scrn_scroll_y_set(vdp2_scrn_t scroll_screen, fix16_t scroll)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.sc0.y = scroll;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.sc1.y = scroll;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->shadow_regs.scn2.y = fix16_int32_to(scroll);
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->shadow_regs.scn3.y = fix16_int32_to(scroll);
                break;
        default:
                break;
        }
}

void
vdp2_scrn_scroll_x_update(vdp2_scrn_t scroll_screen, fix16_t delta)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.sc0.x += delta;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.sc1.x += delta;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->shadow_regs.scn2.x += fix16_int32_to(delta);
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->shadow_regs.scn3.x += fix16_int32_to(delta);
                break;
        default:
                break;
        }
}

void
vdp2_scrn_scroll_y_update(vdp2_scrn_t scroll_screen, fix16_t delta)
{
        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _state_vdp2()->shadow_regs.sc0.y += delta;
                break;
        case VDP2_SCRN_NBG1:
                _state_vdp2()->shadow_regs.sc1.y += delta;
                break;
        case VDP2_SCRN_NBG2:
                _state_vdp2()->shadow_regs.scn2.y += fix16_int32_to(delta);
                break;
        case VDP2_SCRN_NBG3:
                _state_vdp2()->shadow_regs.scn3.y += fix16_int32_to(delta);
                break;
        default:
                break;
        }
}
