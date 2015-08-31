/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef TRANSFORM_H
#define TRANSFORM_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include <inttypes.h>

struct object;

struct transform {
        struct object *object;

        int16_vector2_t position;
} transform;

#endif /* !TRANSFORM_H */
