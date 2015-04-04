/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#define ASSERT                  1
#define FIXMATH_NO_OVERFLOW     1
#define FIXMATH_NO_ROUNDING     1

#include <stdbool.h>
#include <stdio.h>

#include <yaul.h>

#include "matrix_stack.h"

static struct {
        bool mss_initialized;
        int32_t mss_mode;
} state;

static struct matrix_stacks matrix_stacks[MATRIX_STACK_MODES];

MEMB(matrix_stack_pool, struct matrix_stack, MATRIX_STACK_DEPTH,
    sizeof(struct matrix_stack));
MEMB(matrix_stack_matrix_pool, fix16_matrix4_t, MATRIX_STACK_DEPTH,
    sizeof(fix16_matrix4_t));

void
matrix_stack_init(void)
{
        if (state.mss_initialized) {
                return;
        }

        SLIST_INIT(&matrix_stacks[MATRIX_STACK_MODE_PROJECTION]);
        SLIST_INIT(&matrix_stacks[MATRIX_STACK_MODE_MODEL_VIEW]);

        memb_init(&matrix_stack_pool);
        memb_init(&matrix_stack_matrix_pool);

        state.mss_initialized = true;
        state.mss_mode = MATRIX_STACK_MODE_INVALID;

        matrix_stack_mode(MATRIX_STACK_MODE_PROJECTION);
        matrix_stack_push();
        matrix_stack_identity_load();

        matrix_stack_mode(MATRIX_STACK_MODE_MODEL_VIEW);
        matrix_stack_push();
        matrix_stack_identity_load();
}

void
matrix_stack_mode(int32_t mode)
{
#ifdef ASSERT
        assert(state.mss_initialized);
        assert((mode == MATRIX_STACK_MODE_PROJECTION) ||
               (mode == MATRIX_STACK_MODE_MODEL_VIEW));
#endif /* ASSERT */

        state.mss_mode = mode;
}

void
matrix_stack_push(void)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(state.mss_initialized);
        assert(state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *ms;
        ms = (struct matrix_stack *)memb_alloc(&matrix_stack_pool);

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(state.mss_mode);

        ms->ms_matrix = (fix16_matrix4_t *)memb_alloc(
                &matrix_stack_matrix_pool);
#ifdef ASSERT
        assert(ms->ms_matrix != NULL);
#endif /* ASSERT */

        if (top_ms != NULL) {
                memcpy(ms->ms_matrix, top_ms->ms_matrix,
                    sizeof(fix16_matrix4_t));
        }

        SLIST_INSERT_HEAD(&matrix_stacks[state.mss_mode], ms,
            ms_entries);
}

void
matrix_stack_pop(void)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(state.mss_initialized);
        assert(state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        int error_code __unused;
        error_code = memb_free(&matrix_stack_matrix_pool, top_ms->ms_matrix);
#ifdef ASSERT
        assert(error_code == 0);
#endif /* ASSERT */
        error_code = memb_free(&matrix_stack_pool, top_ms);
#ifdef ASSERT
        assert(error_code == 0);
#endif
        SLIST_REMOVE_HEAD(&matrix_stacks[state.mss_mode],
            ms_entries);

        /* Make sure we didn't pop off the last matrix in the stack */
#ifdef ASSERT
        assert(!(SLIST_EMPTY(&matrix_stacks[state.mss_mode])));
#endif /* ASSERT */
}

struct matrix_stack *
matrix_stack_top(int32_t mode)
{
        /* Make sure the correct state is set */
#ifdef ASSERT
        assert(state.mss_initialized);
        assert((mode == MATRIX_STACK_MODE_PROJECTION) ||
               (mode == MATRIX_STACK_MODE_MODEL_VIEW));
#endif /* ASSERT */

        return SLIST_FIRST(&matrix_stacks[mode]);
}

void
matrix_stack_load(fix16_matrix4_t *matrix)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(state.mss_initialized);
        assert(state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        memcpy(top_ms->ms_matrix, matrix, sizeof(fix16_matrix4_t));
}

