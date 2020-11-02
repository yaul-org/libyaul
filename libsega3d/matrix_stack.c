#include <string.h>

#include "sega3d.h"

#include "sega3d-internal.h"

#define MATRIX_MAX (20)

static struct {
        uint8_t index; 
        MATRIX matrices[MATRIX_MAX];
} _state;

void
sega3d_matrix_push(matrix_type_t matrix_type)
{
        assert(_state.index < (MATRIX_MAX - 1));

        MATRIX *src_matrix;
        src_matrix = &_state.matrices[_state.index];

        _state.index++;

        if (matrix_type == MATRIX_TYPE_PUSH) {
                MATRIX *dst_matrix;
                dst_matrix = &_state.matrices[_state.index];

                (void)memcpy(dst_matrix, src_matrix, sizeof(MATRIX));
        }
}

void
sega3d_matrix_pop(void)
{
        assert(_state.index > 0);

        _state.index--;
}

void
sega3d_matrix_load(const MATRIX *matrix)
{
        assert(matrix != NULL);

        (void)memcpy(&_state.matrices[0], matrix, sizeof(MATRIX));
}

void
sega3d_matrix_copy(void)
{
        assert(_state.index >= 1);

        MATRIX *src_matrix;
        src_matrix = &_state.matrices[_state.index - 1];

        MATRIX *dst_matrix;
        dst_matrix = &_state.matrices[_state.index];

        (void)memcpy(dst_matrix, src_matrix, sizeof(MATRIX));
}

void
sega3d_matrix_translate(FIXED tx, FIXED ty, FIXED tz)
{
        MATRIX *matrix;
        matrix = &_state.matrices[_state.index];

        (*matrix)[3][0] = tx;
        (*matrix)[3][1] = ty;
        (*matrix)[3][2] = tz;
}

void
sega3d_matrix_scale(FIXED sx, FIXED sy, FIXED sz)
{
        MATRIX *matrix;
        matrix = &_state.matrices[_state.index];

        (*matrix)[0][0] = sx;
        (*matrix)[1][1] = sy;
        (*matrix)[2][2] = sz;
}
