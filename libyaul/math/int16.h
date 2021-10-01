/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_INT16_H_
#define _YAUL_MATH_INT16_H_

#include <sys/cdefs.h>

#include <stdint.h>

typedef struct {
        int16_t x;
        int16_t y;
} __aligned(4) int16_vec2_t;

#define INT16_VEC2_INITIALIZER(x, y)                                           \
    {                                                                          \
            (x),                                                               \
            (y)                                                                \
    }

static inline void __always_inline
int16_vec2_zero(int16_vec2_t *result)
{
        result->x = 0;
        result->y = 0;
}

static inline void __always_inline
int16_vec2_dup(const int16_vec2_t * __restrict v0,
    int16_vec2_t * __restrict result)
{
        result->x = v0->x;
        result->y = v0->y;
}

static inline void __always_inline
int16_vec2_add(const int16_vec2_t * __restrict v0,
    const int16_vec2_t * __restrict v1,
    int16_vec2_t * __restrict result)
{
        result->x = v0->x + v1->x;
        result->y = v0->y + v1->y;
}

static inline void __always_inline
int16_vec2_sub(const int16_vec2_t * __restrict v0,
    const int16_vec2_t * __restrict v1,
    int16_vec2_t * __restrict result)
{
        result->x = v0->x - v1->x;
        result->y = v0->y - v1->y;
}

static inline void __always_inline
int16_vec2_scale(const int8_t scalar, const int16_vec2_t * __restrict v0,
    int16_vec2_t * __restrict result)
{
        result->x = scalar * v0->x;
        result->y = scalar * v0->y;
}

static inline void __always_inline
int16_vec2_scaled(const int8_t scalar, int16_vec2_t *in_out)
{
        in_out->x = scalar * in_out->x;
        in_out->y = scalar * in_out->y;
}

extern void int16_vec2_str(const int16_vec2_t *, char *);

#endif /* !_YAUL_MATH_INT16_H_ */
