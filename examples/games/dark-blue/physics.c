/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

static struct object_pool {
        struct object_pool_entry {
                struct object *object;

                struct collision {
                        struct object *object;
                        struct collider_info info;
                } collisions[OBJECT_POOL_MAX - 1];
                uint32_t collisions_cnt;
        } entries[OBJECT_POOL_MAX];
} object_pool;
static uint32_t object_pool_entry_idx = 0;

static void stage_simulate(void);
static void stage_detect(void);
static void stage_detect_collisions_sort(struct object_pool_entry *);
static void stage_respond(void);

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
        object_pool_entry_idx = 0;

        memset(&object_pool.entries[0], 0x00,
            OBJECT_POOL_MAX * sizeof(struct object));
}

void
physics_update(void)
{
        stage_simulate();
        stage_detect();
        stage_respond();
}

void
physics_object_add(struct object *object)
{
        assert((object_pool_entry_idx + 1) < OBJECT_POOL_MAX);

        assert(object != NULL);
        /* Make sure that the object is active */
        assert(object->active);
        /* We need at least one collider */
        assert((object->colliders[0] != NULL) || (object->colliders[0] != NULL));

        object_pool.entries[object_pool_entry_idx].object = object;
        object_pool_entry_idx++;
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
        aabb.min.x = object->transform.pos_int.x +
            object->colliders[0]->aabb.min.x - 1;
        aabb.min.y = object->transform.pos_int.y +
            object->colliders[0]->aabb.min.y - 1;
        aabb.max.x = object->transform.pos_int.x +
            object->colliders[0]->aabb.max.x;
        aabb.max.y = object->transform.pos_int.y +
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
                object_center.x = object->transform.pos_int.x +
                    object->colliders[0]->aabb.center.x - 1;
                object_center.y = object->transform.pos_int.y +
                    object->colliders[0]->aabb.center.y - 1;

                int16_vector2_t other_center;
                other_center.x = other->transform.pos_int.x +
                    other->colliders[0]->aabb.center.x - 1;
                other_center.y = other->transform.pos_int.y +
                    other->colliders[0]->aabb.center.y - 1;

                int16_vector2_t center_diff;
                int16_vector2_sub(&other_center, &object_center, &center_diff);

                direction->x = sign(center_diff.x) * smallest_axis->x;
                direction->y = sign(center_diff.y) * smallest_axis->y;
        }

        return true;
}

static void
stage_simulate(void)
{
        uint32_t object_idx;
        for (object_idx = 0; object_idx < object_pool_entry_idx; object_idx++) {
                struct object *object;
                object = object_pool.entries[object_idx].object;

                if (!object->active) {
                        continue;
                }

                if (object->rigid_body == NULL) {
                        continue;
                }

                fix16_t dt;
                dt = F16(1.0f / 60.0f);

                fix16_t dt_2;
                dt_2 = F16((1.0f / 60.0f) * (1.0f / 60.0f));

                struct transform *transform;
                transform = &object->transform;

                struct rigid_body *rigid_body;
                rigid_body = object->rigid_body;

                if (!object->rigid_body->kinematic) {
                        fix16_vector2_t gravity =
                            FIX16_VECTOR2_INITIALIZER(0.0f, 0.0f);

                        /* Add obligatory gravitational force */
                        rigid_body_forces_add(rigid_body, &gravity);
                }

                /* Add up all forces */
                fix16_vector2_t forces;
                rigid_body_forces_sum(rigid_body, &forces);
                fix16_vector2_scaled(F16(1.0f / /* mass = */ 1.0f),
                    &forces, &rigid_body->acceleration);

                fix16_vector2_t last_displacement;
                fix16_vector2_dup(&rigid_body->displacement,
                    &last_displacement);

                /* Displacement: 1/2*(a_n*(dt**2)) + (v_n*dt) + x_n */
                fix16_vector2_t term_1;
                fix16_vector2_scaled(fix16_mul(F16(0.5f), dt_2),
                    &rigid_body->acceleration, &term_1);
                fix16_vector2_t term_2;
                fix16_vector2_scaled(dt, &rigid_body->velocity, &term_2);
                fix16_vector2_add(&term_1, &term_2, &term_1);
                fix16_vector2_add(&rigid_body->displacement, &term_1,
                    &rigid_body->displacement);

                /* Velocity: v_n+1 = (a_n*dt) + v_n */
                fix16_vector2_t scaled_a;
                fix16_vector2_scaled(dt, &rigid_body->acceleration, &scaled_a);
                fix16_vector2_add(&rigid_body->velocity, &scaled_a,
                    &rigid_body->velocity);

                rigid_body->velocity.y = fix16_clamp(rigid_body->velocity.y,
                    F16(-60.0f),
                    F16(120.0f));

                cons_buffer("*** acceleration=");
                fix16_vector2_str(&rigid_body->acceleration, text, 2);
                cons_buffer(text);
                cons_buffer("\n");
                cons_buffer("*** velocity=");
                fix16_vector2_str(&rigid_body->velocity, text, 2);
                cons_buffer(text);
                cons_buffer("\n");
                cons_buffer("*** displacement=");
                fix16_vector2_str(&rigid_body->displacement, text, 2);
                cons_buffer(text);
                cons_buffer("\n");

                int16_vector2_t displacement;
                displacement.x = fix16_to_int(rigid_body->displacement.x);
                displacement.y = fix16_to_int(rigid_body->displacement.y);

                int16_vector2_t initial;
                initial.x = transform->pos_int.x;
                initial.y = ((224 / 16) - 11) * 16;

                int16_vector2_add(&initial, &displacement, &transform->pos_int);

                /* Clear its forces */
                rigid_body_forces_clear(rigid_body);
        }
}

