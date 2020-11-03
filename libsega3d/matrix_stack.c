#include <string.h>

#include "sega3d.h"

#include "sega3d-internal.h"


typedef struct {
        uint8_t index; 
        MATRIX matrices[MATRIX_STACK_MAX];
} matrix_stack_t;

static matrix_stack_t _matrix_stack;

void
_internal_matrix_init(void)
{
        for (uint8_t i = 0; i < MATRIX_STACK_MAX; i++) {
                MATRIX *matrix;
                matrix = &_matrix_stack.matrices[i];

                (*matrix)[0][0] = toFIXED(1.0f);
                (*matrix)[0][1] = toFIXED(0.0f);
                (*matrix)[0][2] = toFIXED(0.0f);

                (*matrix)[1][0] = toFIXED(0.0f);
                (*matrix)[1][1] = toFIXED(1.0f);
                (*matrix)[1][2] = toFIXED(0.0f);

                (*matrix)[2][0] = toFIXED(0.0f);
                (*matrix)[2][1] = toFIXED(0.0f);
                (*matrix)[2][2] = toFIXED(1.0f);

                (*matrix)[3][0] = toFIXED(0.0f);
                (*matrix)[3][1] = toFIXED(0.0f);
                (*matrix)[3][2] = toFIXED(1.0f);
        }
}

void
sega3d_matrix_push(matrix_type_t matrix_type)
{
        assert(_matrix_stack.index < (MATRIX_STACK_MAX - 1));

        MATRIX *src_matrix;
        src_matrix = &_matrix_stack.matrices[_matrix_stack.index];

        _matrix_stack.index++;

        if (matrix_type == MATRIX_TYPE_PUSH) {
                MATRIX *dst_matrix;
                dst_matrix = &_matrix_stack.matrices[_matrix_stack.index];

                (void)memcpy(dst_matrix, src_matrix, sizeof(MATRIX));
        }
}

void
sega3d_matrix_pop(void)
{
        assert(_matrix_stack.index > 0);

        _matrix_stack.index--;
}

const MATRIX *
sega3d_matrix_top(void)
{
        return &_matrix_stack.matrices[_matrix_stack.index];
}

void
sega3d_matrix_load(const MATRIX *matrix)
{
        assert(matrix != NULL);

        (void)memcpy(&_matrix_stack.matrices[0], matrix, sizeof(MATRIX));
}

void
sega3d_matrix_copy(void)
{
        assert(_matrix_stack.index >= 1);

        MATRIX *src_matrix;
        src_matrix = &_matrix_stack.matrices[_matrix_stack.index - 1];

        MATRIX *dst_matrix;
        dst_matrix = &_matrix_stack.matrices[_matrix_stack.index];

        (void)memcpy(dst_matrix, src_matrix, sizeof(MATRIX));
}

void
sega3d_matrix_translate(FIXED tx, FIXED ty, FIXED tz)
{
        MATRIX *matrix;
        matrix = &_matrix_stack.matrices[_matrix_stack.index];

        (*matrix)[3][0] = tx;
        (*matrix)[3][1] = ty;
        (*matrix)[3][2] = tz;
}

void
sega3d_matrix_scale(FIXED sx, FIXED sy, FIXED sz)
{
        MATRIX *matrix;
        matrix = &_matrix_stack.matrices[_matrix_stack.index];

        (*matrix)[0][0] = sx;
        (*matrix)[1][1] = sy;
        (*matrix)[2][2] = sz;
}

void
sega3d_matrix_rotate_x(const ANGLE angle)
{
        FIXED *matrix;
        matrix = (FIXED *)&_matrix_stack.matrices[_matrix_stack.index];

        const int32_t bradians = fix16_int32_to(FIX16_LUT_TABLE_COUNT * angle);
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
        matrix = (FIXED *)&_matrix_stack.matrices[_matrix_stack.index];

        const int32_t bradians = fix16_int32_to(FIX16_LUT_TABLE_COUNT * angle);
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
        matrix = (FIXED *)&_matrix_stack.matrices[_matrix_stack.index];

        const int32_t bradians = fix16_int32_to(FIX16_LUT_TABLE_COUNT * angle);
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
