/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef VECTOR4_H
#define VECTOR4_H

#include "types.h"

void fix16_vector4_normalize(fix16_vector4_t *);

fix16_t fix16_vector4_length(const fix16_vector4_t *);

void fix16_vector4_add(const fix16_vector4_t *, const fix16_vector4_t *,
    fix16_vector4_t *);
void fix16_vector4_sub(const fix16_vector4_t *, const fix16_vector4_t *,
    fix16_vector4_t *);

void fix16_vector4_scale(fix16_t, fix16_vector4_t *);

fix16_t fix16_vector4_dot(const fix16_vector4_t *, const fix16_vector4_t *);

void fix16_vector4_cross(const fix16_vector4_t *, const fix16_vector4_t *,
    fix16_vector4_t *);

#endif /* !VECTOR4_H */
