/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>

#include "common.h"
#include "rigid_body.h"

void
rigid_body_init(struct rigid_body *rigid_body, bool kinematic)
{
        assert(rigid_body != NULL);

        rigid_body->kinematic = kinematic;

        rigid_body->mass = F16(1.0f);
        fix16_vector2_zero(&rigid_body->displacement);
        fix16_vector2_zero(&rigid_body->velocity);
        fix16_vector2_zero(&rigid_body->acceleration);

        memset(&rigid_body->forces[0], 0x00, sizeof(rigid_body->forces));
        rigid_body->forces_cnt = 0;
}

void
rigid_body_forces_add(struct rigid_body *rigid_body,
    const fix16_vector2_t *force)
{
        assert(rigid_body != NULL);
        assert(rigid_body->forces_cnt < RIGID_BODY_FORCES_MAX);

        memcpy(&rigid_body->forces[rigid_body->forces_cnt], force,
            sizeof(fix16_vector2_t));

        rigid_body->forces_cnt++;
}

void
rigid_body_forces_clear(struct rigid_body *rigid_body)
{
        assert(rigid_body != NULL);

        rigid_body->forces_cnt = 0;
}

void
rigid_body_forces_sum(struct rigid_body *rigid_body, fix16_vector2_t *result)
{
        assert(rigid_body != NULL);

        result->x = 0;
        result->y = 0;

        uint32_t force_idx;
        for (force_idx = 0; force_idx < rigid_body->forces_cnt; force_idx++) {
                fix16_vector2_t *force;
                force = &rigid_body->forces[force_idx];
                fix16_vector2_add(result, force, result);
        }
}
