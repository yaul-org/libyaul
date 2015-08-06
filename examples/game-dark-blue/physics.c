/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "globals.h"
#include "physics.h"

#define OBJECT_POOL_MAX 64

static struct object *object_pool[OBJECT_POOL_MAX];
static uint32_t object_pool_idx = 0;

void
physics_init(void)
{
        object_pool_idx = 0;

        memset(&object_pool[0], 0x00, OBJECT_POOL_MAX * sizeof(struct object));
}

void
physics_update(void)
{
        uint32_t object_idx;
        for (object_idx = 0; object_idx < object_pool_idx; object_idx++) {
                struct object *object;
                object = object_pool[object_idx];

                if (!object->active) {
                        continue;
                }
        }
}

void
physics_object_add(struct object *object)
{
        assert((object_pool_idx + 1) < OBJECT_POOL_MAX);

        assert(object != NULL);
        /* Make sure that the object is active */
        assert(object->active);
        /* We need at least one collider */
        assert((object->colliders[0] != NULL) || (object->colliders[0] != NULL));

        object_pool[object_pool_idx] = object;
        object_pool_idx++;
}
