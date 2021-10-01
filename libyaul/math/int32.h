/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_INT32_H_
#define _YAUL_MATH_INT32_H_

#include <sys/cdefs.h>

#include <stdint.h>

typedef struct {
        int32_t x;
        int32_t y;
} __aligned(4) int32_vec2_t;

#define INT32_VEC2_INITIALIZER(x, y)                                           \
    {                                                                          \
            (x),                                                               \
            (y)                                                                \
    }

static inline void __always_inline
int32_vec2_zero(int32_vec2_t *result)
{
        result->x = 0;
        result->y = 0;
}

static inline void __always_inline
int32_vec2_dup(const int32_vec2_t * __restrict v0,
    int32_vec2_t * __restrict result)
{
        result->x = v0->x;
        result->y = v0->y;
}

static inline void __always_inline
int32_vec2_add(const int32_vec2_t * __restrict v0,
    const int32_vec2_t * __restrict v1,
    int32_vec2_t * __restrict result)
{
        result->x = v0->x + v1->x;
        result->y = v0->y + v1->y;
}

static inline void __always_inline
int32_vec2_sub(const int32_vec2_t * __restrict v0,
    const int32_vec2_t * __restrict v1,
    int32_vec2_t * __restrict result)
{
        result->x = v0->x - v1->x;
        result->y = v0->y - v1->y;
}

static inline void __always_inline
int32_vec2_scale(const int8_t scalar, const int32_vec2_t * __restrict v0,
    int32_vec2_t * __restrict result)
{
        result->x = scalar * v0->x;
        result->y = scalar * v0->y;
}

static inline void __always_inline
int32_vec2_scaled(const int8_t scalar, int32_vec2_t *in_out)
{
        in_out->x = scalar * in_out->x;
        in_out->y = scalar * in_out->y;
}

extern void int32_vec2_str(const int32_vec2_t *, char *);

#endif /* !_YAUL_MATH_INT32_H_ */
