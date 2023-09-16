/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "internal.h"

extern fix16_mat43_t __pool_matrices[];

void
__matrix_init(void)
{
    fix16_mat43_t * const matrix_pool = &__pool_matrices[1];

    __state.mstack->pool_matrix = matrix_pool;
    __state.mstack->top_matrix = matrix_pool;
    __state.mstack->bottom_matrix = &matrix_pool[(MATRIX_STACK_COUNT - 1) - 1];
    __state.mstack->stack_count = MATRIX_STACK_COUNT - 1;

    for (uint32_t i = 0; i < MATRIX_STACK_COUNT - 1; i++) {
        fix16_mat43_identity(&matrix_pool[i]);
    }

    fix16_mat43_identity(__matrix_view_get());
}

fix16_mat43_t *
__matrix_view_get(void)
{
    return &__pool_matrices[0];
}

void
matrix_push(void)
{
    matrix_ptr_push();

    const fix16_mat43_t * const prev_matrix = __state.mstack->top_matrix - 1;

    fix16_mat43_dup(prev_matrix, __state.mstack->top_matrix);
}

void
matrix_ptr_push(void)
{
    assert(__state.mstack->top_matrix < __state.mstack->bottom_matrix);

    __state.mstack->top_matrix++;
}

void
matrix_pop(void)
{
    assert((__state.mstack->bottom_matrix - __state.mstack->top_matrix) > 1);

    /* Avoid letting the view matrix be "popped" */
    __state.mstack->top_matrix--;
}

fix16_mat43_t *
matrix_top(void)
{
    return __state.mstack->top_matrix;
}

void
matrix_copy(fix16_mat43_t *m0)
{
    fix16_mat43_dup(matrix_top(), m0);
}

void
matrix_set(const fix16_mat43_t *m0)
{
    fix16_mat43_dup(m0, matrix_top());
}

void
matrix_x_translate(fix16_t x)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    fix16_mat43_x_translate(top_matrix, top_matrix, x);
}

void
matrix_y_translate(fix16_t y)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    fix16_mat43_y_translate(top_matrix, top_matrix, y);
}

void
matrix_z_translate(fix16_t z)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    fix16_mat43_z_translate(top_matrix, top_matrix, z);
}

void
matrix_translate(const fix16_vec3_t *t)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    fix16_mat43_translate(top_matrix, top_matrix, t);
}

void
matrix_translation_set(const fix16_vec3_t *t)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    fix16_mat43_translation_set(top_matrix, t);
}

void
matrix_translation_get(fix16_vec3_t *t)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    t->x = top_matrix->frow[0][3];
    t->y = top_matrix->frow[1][3];
    t->z = top_matrix->frow[2][3];
}

void
matrix_x_rotate(angle_t angle)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    fix16_mat43_x_rotate(top_matrix, top_matrix, angle);
}

void
matrix_y_rotate(angle_t angle)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    fix16_mat43_y_rotate(top_matrix, top_matrix, angle);
}

void
matrix_z_rotate(angle_t angle)
{
    fix16_mat43_t * const top_matrix = __state.mstack->top_matrix;

    fix16_mat43_z_rotate(top_matrix, top_matrix, angle);
}
