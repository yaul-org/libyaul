/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include "common.h"
#include "collider.h"

void
collider_init(struct collider *collider, uint32_t id, uint16_t width,
    uint16_t height, bool trigger, bool fixed)
{
        assert(collider != NULL);

        collider->id = id;
        collider->trigger = trigger;
        collider->fixed = fixed;
        collider->aabb.center.x = width / 2;
        collider->aabb.center.y = height / 2;
        collider->aabb.min.x = 0;
        collider->aabb.min.y = 0;
        collider->aabb.max.x = width - 1;
        collider->aabb.max.y = height - 1;
        collider->aabb.half.x = width / 2;
        collider->aabb.half.y = height / 2;
}
