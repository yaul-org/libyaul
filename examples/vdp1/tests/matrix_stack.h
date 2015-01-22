/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef MATRIX_STACK_H
#define MATRIX_STACK_H

#define FIXMATH_NO_OVERFLOW 1

#include <fixmath.h>

#include <sys/queue.h>

#define MATRIX_STACK_MODEL_VIEW_DEPTH   14
#define MATRIX_STACK_PROJECTION_DEPTH   2
#define MATRIX_STACK_DEPTH              (MATRIX_STACK_MODEL_VIEW_DEPTH +       \
    MATRIX_STACK_PROJECTION_DEPTH)

#define MATRIX_STACK_MODE_INVALID       -1
#define MATRIX_STACK_MODE_PROJECTION    0
#define MATRIX_STACK_MODE_MODEL_VIEW    1

#define MATRIX_STACK_MODES              2

struct matrix_stack;

SLIST_HEAD(matrix_stacks, matrix_stack);

struct matrix_stack {
        fix16_matrix4_t *ms_matrix;

        SLIST_ENTRY(matrix_stack) ms_entries;
} __aligned(8);

void matrix_stack_init(void);
void matrix_stack_mode(int32_t);
void matrix_stack_push(void);
void matrix_stack_pop(void);
struct matrix_stack *matrix_stack_top(int32_t);
void matrix_stack_load(fix16_matrix4_t *);

void matrix_stack_identity_load(void);
void matrix_stack_translate(fix16_t, fix16_t, fix16_t);
void matrix_stack_scale(fix16_t, fix16_t, fix16_t);
void matrix_stack_rotate(fix16_t, int32_t);
void matrix_stack_orthographic_project(fix16_t, fix16_t, fix16_t,
    fix16_t, fix16_t, fix16_t);

#endif /* !MATRIX_STACK_H */
