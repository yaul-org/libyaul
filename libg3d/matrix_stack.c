/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "g3d.h"

#include "g3d-internal.h"

static struct {
        FIXED *top_matrix;
} _state;

void
__matrix_init(void)
{
        _state.top_matrix = (FIXED *)&__state->matrices[0];

        for (uint8_t i = 0; i < MATRIX_STACK_MAX; i++) {
                g3d_matrix_identity(&__state->matrices[i]);
        }
}

void
g3d_matrix_identity(MATRIX *matrix)
{
        FIXED * const matrix_p = (FIXED *)matrix;

        (void)memset(matrix, 0, sizeof(MATRIX));

        matrix_p[M00] = toFIXED(1.0f);
        matrix_p[M11] = toFIXED(1.0f);
        matrix_p[M22] = toFIXED(1.0f);
}

void
g3d_matrix_push(g3d_matrix_type_t matrix_type)
{
        assert((MATRIX *)_state.top_matrix < &__state->matrices[MATRIX_STACK_MAX - 1]);

        _state.top_matrix += MTRX;

        if (matrix_type == G3D_MATRIX_TYPE_PUSH) {
                MATRIX * const dst_matrix = (MATRIX *)_state.top_matrix;
                MATRIX * const src_matrix = dst_matrix - 1;

                (void)memcpy(dst_matrix, src_matrix, sizeof(MATRIX));
        }
}

void
g3d_matrix_pop(void)
{
        if ((MATRIX *)_state.top_matrix != __state->matrices) {
                _state.top_matrix -= MTRX;
        }
}

const MATRIX *
g3d_matrix_top(void)
{
        return (MATRIX *)_state.top_matrix;
}

void
g3d_matrix_load(const MATRIX *matrix)
{
        assert(matrix != NULL);

        (void)memcpy(_state.top_matrix, matrix, sizeof(MATRIX));
}

void
g3d_matrix_copy(MATRIX *matrix)
{
        assert(matrix != NULL);

        (void)memcpy(matrix, _state.top_matrix, sizeof(MATRIX));
}

void
g3d_matrix_transpose_in(MATRIX *matrix)
{
        FIXED * const dst_matrix = (FIXED *)matrix;

        const FIXED m10 = dst_matrix[M10];
        const FIXED m20 = dst_matrix[M20];
        const FIXED m21 = dst_matrix[M21];

        dst_matrix[M10] = dst_matrix[M01];
        dst_matrix[M20] = dst_matrix[M02];

        dst_matrix[M01] = m10;
        dst_matrix[M21] = dst_matrix[M12];

        dst_matrix[M02] = m20;
        dst_matrix[M12] = m21;
}

void
g3d_matrix_trans(FIXED tx, FIXED ty, FIXED tz)
{
        FIXED * const top_matrix = _state.top_matrix;

        top_matrix[M03] += tx;
        top_matrix[M13] += ty;
        top_matrix[M23] += tz;
}

void
g3d_matrix_trans_load(FIXED tx, FIXED ty, FIXED tz)
{
        FIXED * const top_matrix = _state.top_matrix;

        top_matrix[M03] = tx;
        top_matrix[M13] = ty;
        top_matrix[M23] = tz;
}

void
g3d_matrix_trans_reset(void)
{
        FIXED * const top_matrix = _state.top_matrix;

        top_matrix[M03] = 0;
        top_matrix[M13] = 0;
        top_matrix[M23] = 0;
}

