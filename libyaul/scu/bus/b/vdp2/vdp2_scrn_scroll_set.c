/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

void
vdp2_scrn_scroll_x_set(uint8_t scrn, fix16_t scroll)
{
        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                vdp2_state.scroll.nbg0.x = F16(0.0f);
                break;
        case SCRN_NBG1:
                vdp2_state.scroll.nbg1.x = F16(0.0f);
                break;
        case SCRN_NBG2:
                vdp2_state.scroll.nbg2.x = 0;
                break;
        case SCRN_NBG3:
                vdp2_state.scroll.nbg3.x = 0;
                break;
        default:
                return;
        }

        vdp2_scrn_scroll_x_update(scrn, scroll);
}

void
vdp2_scrn_scroll_y_set(uint8_t scrn, fix16_t scroll)
{
        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                vdp2_state.scroll.nbg0.y = F16(0.0f);
                break;
        case SCRN_NBG1:
                vdp2_state.scroll.nbg1.y = F16(0.0f);
                break;
        case SCRN_NBG2:
                vdp2_state.scroll.nbg2.y = 0;
                break;
        case SCRN_NBG3:
                vdp2_state.scroll.nbg3.y = 0;
                break;
        default:
                return;
        }

        vdp2_scrn_scroll_x_update(scrn, scroll);
}
