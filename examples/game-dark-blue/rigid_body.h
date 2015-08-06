/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef RIGID_BODY_H
#define RIGID_BODY_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include <inttypes.h>

struct object;

struct rigid_body {
        struct object *object;

        fix16_t mass;
        fix16_vector2_t velocity;
};

#endif /* !RIGID_BODY_H */
