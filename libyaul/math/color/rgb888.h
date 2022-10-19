/*
 * Copyright (c) 2012-2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_COLOR_H_
#error "Header file must not be directly included"
#endif /* !_YAUL_MATH_COLOR_H_ */

union rgb888 {
        struct {
                unsigned int cc:1;
                unsigned int :7;
                unsigned int b:8;
                unsigned int g:8;
                unsigned int r:8;
        } __packed;

        uint8_t comp[3];
        uint32_t raw;
} __aligned(4);

#define RGB888_INITIALIZER(_r, _g, _b)                                         \
    {                                                                          \
            {                                                                  \
                 0,                                                            \
                _b,                                                            \
                _g,                                                            \
                _r                                                             \
            }                                                                  \
    }

#define RGB888(_r, _g, _b)                                                     \
    ((rgb888_t)RGB888_INITIALIZER(_r, _g, _b))

#define RGB1555_RGB888_INITIALIZER(_msb, _r, _g, _b)                           \
    RGB888_INITIALIZER(((_r) << 3) | ((_r) & 7),                               \
                             ((_g) << 3) | ((_g) & 7),                         \
                             ((_b) << 3) | ((_b) & 7))

#define RGB1555_RGB888(_msb, _r, _g, _b)                                       \
    ((rgb888_t)RGB1555_RGB888_INITIALIZER(_msb, _r, _g, _b))

extern void rgb1555_rgb888_conv(const rgb1555_t *color_1555, rgb888_t *result);