static void
stage_detect(void)
{
        uint32_t object_idx;
        for (object_idx = 0; object_idx < object_pool_entry_idx; object_idx++) {
                struct object_pool_entry *object_pool_entry;
                object_pool_entry = &object_pool.entries[object_idx];

                object_pool_entry->collisions_cnt = 0;
        }

        for (object_idx = 0; object_idx < object_pool_entry_idx; object_idx++) {
                struct object_pool_entry *object_pool_entry;
                object_pool_entry = &object_pool.entries[object_idx];

                struct object *object;
                object = object_pool_entry->object;

                if (!object->active) {
                        continue;
                }

                uint32_t other_idx;
                for (other_idx = object_idx + 1; other_idx < object_pool_entry_idx;
                     other_idx++) {
                        struct object_pool_entry *other_pool_entry;
                        other_pool_entry = &object_pool.entries[other_idx];

                        struct object *other;
                        other = other_pool_entry->object;

                        if (!other->active) {
                                continue;
                        }

                        struct collision *object_collision;
                        object_collision = &object_pool_entry->collisions[
                                object_pool_entry->collisions_cnt];

                        struct collision *other_collision;
                        other_collision = &other_pool_entry->collisions[
                                other_pool_entry->collisions_cnt];

                        int16_t overlap;
                        int16_vector2_t direction;

                        if ((sat_object_object_test(object, other, &overlap,
                                    &direction))) {
                                object_collision->object = other;
                                object_collision->info.overlap = overlap;
                                object_collision->info.direction.x = direction.x;
                                object_collision->info.direction.y = direction.y;
                                object_pool_entry->collisions_cnt++;

                                other_collision->object = object;
                                other_collision->info.overlap = overlap;
                                other_collision->info.direction.x = direction.x;
                                other_collision->info.direction.y = direction.y;
                                other_pool_entry->collisions_cnt++;
                        }
                }

                stage_detect_collisions_sort(object_pool_entry);
        }
}

static void
stage_detect_collisions_sort(struct object_pool_entry *object_pool_entry)
{
        if (object_pool_entry->collisions_cnt <= 1) {
                return;
        }

        bool swapped;
        do {
                swapped = false;

                uint32_t ci;
                for (ci = 0; ci < (object_pool_entry->collisions_cnt - 1); ci++) {
                        struct collision *collision_i;
                        collision_i =
                            &object_pool_entry->collisions[ci];

                        struct collision *collision_i_next;
                        collision_i_next =
                            &object_pool_entry->collisions[ci + 1];

                        bool swap;
                        swap = abs(collision_i->info.direction.x) <
                            abs(collision_i_next->info.direction.x);

                        swapped = true;

                        if (!swap) {
                                continue;
                        }

                        struct collision temp;
                        (void)memcpy(&temp, collision_i,
                            sizeof(struct collision));

                        (void)memcpy(collision_i, collision_i_next,
                            sizeof(struct collision));
                        (void)memcpy(collision_i_next, &temp,
                            sizeof(struct collision));
                }
        } while (!swapped);
}

static void
stage_respond(void)
{
        uint32_t object_idx;
        for (object_idx = 0; object_idx < object_pool_entry_idx; object_idx++) {
                struct object_pool_entry *object_pool_entry;
                object_pool_entry = &object_pool.entries[object_idx];

                struct object *object;
                object = object_pool_entry->object;

                if (!object->active) {
                        continue;
                }

                uint32_t collision_idx;
                for (collision_idx = 0;
                     collision_idx < object_pool_entry->collisions_cnt;
                     collision_idx++) {
                        struct collision *collision;
                        collision =
                            &object_pool_entry->collisions[collision_idx];

                        struct object *other;
                        other = collision->object;

                        if (!(sat_object_object_test(object, other, NULL, NULL))) {
                                continue;
                        }

                        int16_vector2_t position_delta;
                        position_delta.x = collision->info.overlap *
                            collision->info.direction.x;
                        position_delta.y = collision->info.overlap *
                            collision->info.direction.y;

                        if (object->rigid_body != NULL) {
                                if (collision->info.direction.x == 0) {
                                        //object->rigid_body->displacement.y = 0;
                                        object->rigid_body->velocity.y = F16(0.0f);
                                } else if (collision->info.direction.y == 0) {
                                        //object->rigid_body->displacement.x = 0;
                                        object->rigid_body->velocity.x = F16(0.0f);
                                }
                                
                        }

                        if (!object->colliders[0]->fixed) {
                                int16_vector2_add(&object->transform.pos_int,
                                    &position_delta,
                                    &object->transform.pos_int);
                        }

                        if (object->on_collision != NULL) {
                                OBJECT_CALL_EVENT(object, collision, other,
                                    &collision->info);
                        }
                }
        }
}