void
g3d_matrix_rot_load(const ANGLE rx, const ANGLE ry, const ANGLE rz)
{
        FIXED * const top_matrix = _state.top_matrix;

        const int32_t rx_bradians = fix16_int16_muls(rx, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sx = fix16_bradians_sin(rx_bradians);
        const FIXED cx = fix16_bradians_cos(rx_bradians);

        const int32_t ry_bradians = fix16_int16_muls(ry, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sy = fix16_bradians_sin(ry_bradians);
        const FIXED cy = fix16_bradians_cos(ry_bradians);

        const int32_t rz_bradians = fix16_int16_muls(rz, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sz = fix16_bradians_sin(rz_bradians);
        const FIXED cz = fix16_bradians_cos(rz_bradians);

        const FIXED sxsy = fix16_mul(sx, sy);
        const FIXED cxsy = fix16_mul(cx, sy);

        top_matrix[M00] = fix16_mul(   cy, cz);
        top_matrix[M01] = fix16_mul( sxsy, cz) + fix16_mul(cx, sz);
        top_matrix[M02] = fix16_mul(-cxsy, cz) + fix16_mul(sx, sz);
        top_matrix[M10] = fix16_mul(  -cy, sz);
        top_matrix[M11] = fix16_mul(-sxsy, sz) + fix16_mul(cx, cz);
        top_matrix[M12] = fix16_mul( cxsy, sz) + fix16_mul(sx, cz);
        top_matrix[M20] = sy;
        top_matrix[M21] = fix16_mul(  -sx, cy);
        top_matrix[M22] = fix16_mul(   cx, cy);
}

void
g3d_matrix_rot_x(const ANGLE angle)
{
        FIXED * const top_matrix = _state.top_matrix;

        const int32_t bradians = fix16_int16_muls(angle, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sin = fix16_bradians_sin(bradians);
        const FIXED cos = fix16_bradians_cos(bradians);

        const FIXED m01 = top_matrix[M01];
        const FIXED m02 = top_matrix[M02];
        const FIXED m11 = top_matrix[M11];
        const FIXED m12 = top_matrix[M12];
        const FIXED m21 = top_matrix[M21];
        const FIXED m22 = top_matrix[M22];

        top_matrix[M01] =  fix16_mul(m01, cos) + fix16_mul(m02, sin);
        top_matrix[M02] = -fix16_mul(m01, sin) + fix16_mul(m02, cos);
        top_matrix[M11] =  fix16_mul(m11, cos) + fix16_mul(m12, sin);
        top_matrix[M12] = -fix16_mul(m11, sin) + fix16_mul(m12, cos);
        top_matrix[M21] =  fix16_mul(m21, cos) + fix16_mul(m22, sin);
        top_matrix[M22] = -fix16_mul(m21, sin) + fix16_mul(m22, cos);
}

void
g3d_matrix_rot_y(const ANGLE angle)
{
        FIXED * const top_matrix = _state.top_matrix;

        const int32_t bradians = fix16_int16_muls(angle, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sin_value = fix16_bradians_sin(bradians);
        const FIXED cos_value = fix16_bradians_cos(bradians);

        const FIXED m00 = top_matrix[M00];
        const FIXED m02 = top_matrix[M02];
        const FIXED m10 = top_matrix[M10];
        const FIXED m12 = top_matrix[M12];
        const FIXED m20 = top_matrix[M20];
        const FIXED m22 = top_matrix[M22];

        top_matrix[M00] = fix16_mul(m00, cos_value) - fix16_mul(m02, sin_value);
        top_matrix[M02] = fix16_mul(m00, sin_value) + fix16_mul(m02, cos_value);
        top_matrix[M10] = fix16_mul(m10, cos_value) - fix16_mul(m12, sin_value);
        top_matrix[M12] = fix16_mul(m10, sin_value) + fix16_mul(m12, cos_value);
        top_matrix[M20] = fix16_mul(m20, cos_value) - fix16_mul(m22, sin_value);
        top_matrix[M22] = fix16_mul(m20, sin_value) + fix16_mul(m22, cos_value);
}

void
g3d_matrix_rot_z(const ANGLE angle)
{
        FIXED * const top_matrix = _state.top_matrix;

        const int32_t bradians = fix16_int16_muls(angle, FIX16(FIX16_LUT_SIN_TABLE_COUNT));
        const FIXED sin_value = fix16_bradians_sin(bradians);
        const FIXED cos_value = fix16_bradians_cos(bradians);

        const FIXED m00 = top_matrix[M00];
        const FIXED m01 = top_matrix[M01];
        const FIXED m10 = top_matrix[M10];
        const FIXED m11 = top_matrix[M11];
        const FIXED m20 = top_matrix[M20];
        const FIXED m21 = top_matrix[M21];

        top_matrix[M00] =  fix16_mul(m00, cos_value) + fix16_mul(m01, sin_value);
        top_matrix[M01] = -fix16_mul(m00, sin_value) + fix16_mul(m01, cos_value);
        top_matrix[M10] =  fix16_mul(m10, cos_value) + fix16_mul(m11, sin_value);
        top_matrix[M11] = -fix16_mul(m10, sin_value) + fix16_mul(m11, cos_value);
        top_matrix[M20] =  fix16_mul(m20, cos_value) + fix16_mul(m21, sin_value);
        top_matrix[M21] = -fix16_mul(m20, sin_value) + fix16_mul(m21, cos_value);
}

void
g3d_matrix_transpose(void)
{
        g3d_matrix_transpose_in((MATRIX *)_state.top_matrix);
}
