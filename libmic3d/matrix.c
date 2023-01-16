#include <mat_stack.h>

#include "internal.h"

extern fix16_mat43_t __pool_matrices[];

void
__matrix_init(void)
{
        mat_stack_init(__state.mat_stack, &__pool_matrices[1], MATRIX_STACK_COUNT - 1);

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
        mat_stack_push(__state.mat_stack);
}

void
matrix_ptr_push(void)
{
        mat_stack_ptr_push(__state.mat_stack);
}

void
matrix_pop(void)
{
        mat_stack_pop(__state.mat_stack);
}

fix16_mat43_t *
matrix_top(void)
{
        return mat_stack_top(__state.mat_stack);
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
        mat_stack_x_translate(__state.mat_stack, x);
}

void
matrix_y_translate(fix16_t y)
{
        mat_stack_y_translate(__state.mat_stack, y);
}

void
matrix_z_translate(fix16_t z)
{
        mat_stack_z_translate(__state.mat_stack, z);
}

void
matrix_translate(const fix16_vec3_t *t)
{
        mat_stack_translate(__state.mat_stack, t);
}

void
matrix_translation_set(const fix16_vec3_t *t)
{
        mat_stack_translation_set(__state.mat_stack, t);
}

void
matrix_translation_get(fix16_vec3_t *t)
{
        mat_stack_translation_get(__state.mat_stack, t);
}

void
matrix_x_rotate(angle_t angle)
{
        mat_stack_x_rotate(__state.mat_stack, angle);
}

void
matrix_y_rotate(angle_t angle)
{
        mat_stack_y_rotate(__state.mat_stack, angle);
}

void
matrix_z_rotate(angle_t angle)
{
        mat_stack_z_rotate(__state.mat_stack, angle);
}
