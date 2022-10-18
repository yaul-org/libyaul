/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_UINT8_H_
#define _YAUL_MATH_UINT8_H_

#include <sys/cdefs.h>

#include <stdint.h>

typedef struct {
        uint8_t x;
        uint8_t y;
} uint8_vec2_t;

typedef struct {
        uint8_t x;
        uint8_t y;
        uint8_t z;
} uint8_vec3_t;

#define UINT8_VEC2_INITIALIZER(x, y)                                            \
    {                                                                          \
            (x),                                                               \
            (y)                                                                \
    }

#define UINT8_VEC3_INITIALIZER(x, y)                                            \
    {                                                                          \
            (x),                                                               \
            (y),                                                               \
            (z)                                                                \
    }

#endif /* !_YAUL_MATH_UINT8_H_ */
