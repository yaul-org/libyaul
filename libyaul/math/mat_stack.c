#include <assert.h>

#include "mat_stack.h"

void
mat_stack_init(mat_stack_t *mat_stack, fix16_mat_t *matrix_pool, uint32_t count)
{
        assert(mat_stack != NULL);
        assert(matrix_pool != NULL);
        assert(count > 0);

        mat_stack->pool_matrix = matrix_pool;
        mat_stack->top_matrix = matrix_pool;
        mat_stack->bottom_matrix = &matrix_pool[count - 1];
        mat_stack->stack_count = count;

        for (uint32_t i = 0; i < mat_stack->stack_count; i++) {
                fix16_mat_identity(&mat_stack->top_matrix[i]);
        }
}

void
mat_stack_reset(mat_stack_t *mat_stack)
{
        assert(mat_stack != NULL);

        mat_stack->top_matrix = mat_stack->pool_matrix;
}

void
mat_stack_push(mat_stack_t *mat_stack)
{
        assert(mat_stack != NULL);

        mat_stack_ptr_push(mat_stack);

        const fix16_mat_t * const prev_matrix = mat_stack->top_matrix - 1;

        fix16_mat_dup(prev_matrix, mat_stack->top_matrix);
}

void
mat_stack_ptr_push(mat_stack_t *mat_stack)
{
        assert(mat_stack != NULL);
        assert(mat_stack->top_matrix < mat_stack->bottom_matrix);

        mat_stack->top_matrix++;
}

void
mat_stack_pop(mat_stack_t *mat_stack)
{
        assert(mat_stack != NULL);

        if (mat_stack->top_matrix != mat_stack->bottom_matrix) {
                mat_stack->top_matrix--;
        }
}

void
mat_stack_set(mat_stack_t *mat_stack, const fix16_mat_t *m0)
{
        assert(mat_stack != NULL);

        fix16_mat_dup(m0, mat_stack->top_matrix);
}

void
mat_stack_copy(mat_stack_t *mat_stack, fix16_mat_t *m0)
{
        assert(mat_stack != NULL);

        fix16_mat_dup(mat_stack->top_matrix, m0);
}

void
mat_stack_identity(mat_stack_t *mat_stack)
{
        assert(mat_stack != NULL);

        fix16_mat_identity(mat_stack->top_matrix);
}

void
mat_stack_x_translate(mat_stack_t *mat_stack, fix16_t x)
{
        assert(mat_stack != NULL);

        fix16_mat_t * const top_matrix = mat_stack->top_matrix;

        fix16_mat_x_translate(top_matrix, top_matrix, x);
}

void
mat_stack_y_translate(mat_stack_t *mat_stack, fix16_t y)
{
        assert(mat_stack != NULL);

        fix16_mat_t * const top_matrix = mat_stack->top_matrix;

        fix16_mat_y_translate(top_matrix, top_matrix, y);
}

void
mat_stack_z_translate(mat_stack_t *mat_stack, fix16_t z)
{
        assert(mat_stack != NULL);

        fix16_mat_t * const top_matrix = mat_stack->top_matrix;

        fix16_mat_z_translate(top_matrix, top_matrix, z);
}

void
mat_stack_translate(mat_stack_t *mat_stack, const fix16_vec3_t *t)
{
        assert(mat_stack != NULL);

        fix16_mat_t * const top_matrix = mat_stack->top_matrix;

        fix16_mat_translate(top_matrix, top_matrix, t);
}

void
mat_stack_translation_set(mat_stack_t *mat_stack, const fix16_vec3_t *t)
{
        assert(mat_stack != NULL);

        fix16_mat_t * const top_matrix = mat_stack->top_matrix;

        fix16_mat_translation_set(top_matrix, t);
}

void
mat_stack_x_rotate(mat_stack_t *mat_stack, angle_t angle)
{
        assert(mat_stack != NULL);

        fix16_mat_t * const top_matrix = mat_stack->top_matrix;

        fix16_mat_x_rotate(top_matrix, top_matrix, angle);
}

void
mat_stack_y_rotate(mat_stack_t *mat_stack, angle_t angle)
{
        assert(mat_stack != NULL);

        fix16_mat_t * const top_matrix = mat_stack->top_matrix;

        fix16_mat_y_rotate(top_matrix, top_matrix, angle);
}

void
mat_stack_z_rotate(mat_stack_t *mat_stack, angle_t angle)
{
        assert(mat_stack != NULL);

        fix16_mat_t * const top_matrix = mat_stack->top_matrix;

        fix16_mat_z_rotate(top_matrix, top_matrix, angle);
}