void
matrix_stack_identity_load(void)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(state.mss_initialized);
        assert(state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_identity(top_ms->ms_matrix);
}

void
matrix_stack_translate(fix16_t x, fix16_t y, fix16_t z)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(state.mss_initialized);
        assert(state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_t transform;
        fix16_matrix4_identity(&transform);

        transform.frow[0][3] = x;
        transform.frow[1][3] = y;
        transform.frow[2][3] = z;

        fix16_matrix4_t matrix;
        fix16_matrix4_multiply(top_ms->ms_matrix, &transform, &matrix);

        matrix_stack_load(&matrix);
}

void
matrix_stack_scale(fix16_t x, fix16_t y, fix16_t z)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(state.mss_initialized);
        assert(state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(state.mss_mode);

#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_t transform;
        fix16_matrix4_identity(&transform);

        transform.frow[0][0] = x;
        transform.frow[1][1] = y;
        transform.frow[2][2] = z;

        fix16_matrix4_t matrix;
        fix16_matrix4_multiply(top_ms->ms_matrix, &transform, &matrix);

        matrix_stack_load(&matrix);
}

void
matrix_stack_orthographic_project(fix16_t left, fix16_t right, fix16_t top,
    fix16_t bottom, fix16_t near, fix16_t far)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(state.mss_initialized);
        assert(state.mss_mode != MATRIX_STACK_MODE_INVALID);
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(state.mss_mode);

#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_t transform;
        fix16_matrix4_identity(&transform);

#ifdef ASSERT
        assert(near > F16(0.0f));
#endif /* ASSERT */


        transform.frow[0][0] = fix16_div(F16(2.0f), fix16_sub(right, left));
        transform.frow[0][3] = fix16_div(-fix16_add(right, left), fix16_sub(right, left));
        transform.frow[1][1] = fix16_div(F16(2.0f), fix16_sub(top, bottom));
        transform.frow[1][3] = fix16_div(-fix16_add(top, bottom), fix16_sub(top, bottom));
        transform.frow[2][2] = fix16_div(F16(-2.0f), fix16_sub(far, near));
        transform.frow[2][3] = fix16_div(-fix16_add(far, near), fix16_sub(far, near));

        fix16_matrix4_t matrix;
        fix16_matrix4_multiply(top_ms->ms_matrix, &transform, &matrix);

        matrix_stack_load(&matrix);
}

void
matrix_stack_rotate(fix16_t angle, int32_t component)
{
#ifdef ASSERT
        /* Make sure the correct state is set */
        assert(state.mss_initialized);
        assert(state.mss_mode != MATRIX_STACK_MODE_INVALID);

        assert((component >= 0) && (component <= 2));
#endif /* ASSERT */

        struct matrix_stack *top_ms;
        top_ms = matrix_stack_top(state.mss_mode);
#ifdef ASSERT
        assert(top_ms != NULL);
#endif /* ASSERT */

        fix16_matrix4_t transform;
        fix16_matrix4_identity(&transform);

        fix16_t sin;
        sin = fix16_sin(fix16_deg_to_rad(angle));
        fix16_t cos;
        cos = fix16_cos(fix16_deg_to_rad(angle));

        switch (component) {
        case 0: /* X */
                transform.frow[1][1] = cos;
                transform.frow[1][2] = -sin;
                transform.frow[2][1] = sin;
                transform.frow[2][2] = cos;
                break;
        case 1: /* Y */
                transform.frow[0][0] = cos;
                transform.frow[0][2] = sin;
                transform.frow[2][0] = -sin;
                transform.frow[2][2] = cos;
                break;
        case 2: /* Z */
                transform.frow[0][0] = cos;
                transform.frow[0][1] = -sin;
                transform.frow[1][0] = sin;
                transform.frow[1][1] = cos;
                break;
        }

        fix16_matrix4_t matrix;
        fix16_matrix4_multiply(top_ms->ms_matrix, &transform, &matrix);

        matrix_stack_load(&matrix);
}
