/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_COLOR_H_
#error "Header file must not be directly included"
#endif /* !_YAUL_MATH_COLOR_H_ */

typedef union {
        struct {
                uint8_t v;
                uint8_t s;
                /* XXX: Hue is in the range of [0..255] instead of [0..360] */
                uint8_t h;
        };

        uint8_t comp[3];
} __aligned(4) hsv_t;

#define HSV_INITIALIZER(_h, _s, _v)                                            \
    {                                                                          \
            {                                                                  \
                _v,                                                            \
                _s,                                                            \
                _h                                                             \
            }                                                                  \
    }

#define HSV(_h, _s, _v)                                                        \
    ((hsv_t)HSV_INITIALIZER(_h, _s, _v))

extern void rgb888_hsv_conv(const rgb888_t *color_888, hsv_t *result);
