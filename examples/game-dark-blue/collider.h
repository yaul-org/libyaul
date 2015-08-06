/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef COLLIDER_H
#define COLLIDER_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

#include <inttypes.h>

#include "bounding_box.h"

struct object;

struct collider {
        struct object *object;

        uint32_t id;
        bool trigger;
        struct bounding_box bb;
};

#endif /* !COLLIDER_H */
