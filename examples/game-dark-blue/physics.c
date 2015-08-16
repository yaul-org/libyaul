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

static void physics_stage_simulate(void);
static void physics_stage_detect(void);
static void physics_stage_respond(void);

static struct object *object_pool[OBJECT_POOL_MAX];
static uint32_t object_pool_idx = 0;

static struct collision {
        struct {
                struct object *object;
                struct collider_info info;
        } object, other;
} collision_pool[OBJECT_POOL_MAX] __unused;

static uint32_t collision_pool_idx = 0;

struct sat_projection {
        int16_t min;
        int16_t max;
};

static inline bool sat_overlap_test(const struct sat_projection *,
    const struct sat_projection *);
static inline int16_t sat_overlap(const struct sat_projection *a,
    const struct sat_projection *b);
static inline bool sat_object_object_test(const struct object *,
    const struct object *, int16_t *, int16_vector2_t *);
static inline void sat_object_projection_calculate(const struct object *,
    struct sat_projection *);

void
physics_init(void)
{
        object_pool_idx = 0;

        memset(&object_pool[0], 0x00, OBJECT_POOL_MAX * sizeof(struct object));
}

void
physics_update(void)
{
        physics_stage_simulate();
        physics_stage_detect();
        physics_stage_respond();
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

static inline int16_t
sat_overlap(const struct sat_projection *a, const struct sat_projection *b)
{
        return (min(a->max, b->max) - max(a->min, b->min));
}

static inline bool
sat_overlap_test(const struct sat_projection *a, const struct sat_projection *b)
{
        return (((uint32_t)((a->max - b->min) > 0) |
                (uint32_t)((a->min - b->max) > 0)) != 0);
}

static inline void
sat_object_projection_calculate(const struct object *object,
    struct sat_projection *projected)
{
        struct aabb aabb;
        aabb.min.x = object->transform.position.x +
            object->colliders[0]->aabb.min.x - 1;
        aabb.min.y = object->transform.position.y +
            object->colliders[0]->aabb.min.y - 1;
        aabb.max.x = object->transform.position.x +
            object->colliders[0]->aabb.max.x;
        aabb.max.y = object->transform.position.y +
            object->colliders[0]->aabb.max.y;

        /* dot(up_vector, vertex[0]) */
        projected[0].min = aabb.min.y;
        projected[0].max = aabb.max.y;

        /* dot(right_vector, vertex[1]) */
        projected[1].min = aabb.min.x;
        projected[1].max = aabb.max.x;

        /* dot(down_vector, vertex[2]) */
        projected[2].min = -aabb.max.y;
        projected[2].max = -aabb.min.y;

        /* dot(left_vector, vector[3]) */
        projected[3].min = -aabb.max.x;
        projected[3].max = -aabb.min.x;
}

static inline bool
sat_object_object_test(const struct object *object, const struct object *other,
    int16_t *overlap, int16_vector2_t *smallest)
{
        static const int16_vector2_t axes[] = {
                INT16_VECTOR2_INITIALIZER( 0,  1),
                INT16_VECTOR2_INITIALIZER( 1,  0),
                INT16_VECTOR2_INITIALIZER( 0, -1),
                INT16_VECTOR2_INITIALIZER(-1,  0)
        };

        struct sat_projection object_projected[4];
        sat_object_projection_calculate(object, object_projected);

        struct sat_projection other_projected[4];
        sat_object_projection_calculate(other, other_projected);

        const int16_vector2_t *smallest_copy;
        smallest_copy = NULL;

        int16_t overlap_copy;
        overlap_copy = INT16_MAX;

        uint32_t axis;
        for (axis = 0; axis < 4; axis++) {
                if (!(sat_overlap_test(&object_projected[axis],
                            &other_projected[axis]))) {
                        return false;
                }

                int16_t amount;
                amount = sat_overlap(&object_projected[axis],
                    &other_projected[axis]);

                if (amount < overlap_copy) {
                        overlap_copy = amount;
                        smallest_copy = &axes[axis];
                }
        }

        *overlap = overlap_copy;

        int16_vector2_t object_center;
        object_center.x = object->transform.position.x +
            object->colliders[0]->aabb.center.x - 1;
        object_center.y = object->transform.position.y +
            object->colliders[0]->aabb.center.y - 1;

        int16_vector2_t other_center;
        other_center.x = other->transform.position.x +
            other->colliders[0]->aabb.center.x - 1;
        other_center.y = other->transform.position.y +
            other->colliders[0]->aabb.center.y - 1;

        int16_vector2_t center_diff;
        int16_vector2_sub(&other_center, &object_center, &center_diff);

        smallest->x = sign(center_diff.x) * smallest_copy->x;
        smallest->y = sign(center_diff.y) * smallest_copy->y;

        return true;
}

static void
physics_stage_simulate(void)
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

static void
physics_stage_detect(void)
{
        collision_pool_idx = 0;

        uint32_t object_idx;
        for (object_idx = 0; object_idx < object_pool_idx; object_idx++) {
                struct object *object;
                object = object_pool[object_idx];

                if (!object->active) {
                        continue;
                }

                uint32_t other_idx;
                for (other_idx = object_idx + 1; other_idx < object_pool_idx;
                     other_idx++) {
                        struct object *other;
                        other = object_pool[other_idx];

                        if (!other->active) {
                                continue;
                        }

                        int16_t overlap;
                        int16_vector2_t smallest;

                        if ((sat_object_object_test(object, other, &overlap,
                                    &smallest))) {
                                struct collision *collision;
                                collision = &collision_pool[collision_pool_idx];

                                collision->object.object = object;
                                collision->other.object = other;

                                collision->object.info.overlap = overlap;
                                collision->object.info.smallest.x = smallest.x;
                                collision->object.info.smallest.y = smallest.y;

                                collision->other.info.overlap = overlap;
                                collision->other.info.smallest.x = smallest.x;
                                collision->other.info.smallest.y = smallest.y;

                                collision_pool_idx++;
                        }
                }
        }
}

static void
physics_stage_respond(void)
{
        uint32_t collision_idx;
        for (collision_idx = 0;
             collision_idx < collision_pool_idx; collision_idx++) {
        /* int16_vector2_t delta; */
        /* delta.x = info->smallest.x; */
        /* delta.y = info->smallest.y; */

        /* int16_vector2_scale(info->overlap, &delta); */

        /* int16_vector2_add(&player->transform.position, */
        /*     &delta, */
        /*     &player->transform.position); */

                struct collision *collision;
                collision = &collision_pool[collision_idx];

                struct object *object;
                object = collision->object.object;

                struct object *other;
                other = collision->other.object;

                if (object->on_collision != NULL) {
                        OBJECT_CALL_EVENT(object, collision, other,
                            &collision->other.info);
                }

                if (other->on_collision != NULL) {
                        OBJECT_CALL_EVENT(other, collision, object,
                            &collision->object.info);
                }
        }
}
