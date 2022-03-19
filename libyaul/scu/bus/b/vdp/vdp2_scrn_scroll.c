/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>
#include <stdint.h>

#include "vdp-internal.h"

static inline __always_inline void
_fixed_point_scroll_set(uint16_t *scroll, fix16_t amount)
{
        uint32_t * const reg = (uint32_t *)scroll;

        *reg = amount;
}

static inline __always_inline void
_fixed_point_scroll_update(uint16_t *scroll, fix16_t amount)
{
        uint32_t * const reg = (uint32_t *)scroll;

        *reg += amount;
}

static inline __always_inline void
_integer_scroll_set(uint16_t *scroll, fix16_t amount)
{
        *scroll = (uint16_t)fix16_int32_to(amount);
}

static inline __always_inline void
_integer_scroll_update(uint16_t *scroll, fix16_t amount)
{
        *scroll = (uint16_t)fix16_int32_to(amount);
}

void
vdp2_scrn_scroll_x_set(vdp2_scrn_t scroll_screen, fix16_t scroll)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1) ||
               (scroll_screen == VDP2_SCRN_NBG2) ||
               (scroll_screen == VDP2_SCRN_NBG3));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _fixed_point_scroll_set(&_state_vdp2()->regs->scxin0, scroll);
                break;
        case VDP2_SCRN_NBG1:
                _fixed_point_scroll_set(&_state_vdp2()->regs->scxin1, scroll);
                break;
        case VDP2_SCRN_NBG2:
                _integer_scroll_set(&_state_vdp2()->regs->scxn2, scroll);
                break;
        case VDP2_SCRN_NBG3:
                _integer_scroll_set(&_state_vdp2()->regs->scxn3, scroll);
                break;
        default:
                break;
        }
}

void
vdp2_scrn_scroll_y_set(vdp2_scrn_t scroll_screen, fix16_t scroll)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1) ||
               (scroll_screen == VDP2_SCRN_NBG2) ||
               (scroll_screen == VDP2_SCRN_NBG3));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _fixed_point_scroll_set(&_state_vdp2()->regs->scyin0, scroll);
                break;
        case VDP2_SCRN_NBG1:
                _fixed_point_scroll_set(&_state_vdp2()->regs->scyin1, scroll);
                break;
        case VDP2_SCRN_NBG2:
                _integer_scroll_set(&_state_vdp2()->regs->scyn2, scroll);
                break;
        case VDP2_SCRN_NBG3:
                _integer_scroll_set(&_state_vdp2()->regs->scyn3, scroll);
                break;
        default:
                break;
        }
}

void
vdp2_scrn_scroll_x_update(vdp2_scrn_t scroll_screen, fix16_t delta)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1) ||
               (scroll_screen == VDP2_SCRN_NBG2) ||
               (scroll_screen == VDP2_SCRN_NBG3));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _fixed_point_scroll_update(&_state_vdp2()->regs->scxin0, delta);
                break;
        case VDP2_SCRN_NBG1:
                _fixed_point_scroll_update(&_state_vdp2()->regs->scxin1, delta);
                break;
        case VDP2_SCRN_NBG2:
                _integer_scroll_update(&_state_vdp2()->regs->scxn2, delta);
                break;
        case VDP2_SCRN_NBG3:
                _integer_scroll_update(&_state_vdp2()->regs->scxn3, delta);
                break;
        default:
                break;
        }        
}

void
vdp2_scrn_scroll_y_update(vdp2_scrn_t scroll_screen, fix16_t delta)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scroll_screen == VDP2_SCRN_NBG0) ||
               (scroll_screen == VDP2_SCRN_NBG1) ||
               (scroll_screen == VDP2_SCRN_NBG2) ||
               (scroll_screen == VDP2_SCRN_NBG3));
#endif /* DEBUG */

        switch (scroll_screen) {
        case VDP2_SCRN_NBG0:
                _fixed_point_scroll_update(&_state_vdp2()->regs->scyin0, delta);
                break;
        case VDP2_SCRN_NBG1:
                _fixed_point_scroll_update(&_state_vdp2()->regs->scyin1, delta);
                break;
        case VDP2_SCRN_NBG2:
                _integer_scroll_update(&_state_vdp2()->regs->scyn2, delta);
                break;
        case VDP2_SCRN_NBG3:
                _integer_scroll_update(&_state_vdp2()->regs->scyn3, delta);
                break;
        default:
                break;
        }        
}
