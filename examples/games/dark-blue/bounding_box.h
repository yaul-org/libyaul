/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1

#include <fixmath.h>

struct bounding_box {
        union {
                int16_vector2_t points[4];

                struct {
                        int16_vector2_t a;
                        int16_vector2_t b;
                        int16_vector2_t c;
                        int16_vector2_t d;
                } point;
        };
};

#endif /* !BOUNDING_BOX_H */
