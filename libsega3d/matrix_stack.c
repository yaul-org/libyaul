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
