/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef MATRIX_H
#define MATRIX_H

#include "types.h"

fix16_matrix4_t *fix16_matrix_zero(void);
fix16_matrix4_t *fix16_matrix_identity(void);
void fix16_matrix_multiply(const fix16_matrix4_t *, const fix16_matrix4_t *,
    fix16_matrix4_t *);
void fix16_matrix4_transpose(const fix16_matrix4_t *, fix16_matrix4_t *);

#endif /* !MATRIX_H */
