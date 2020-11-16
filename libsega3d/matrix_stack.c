/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "sega3d.h"

#include "sega3d-internal.h"

static struct {
        FIXED *top_matrix;
} _state;

void
_internal_matrix_init(void)
{
        _state.top_matrix = (FIXED *)&_internal_state->matrices[0];

        for (uint8_t i = 0; i < MATRIX_STACK_MAX; i++) {
                FIXED *matrix;
                matrix = (FIXED *)&_internal_state->matrices[i];

                (void)memset(matrix, 0, sizeof(MATRIX));

                matrix[M00] = toFIXED(1.0f);
                matrix[M11] = toFIXED(1.0f);
                matrix[M22] = toFIXED(1.0f);
        }
}

void
sega3d_matrix_push(matrix_type_t matrix_type)
{
        assert((MATRIX *)_state.top_matrix < &_internal_state->matrices[MATRIX_STACK_MAX - 1]);

        _state.top_matrix += MTRX;

        if (matrix_type == MATRIX_TYPE_PUSH) {
                MATRIX * const dst_matrix = (MATRIX *)_state.top_matrix;
                MATRIX * const src_matrix = dst_matrix - 1;

                (void)memcpy(dst_matrix, src_matrix, sizeof(MATRIX));
        }
}

void
sega3d_matrix_pop(void)
{
        if ((MATRIX *)_state.top_matrix != _internal_state->matrices) {
                _state.top_matrix -= MTRX;
        }
}

const MATRIX *
sega3d_matrix_top(void)
{
        return (MATRIX *)_state.top_matrix;
}

void
sega3d_matrix_load(const MATRIX *matrix)
{
        assert(matrix != NULL);

        (void)memcpy(_state.top_matrix, matrix, sizeof(MATRIX));
}

void
sega3d_matrix_copy(MATRIX *matrix)
{
        assert(matrix != NULL);

        (void)memcpy(matrix, _state.top_matrix, sizeof(MATRIX));
}

void
sega3d_matrix_translate(FIXED tx, FIXED ty, FIXED tz)
{
        FIXED * const top_matrix = _state.top_matrix;

        register FIXED *dst_matrix;
        dst_matrix = &top_matrix[M03];

        register const FIXED *src_matrix = (const FIXED *)&top_matrix[M00];

        const fix16_vec3_t t = {
                .x = -tx,
                .y = -ty,
                .z = -tz
        };

        *dst_matrix = fix16_vec3_dot(&t, (const fix16_vec3_t *)src_matrix);
        src_matrix += 4;
        dst_matrix += 4;
        *dst_matrix = fix16_vec3_dot(&t, (const fix16_vec3_t *)src_matrix);
        src_matrix += 4;
        dst_matrix += 4;
        *dst_matrix = fix16_vec3_dot(&t, (const fix16_vec3_t *)src_matrix);
}

void
sega3d_matrix_rotate_x(const ANGLE angle)
{
        FIXED * const matrix = _state.top_matrix;

        const int32_t bradians = fix16_int16_muls(angle, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sin = fix16_bradians_sin(bradians);
        const FIXED cos = fix16_bradians_cos(bradians);

        const FIXED m01 = matrix[M01];
        const FIXED m02 = matrix[M02];
        const FIXED m11 = matrix[M11];
        const FIXED m12 = matrix[M12];
        const FIXED m21 = matrix[M21];
        const FIXED m22 = matrix[M22];

        matrix[M01] =  fix16_mul(m01, cos) + fix16_mul(m02, sin);
        matrix[M02] = -fix16_mul(m01, sin) + fix16_mul(m02, cos);
        matrix[M11] =  fix16_mul(m11, cos) + fix16_mul(m12, sin);
        matrix[M12] = -fix16_mul(m11, sin) + fix16_mul(m12, cos);
        matrix[M21] =  fix16_mul(m21, cos) + fix16_mul(m22, sin);
        matrix[M22] = -fix16_mul(m21, sin) + fix16_mul(m22, cos);
}

void
sega3d_matrix_rotate_y(const ANGLE angle)
{
        FIXED * const matrix = _state.top_matrix;

        const int32_t bradians = fix16_int16_muls(angle, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sin_value = fix16_bradians_sin(bradians);
        const FIXED cos_value = fix16_bradians_cos(bradians);

        const FIXED m00 = matrix[M00];
        const FIXED m02 = matrix[M02];
        const FIXED m10 = matrix[M10];
        const FIXED m12 = matrix[M12];
        const FIXED m20 = matrix[M20];
        const FIXED m22 = matrix[M22];

        matrix[M00] = fix16_mul(m00, cos_value) - fix16_mul(m02, sin_value);
        matrix[M02] = fix16_mul(m00, sin_value) + fix16_mul(m02, cos_value);
        matrix[M10] = fix16_mul(m10, cos_value) - fix16_mul(m12, sin_value);
        matrix[M12] = fix16_mul(m10, sin_value) + fix16_mul(m12, cos_value);
        matrix[M20] = fix16_mul(m20, cos_value) - fix16_mul(m22, sin_value);
        matrix[M22] = fix16_mul(m20, sin_value) + fix16_mul(m22, cos_value);
}

void
sega3d_matrix_rotate_z(const ANGLE angle)
{
        FIXED * const matrix = _state.top_matrix;

        const int32_t bradians = fix16_int16_muls(angle, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sin_value = fix16_bradians_sin(bradians);
        const FIXED cos_value = fix16_bradians_cos(bradians);

        const FIXED m00 = matrix[M00];
        const FIXED m01 = matrix[M01];
        const FIXED m10 = matrix[M10];
        const FIXED m11 = matrix[M11];
        const FIXED m20 = matrix[M20];
        const FIXED m21 = matrix[M21];

        matrix[M00] =  fix16_mul(m00, cos_value) + fix16_mul(m01, sin_value);
        matrix[M01] = -fix16_mul(m00, sin_value) + fix16_mul(m01, cos_value);
        matrix[M10] =  fix16_mul(m10, cos_value) + fix16_mul(m11, sin_value);
        matrix[M11] = -fix16_mul(m10, sin_value) + fix16_mul(m11, cos_value);
        matrix[M20] =  fix16_mul(m20, cos_value) + fix16_mul(m21, sin_value);
        matrix[M21] = -fix16_mul(m20, sin_value) + fix16_mul(m21, cos_value);
}
