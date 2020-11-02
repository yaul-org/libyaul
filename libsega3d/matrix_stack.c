#include <string.h>

#include "sega3d.h"

#include "sega3d-internal.h"

#define MATRIX_MAX (20)

typedef struct {
        uint8_t index; 
        MATRIX matrices[MATRIX_MAX];
} matrix_stack_t;

static matrix_stack_t _matrix_stack;

void
_internal_matrix_init(void)
{
        for (uint8_t i = 0; i < MATRIX_MAX; i++) {
                MATRIX *matrix;
                matrix = &_matrix_stack.matrices[i];

                (*matrix)[0][0] = 1;
                (*matrix)[0][1] = 0;
                (*matrix)[0][2] = 0;

                (*matrix)[1][0] = 0;
                (*matrix)[1][1] = 1;
                (*matrix)[1][2] = 0;

                (*matrix)[2][0] = 0;
                (*matrix)[2][1] = 0;
                (*matrix)[2][2] = 1;

                (*matrix)[3][0] = 0;
                (*matrix)[3][1] = 0;
                (*matrix)[3][2] = 1;
        }
}

void
sega3d_matrix_push(matrix_type_t matrix_type)
{
        assert(_matrix_stack.index < (MATRIX_MAX - 1));

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
