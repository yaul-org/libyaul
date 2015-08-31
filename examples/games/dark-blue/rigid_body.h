/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

        bool kinematic;

        fix16_t mass;
        fix16_vector2_t displacement;
        fix16_vector2_t velocity;
        fix16_vector2_t acceleration;

#define RIGID_BODY_FORCES_MAX    4
        /* An extra force for gravity */
        fix16_vector2_t forces[RIGID_BODY_FORCES_MAX];
        uint32_t forces_cnt;
};

void rigid_body_init(struct rigid_body *, bool);
void rigid_body_forces_add(struct rigid_body *, const fix16_vector2_t *);
void rigid_body_forces_clear(struct rigid_body *);
void rigid_body_forces_sum(struct rigid_body *, fix16_vector2_t *);

#endif /* !RIGID_BODY_H */
