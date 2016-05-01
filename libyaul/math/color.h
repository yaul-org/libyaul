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
}  __aligned (2) color_rgb555_t;

#define COLOR_RGB_INITIALIZER(r, g, b)                                         \
    {                                                                          \
            r,                                                                 \
            g,                                                                 \
            b                                                                  \
    }

static inline uint8_t color_rgb_min(const color_rgb555_t *color __attribute__ ((unused))) {
        return 0;
}

static inline uint8_t color_rgb_max(const color_rgb555_t *color __attribute__ ((unused))) {
        return 0;
}

typedef union {
        struct {
                unsigned int :8;
                unsigned int b:8;
                unsigned int g:8;
                unsigned int r:8;
        } __packed;
        uint8_t comp[4];
        uint32_t raw;
}  __aligned (4) color_rgb888_t;

#define COLOR_RGB888_INITIALIZER(r, g, b)                                      \
    {                                                                          \
            r,                                                                 \
            g,                                                                 \
            b                                                                  \
    }

static inline uint8_t color_rgb888_min(const color_rgb888_t *color __attribute__ ((unused))) {
        return 0;
}

static inline uint8_t color_rgb888_max(const color_rgb888_t *color __attribute__ ((unused))) {
        return 0;
}

typedef union {
        struct {
                fix16_t b;
                fix16_t g;
                fix16_t r;
        } __packed;
        fix16_t comp[3];
}  __packed color_fix16_rgb_t;

static inline fix16_t color_fix16_rgb_min(const color_fix16_rgb_t *color) {
        return fix16_min(fix16_min(color->r, color->g), color->b);
}

static inline fix16_t color_fix16_rgb_max(const color_fix16_rgb_t *color) {
        return fix16_max(fix16_max(color->r, color->g), color->b);
}

typedef union {
        struct {
                fix16_t v;
                fix16_t s;
                fix16_t h;
        };
        fix16_t comp[3];
} __packed color_fix16_hsv_t;

typedef union {
        struct {
                uint8_t v;
                uint8_t s;
                uint8_t h;
        } __aligned (4);
        uint8_t comp[4];
} __packed color_uint8_hsv_t;

extern void color_rgb555_hsv_convert(const color_rgb555_t *,
    color_fix16_hsv_t *);
extern void color_rgb888_hsv_convert(const color_rgb888_t *,
    color_fix16_hsv_t *);
extern void color_fix16_rgb_hsv_convert(const color_fix16_rgb_t *,
    color_fix16_hsv_t *);
extern void color_hsv_rgb555_convert(const color_fix16_hsv_t *,
    color_rgb555_t *);
extern void color_hsv_rgb888_convert(const color_fix16_hsv_t *,
    color_rgb888_t *);
extern void color_hsv_fix16_rgb_convert(const color_fix16_hsv_t *,
    color_fix16_rgb_t *);
extern void color_rgb555_rgb888_convert(const color_rgb555_t *,
    color_rgb888_t *);
extern void color_rgb888_fix16_rgb_convert(const color_rgb888_t *,
    color_fix16_rgb_t *);
extern void color_rgb888_rgb555_convert(const color_rgb888_t *,
    color_rgb555_t *);
extern void color_fix16_rgb_rgb888_convert(const color_fix16_rgb_t *,
    color_rgb888_t *);
extern void color_hsv_lerp(const color_fix16_hsv_t *, const color_fix16_hsv_t *,
    fix16_t, color_fix16_hsv_t *);
extern void color_hsv_str(const color_fix16_hsv_t *, char *, int32_t);

#endif /* !__libfixmath_color_h__ */
