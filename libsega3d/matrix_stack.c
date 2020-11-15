#include <string.h>

#include "sega3d.h"

#include "sega3d-internal.h"

typedef struct {
        uint8_t index;
} __aligned(16) matrix_stack_t;

static matrix_stack_t _matrix_stack = {
        .index = 0
};

void
_internal_matrix_init(void)
{
        for (uint8_t i = 0; i < MATRIX_STACK_MAX; i++) {
                FIXED *matrix;
                matrix = (FIXED *)&_internal_state.matrices[i];

                (void)memset(matrix, 0, sizeof(MATRIX));

                matrix[M00] = toFIXED(1.0f);
                matrix[M11] = toFIXED(1.0f);
                matrix[M22] = toFIXED(1.0f);
        }
}

void
sega3d_matrix_push(matrix_type_t matrix_type)
{
        assert(_matrix_stack.index < (MATRIX_STACK_MAX - 1));

        _matrix_stack.index++;

        if (matrix_type == MATRIX_TYPE_PUSH) {
                MATRIX *dst_matrix;
                dst_matrix = &_internal_state.matrices[_matrix_stack.index];

                MATRIX *src_matrix;
                src_matrix = dst_matrix - 1;

                (void)memcpy(dst_matrix, src_matrix, sizeof(MATRIX));
        }
}

void
sega3d_matrix_pop(void)
{
        if (_matrix_stack.index > 0) {
                _matrix_stack.index--;
        }
}

const MATRIX *
sega3d_matrix_top(void)
{
        return &_internal_state.matrices[_matrix_stack.index];
}

void
sega3d_matrix_load(const MATRIX *matrix)
{
        assert(matrix != NULL);

        (void)memcpy(&_internal_state.matrices[0], matrix, sizeof(MATRIX));
}

void
sega3d_matrix_copy(MATRIX *matrix)
{
        MATRIX *src_matrix;
        src_matrix = &_internal_state.matrices[_matrix_stack.index];

        (void)memcpy(matrix, src_matrix, sizeof(MATRIX));
}

void
sega3d_matrix_inverse_push(void)
{
        int8_t index;
        index = _matrix_stack.index - 1;

        if (index < 0) {
                index = 0;
        }

        FIXED *src_matrix;
        src_matrix = (FIXED *)&_internal_state.matrices[_matrix_stack.index];

        sega3d_matrix_push(MATRIX_TYPE_MOVE_PTR);

        FIXED *dst_matrix;
        dst_matrix = (FIXED *)&_internal_state.matrices[_matrix_stack.index];

        dst_matrix[M00] =  src_matrix[M00];
        dst_matrix[M10] =  src_matrix[M01];
        dst_matrix[M20] =  src_matrix[M02];
        dst_matrix[M03] = -src_matrix[M03];

        dst_matrix[M01] =  src_matrix[M10];
        dst_matrix[M11] =  src_matrix[M11];
        dst_matrix[M21] =  src_matrix[M12];
        dst_matrix[M13] = -src_matrix[M13];

        dst_matrix[M02] =  src_matrix[M20];
        dst_matrix[M12] =  src_matrix[M21];
        dst_matrix[M22] =  src_matrix[M22];
        dst_matrix[M23] = -src_matrix[M23];
}

void
sega3d_matrix_translate(FIXED tx, FIXED ty, FIXED tz)
{
        FIXED *matrix;
        matrix = (FIXED *)&_internal_state.matrices[_matrix_stack.index];

        fix16_vec3_t t;

        t.x = -tx;
        t.y = -ty;
        t.z = -tz;

        matrix[M03] = fix16_vec3_dot(&t, (const fix16_vec3_t *)&matrix[M00]);
        matrix[M13] = fix16_vec3_dot(&t, (const fix16_vec3_t *)&matrix[M10]);
        matrix[M23] = fix16_vec3_dot(&t, (const fix16_vec3_t *)&matrix[M20]);
}

void
sega3d_matrix_rotate_x(const ANGLE angle)
{
        FIXED *matrix;
        matrix = (FIXED *)&_internal_state.matrices[_matrix_stack.index];

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
        FIXED *matrix;
        matrix = (FIXED *)&_internal_state.matrices[_matrix_stack.index];

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
        FIXED *matrix;
        matrix = (FIXED *)&_internal_state.matrices[_matrix_stack.index];

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
