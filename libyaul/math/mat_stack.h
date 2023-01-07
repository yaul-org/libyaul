/*
 * Copyright (c) 2022
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_MATH_MAT_STACK_H_
#define _YAUL_MATH_MAT_STACK_H_

#include <sys/cdefs.h>

#include <fix16.h>

__BEGIN_DECLS

typedef struct mat_stack {
        fix16_mat43_t *pool_matrix;
        fix16_mat43_t *top_matrix;
        fix16_mat43_t *bottom_matrix;
        uint32_t stack_count;
} mat_stack_t;

static inline fix16_mat43_t * __always_inline
mat_stack_top(mat_stack_t *mat_stack)
{
        return mat_stack->top_matrix;
}

extern void mat_stack_init(mat_stack_t *mat_stack, fix16_mat43_t *matrix_pool, uint32_t count);
extern void mat_stack_reset(mat_stack_t *mat_stack);

extern void mat_stack_push(mat_stack_t *mat_stack);
extern void mat_stack_ptr_push(mat_stack_t *mat_stack);
extern void mat_stack_pop(mat_stack_t *mat_stack);
extern void mat_stack_set(mat_stack_t *mat_stack, const fix16_mat43_t *m0);
extern void mat_stack_copy(mat_stack_t *mat_stack, fix16_mat43_t *m0);

extern void mat_stack_identity(mat_stack_t *mat_stack);

extern void mat_stack_x_translate(mat_stack_t *mat_stack, fix16_t x);
extern void mat_stack_y_translate(mat_stack_t *mat_stack, fix16_t y);
extern void mat_stack_z_translate(mat_stack_t *mat_stack, fix16_t z);
extern void mat_stack_translate(mat_stack_t *mat_stack, const fix16_vec3_t *t);
extern void mat_stack_translation_set(mat_stack_t *mat_stack, const fix16_vec3_t *t);
extern void mat_stack_translation_get(mat_stack_t *mat_stack, fix16_vec3_t *t);

extern void mat_stack_x_rotate(mat_stack_t *mat_stack, angle_t angle);
extern void mat_stack_y_rotate(mat_stack_t *mat_stack, angle_t angle);
extern void mat_stack_z_rotate(mat_stack_t *mat_stack, angle_t angle);

__END_DECLS

#endif /* !_YAUL_MATH_MAT_STACK_H_ */
