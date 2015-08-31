/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef AABB_H
#define AABB_H

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

struct aabb {
        int16_vector2_t center;
        int16_vector2_t min;
        int16_vector2_t max;
        int16_vector2_t half;
};

static inline bool
aabb_aabb_test(const struct aabb *a0, const struct aabb *a1) {
        return ((uint32_t)(a0->max.x > a1->min.x) &
            (uint32_t)(a0->min.x < a1->max.x) &
            (uint32_t)(a0->max.y > a1->min.y) &
            (uint32_t)(a0->min.y < a1->max.y)) != 0;
}

#endif /* !AABB_H */
