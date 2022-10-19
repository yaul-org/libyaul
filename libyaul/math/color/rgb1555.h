/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_COLOR_H_
#error "Header file must not be directly included"
#endif /* !_YAUL_MATH_COLOR_H_ */

union rgb1555 {
        struct {
                unsigned int msb:1;
                unsigned int b:5;
                unsigned int g:5;
                unsigned int r:5;
        } __packed;

        uint16_t raw;
} __aligned(2);

#define RGB1555_INITIALIZER(_msb, _r, _g, _b)                                  \
    {                                                                          \
            {                                                                  \
                _msb,                                                          \
                _b,                                                            \
                _g,                                                            \
                _r                                                             \
            }                                                                  \
    }

#define RGB1555(_msb, _r, _g, _b)                                              \
    ((rgb1555_t)RGB1555_INITIALIZER(_msb, _r, _g, _b))

#define RGB888_RGB1555_INITIALIZER(_msb, _r, _g, _b)                           \
    RGB1555_INITIALIZER(_msb, (_r) >> 3, (_g) >> 3, (_b) >> 3)

#define RGB888_RGB1555(_msb, _r, _g, _b)                                       \
    ((rgb1555_t)RGB888_RGB1555_INITIALIZER(_msb, _r, _g, _b))

extern void rgb888_rgb1555_conv(const rgb888_t *color_888, rgb1555_t *result);
