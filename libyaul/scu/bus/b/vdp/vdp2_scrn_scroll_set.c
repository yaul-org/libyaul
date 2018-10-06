/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include <assert.h>

#include "vdp-internal.h"

/*-
 * Integer scrolling for NGB2 and NBG3
 * +------------------+-------+ +------------------+-------+
 * | X integer scroll | Value | | Y integer scroll | Value |
 * +------------------+-------+ +------------------+-------+
 * | -2048            |  0    | | -2048            |       |
 * | -2047            |  2047 | | -2047            |       |
 * |  ...             |  ...  | |  ...             |       |
 * | -2               |  1    | | -2               |       |
 * | -1               |  0    | | -1               |       |
 * |                  |       | |                  |       |
 * |  0               |  0    | |  0               |       |
 * |                  |       | |                  |       |
 * |  1               |  2047 | |  1               |       |
 * |  2               |  2046 | |  2               |       |
 * |  ...             |  ...  | |  ...             |       |
 * |  2047            |  1    | |  2047            |       |
 * |  2048            |  0    | |  2048            |       |
 * +------------------+-------+ +------------------+-------+
 */

#define WRAP_Y_INTEGER(x)                                                      \
    (((x) < 0)                                                                 \
        ? ((((x) & 0x07FF) + 0x07FF) & 0x07FF)                                 \
        : ((x) & 0x07FF))

#define WRAP_X_INTEGER(x)                                                      \
    ((0x07FF - (x) + 1) & 0x07FF)

static inline void _set_fixed_point_scroll(fix16_t *, fix16_t, uint16_t *,
    uint16_t *);
static inline void _set_integer_x_scroll(int16_t *, fix16_t, uint16_t *);
static inline void _set_integer_y_scroll(int16_t *, fix16_t, uint16_t *);

void
vdp2_scrn_scroll_x_set(uint8_t scrn, fix16_t scroll)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_RBG1) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG1));
#endif /* DEBUG */

        /* All screen scroll values must be identified as positive
         * values */
        uint16_t in;
        uint16_t dn;

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                _set_fixed_point_scroll(&_state_vdp2()->nbg0.scroll.x, scroll, &in, &dn);

                _state_vdp2()->regs.scxin0 = in;
                _state_vdp2()->regs.scxdn0 = dn;
                break;
        case SCRN_NBG1:
                _set_fixed_point_scroll(&_state_vdp2()->nbg1.scroll.x, scroll, &in, &dn);

                _state_vdp2()->regs.scxin1 = in;
                _state_vdp2()->regs.scxdn1 = dn;
                break;
        case SCRN_NBG2:
                _set_integer_x_scroll(&_state_vdp2()->nbg2.scroll.x, scroll, &in);

                _state_vdp2()->regs.scxn2 = in;
                break;
        case SCRN_NBG3:
                _set_integer_x_scroll(&_state_vdp2()->nbg3.scroll.x, scroll, &in);

                _state_vdp2()->regs.scxn3 = in;
                break;
        default:
                return;
        }
}

void
vdp2_scrn_scroll_y_set(uint8_t scrn, fix16_t scroll)
{
#ifdef DEBUG
        /* Check if the background passed is valid */
        assert((scrn == SCRN_NBG0) ||
               (scrn == SCRN_RBG1) ||
               (scrn == SCRN_NBG1) ||
               (scrn == SCRN_NBG2) ||
               (scrn == SCRN_NBG3) ||
               (scrn == SCRN_RBG1));
#endif /* DEBUG */

        /* All screen scroll values must be identified as positive
         * values */
        uint16_t in;
        uint16_t dn;

        switch (scrn) {
        case SCRN_RBG1:
        case SCRN_NBG0:
                _set_fixed_point_scroll(&_state_vdp2()->nbg0.scroll.y, scroll, &in, &dn);

                _state_vdp2()->regs.scyin0 = in;
                _state_vdp2()->regs.scydn0 = dn;
                break;
        case SCRN_NBG1:
                _set_fixed_point_scroll(&_state_vdp2()->nbg1.scroll.y, scroll, &in, &dn);

                _state_vdp2()->regs.scyin1 = in;
                _state_vdp2()->regs.scydn1 = dn;
                break;
        case SCRN_NBG2:
                _set_integer_y_scroll(&_state_vdp2()->nbg2.scroll.y, scroll, &in);

                _state_vdp2()->regs.scyn2 = in;
                break;
        case SCRN_NBG3:
                _set_integer_y_scroll(&_state_vdp2()->nbg3.scroll.y, scroll, &in);

                _state_vdp2()->regs.scyn3 = in;
                break;
        default:
                return;
        }
}

static inline void
_set_fixed_point_scroll(fix16_t *scroll, fix16_t amount, uint16_t *in,
    uint16_t *dn)
{
        int32_t integral;
        integral = fix16_to_int(amount);

        fix16_t fractional;
        fractional = fix16_fractional(amount);

        *in = integral;
        *dn = fractional & 0xFF00;
        *scroll = fix16_add(fix16_from_int(*in), fractional);
}

static inline void
_set_integer_x_scroll(int16_t *scroll, fix16_t amount, uint16_t *in)
{
        *scroll = WRAP_X_INTEGER((int16_t)fix16_to_int(amount));
        *in = *scroll;
}

static inline void
_set_integer_y_scroll(int16_t *scroll, fix16_t amount, uint16_t *in)
{
        *scroll = WRAP_Y_INTEGER((int16_t)fix16_to_int(amount));
        *in = *scroll;
}
