/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_INT8_H_
#define _YAUL_MATH_INT8_H_

#include <sys/cdefs.h>

#include <stdint.h>

typedef struct {
        int8_t x;
        int8_t y;
} __aligned(4) int8_vec2_t;

typedef struct {
        int8_t x;
        int8_t y;
        int8_t z;
} __aligned(4) int8_vec3_t;

#define INT8_VEC2_INITIALIZER(x, y)                                            \
    {                                                                          \
            (x),                                                               \
            (y)                                                                \
    }

#define INT8_VEC3_INITIALIZER(x, y)                                            \
    {                                                                          \
            (x),                                                               \
            (y),                                                               \
            (z)                                                                \
    }

#endif /* !_YAUL_MATH_INT8_H_ */
