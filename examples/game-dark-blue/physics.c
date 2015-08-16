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

#define OBJECT_POOL_MAX 32

static void physics_stage_simulate(void);
static void physics_stage_detect(void);
static void physics_stage_respond(void);

static struct object *object_pool[OBJECT_POOL_MAX];
static uint32_t object_pool_idx = 0;

static struct collision_pool {
        struct collision_pool_entry {
                struct object *object;
                struct collision {
                        struct object *object;
                        struct collider_info info;
                } collisions[OBJECT_POOL_MAX - 1];
                uint32_t collisions_cnt;
        } entries[OBJECT_POOL_MAX];
} collision_pool;

static uint32_t collision_pool_entry_idx = 0;

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
    struct sat_projection *project)
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
        project[0].min = aabb.min.y;
        project[0].max = aabb.max.y;

        /* dot(right_vector, vertex[1]) */
        project[1].min = aabb.min.x;
        project[1].max = aabb.max.x;

        /* dot(down_vector, vertex[2]) */
        project[2].min = -aabb.max.y;
        project[2].max = -aabb.min.y;

        /* dot(left_vector, vector[3]) */
        project[3].min = -aabb.max.x;
        project[3].max = -aabb.min.x;
}

static inline bool
sat_object_object_test(const struct object *object, const struct object *other,
    int16_t *overlap, int16_vector2_t *direction)
{
        static const int16_vector2_t axes[] = {
                INT16_VECTOR2_INITIALIZER( 0,  1),
                INT16_VECTOR2_INITIALIZER( 1,  0),
                INT16_VECTOR2_INITIALIZER( 0, -1),
                INT16_VECTOR2_INITIALIZER(-1,  0)
        };

        struct sat_projection object_project[4];
        sat_object_projection_calculate(object, object_project);

        struct sat_projection other_project[4];
        sat_object_projection_calculate(other, other_project);

        const int16_vector2_t *smallest_axis;
        smallest_axis = NULL;

        int16_t overlapped;
        overlapped = INT16_MAX;

        uint32_t axis;
        for (axis = 0; axis < 4; axis++) {
                if (!(sat_overlap_test(&object_project[axis],
                            &other_project[axis]))) {
                        return false;
                }

                if ((overlap != NULL) || (direction != NULL)) {
                        int16_t amount;
                        amount = sat_overlap(&object_project[axis],
                            &other_project[axis]);
                        if (amount < overlapped) {
                                overlapped = amount;
                                smallest_axis = &axes[axis];
                        }
                }
        }

        if (overlap != NULL) {
                *overlap = overlapped;
        }

        if (direction != NULL) {
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

                direction->x = sign(center_diff.x) * smallest_axis->x;
                direction->y = sign(center_diff.y) * smallest_axis->y;
        }

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
        collision_pool_entry_idx = 0;

        uint32_t object_idx;
        for (object_idx = 0; object_idx < object_pool_idx; object_idx++) {
                struct object *object;
                object = object_pool[object_idx];

                if (!object->active) {
                        continue;
                }

                struct collision_pool_entry *collision_pool_entry;
                collision_pool_entry =
                    &collision_pool.entries[collision_pool_entry_idx];

                collision_pool_entry->object = object;
                collision_pool_entry->collisions_cnt = 0;

                uint32_t collision_idx;
                collision_idx = 0;

                uint32_t other_idx;
                for (other_idx = object_idx + 1; other_idx < object_pool_idx;
                     other_idx++) {
                        struct object *other;
                        other = object_pool[other_idx];

                        if (!other->active) {
                                continue;
                        }

                        struct collision *collision;
                        collision =
                            &collision_pool_entry->collisions[collision_idx];

                        if ((sat_object_object_test(object, other, NULL, NULL))) {
                                collision_pool_entry->collisions_cnt =
                                    collision_idx + 1;

                                collision->object = other;

                                collision_idx++;
                        }
                }

                collision_pool_entry_idx++;
        }
}

static void
physics_stage_respond(void)
{
        uint32_t cpe_idx;
        for (cpe_idx = 0; cpe_idx < collision_pool_entry_idx; cpe_idx++) {
                struct collision_pool_entry *collision_pool_entry;
                collision_pool_entry = &collision_pool.entries[cpe_idx];

                struct object *object;
                object = collision_pool_entry->object;

                uint32_t collision_idx;
                for (collision_idx = 0;
                     collision_idx < collision_pool_entry->collisions_cnt;
                     collision_idx++) {
                        struct collision *collision;
                        collision =
                            &collision_pool_entry->collisions[collision_idx];

                        struct object *other;
                        other = collision->object;

                        int16_t overlap;
                        int16_vector2_t direction;

                        if (!(sat_object_object_test(object, other, &overlap,
                                    &direction))) {
                                continue;
                        }

                        int16_vector2_t delta;
                        delta.x = overlap * direction.x;
                        delta.y = overlap * direction.y;

                        if (!object->colliders[0]->fixed) {
                                int16_vector2_add(&object->transform.position,
                                    &delta,
                                    &object->transform.position);
                        }

                        if (!other->colliders[0]->fixed) {
                                int16_vector2_add(&other->transform.position,
                                    &delta,
                                    &other->transform.position);
                        }

                        collision->info.overlap = overlap;
                        collision->info.direction = direction;

                        if (object->on_collision != NULL) {
                                OBJECT_CALL_EVENT(object, collision, other,
                                    &collision->info);
                        }

                        if (other->on_collision != NULL) {
                                OBJECT_CALL_EVENT(other, collision, object,
                                    &collision->info);
                        }
                }
        }
}
