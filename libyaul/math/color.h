#ifndef __libfixmath_color_h__
#define __libfixmath_color_h__

#include "fix16.h"

typedef union {
        struct {
                unsigned int :1;
                unsigned int b:5;
                unsigned int g:5;
                unsigned int r:5;
        } __packed;
        uint8_t comp[3];
        uint16_t raw;
}  __aligned (4) color_rgb_t;

typedef union {
        struct {
                fix16_t v;
                fix16_t s;
                fix16_t h;
        };
        fix16_t comp[3];
} __aligned (4) color_fix16_hsv_t;

typedef union {
        struct {
                uint8_t v;
                uint8_t s;
                uint8_t h;
        };
        uint8_t comp[3];
} __aligned (4) color_uint8_hsv_t;

extern void color_rgb_hsv_convert(const color_rgb_t *,
    color_fix16_hsv_t *);

extern void color_hsv_lerp(const color_fix16_hsv_t *, const color_fix16_hsv_t *,
    fix16_t, color_fix16_hsv_t *);

#endif /* !__libfixmath_color_h__ */
