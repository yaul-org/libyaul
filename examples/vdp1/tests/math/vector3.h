/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef VECTOR3_H
#define VECTOR3_H

#include "types.h"

void fix16_vector3_zero(fix16_vector3_t *);

void fix16_vector3_normalize(fix16_vector3_t *);

fix16_t fix16_vector3_length(const fix16_vector3_t *);

void fix16_vector3_add(const fix16_vector3_t *, const fix16_vector3_t *,
    fix16_vector3_t *);
void fix16_vector3_sub(const fix16_vector3_t *, const fix16_vector3_t *,
    fix16_vector3_t *);

void fix16_vector3_scale(fix16_t, fix16_vector3_t *);

fix16_t fix16_vector3_dot(const fix16_vector3_t *, const fix16_vector3_t *);

void fix16_vector3_cross(const fix16_vector3_t *, const fix16_vector3_t *,
    fix16_vector3_t *);

void fix16_vector3_str(const fix16_vector3_t *, char *, int);

#endif /* !VECTOR3_H */
