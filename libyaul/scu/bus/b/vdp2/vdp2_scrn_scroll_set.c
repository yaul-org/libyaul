/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2/scrn.h>

#include "vdp2-internal.h"

#define WRAP_INTEGER(x)                                                        \
    (((x) < 0)                                                                 \
        ? ((((x) & 0x07FF) + 0x07FF) & 0x07FF)                                 \
        : ((x) & 0x07FF))

static inline void _set_fixed_point_scroll(fix16_t *, fix16_t, uint16_t *,
    uint16_t *);
static inline void _set_integer_scroll(int16_t *, fix16_t, uint16_t *);

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
                _set_fixed_point_scroll(&vdp2_state.nbg0.scroll.x, scroll, &in,
                    &dn);

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCXIN0), in);
                MEMORY_WRITE(16, VDP2(SCXDN0), dn);
                break;
        case SCRN_NBG1:
                _set_fixed_point_scroll(&vdp2_state.nbg1.scroll.x, scroll, &in,
                    &dn);

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCXIN1), in);
                MEMORY_WRITE(16, VDP2(SCXDN1), dn);
                break;
        case SCRN_NBG2:
                _set_integer_scroll(&vdp2_state.nbg3.scroll.x, scroll, &in);

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCXN2), in);
                break;
        case SCRN_NBG3:
                _set_integer_scroll(&vdp2_state.nbg3.scroll.x, scroll, &in);

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCXN3), in);
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
                _set_fixed_point_scroll(&vdp2_state.nbg0.scroll.y, scroll, &in,
                    &dn);

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYIN0), in);
                MEMORY_WRITE(16, VDP2(SCYDN0), dn);
                break;
        case SCRN_NBG1:
                _set_fixed_point_scroll(&vdp2_state.nbg1.scroll.y, scroll, &in,
                    &dn);

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYIN1), in);
                MEMORY_WRITE(16, VDP2(SCYDN1), dn);
                break;
        case SCRN_NBG2:
                _set_integer_scroll(&vdp2_state.nbg3.scroll.y, scroll, &in);

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYN2), in);
                break;
        case SCRN_NBG3:
                _set_integer_scroll(&vdp2_state.nbg3.scroll.y, scroll, &in);

                /* Write to memory */
                MEMORY_WRITE(16, VDP2(SCYN3), in);
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
_set_integer_scroll(int16_t *scroll, fix16_t amount, uint16_t *in)
{
        *scroll = WRAP_INTEGER((int16_t)fix16_to_int(amount));
        *in = *scroll;
}
