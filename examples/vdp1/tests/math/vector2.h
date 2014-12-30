/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef VECTOR2_H
#define VECTOR2_H

#include "types.h"

void int16_vector2_normalize(int16_vector2_t *);

int16_t int16_vector2_length(const int16_vector2_t *);

void int16_vector2_add(const int16_vector2_t *, const int16_vector2_t *,
    int16_vector2_t *);
void int16_vector2_sub(const int16_vector2_t *, const int16_vector2_t *,
    int16_vector2_t *);

void int16_vector2_scale(int8_t, int16_vector2_t *);

int16_t int16_vector2_dot(const int16_vector2_t *, const int16_vector2_t *);

void int16_vector2_str(const int16_vector2_t *, char *);

#endif /* !VECTOR2_H */
