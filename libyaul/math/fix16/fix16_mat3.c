/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <string.h>

#include "fix16.h"

void
fix16_mat3_zero(fix16_mat3_t *m0)
{
        fix16_t *arr_ptr;
        arr_ptr = m0->arr;

        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr   = FIX16_ZERO;
}

void
fix16_mat3_dup(const fix16_mat3_t *m0, fix16_mat3_t *result)
{
        const fix16_t *arr_ptr;
        arr_ptr = m0->arr;

        fix16_t *result_arr_ptr;
        result_arr_ptr = result->arr;

        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr++ = *arr_ptr++;

        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr++ = *arr_ptr++;

        *result_arr_ptr   = *arr_ptr;
}

void
fix16_mat3_identity(fix16_mat3_t *m0)
{
        fix16_t *arr_ptr;
        arr_ptr = m0->arr;

        *arr_ptr++ = FIX16_ONE;  /* M[0,0] (0) */
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ONE;  /* M[1,1] (4) */
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr   = FIX16_ONE;  /* M[2,2] (8) */
}

void
fix16_mat3_transpose(const fix16_mat3_t * __restrict m0, fix16_mat3_t * __restrict result)
{
        result->frow[0][0] = m0->frow[0][0];
        result->frow[0][1] = m0->frow[1][0];
        result->frow[0][2] = m0->frow[2][0];

        result->frow[1][0] = m0->frow[0][1];
        result->frow[1][1] = m0->frow[1][1];
        result->frow[1][2] = m0->frow[2][1];

        result->frow[2][0] = m0->frow[0][2];
        result->frow[2][1] = m0->frow[1][2];
        result->frow[2][2] = m0->frow[2][2];
}

size_t
fix16_mat3_str(const fix16_mat3_t *m0, char *buffer, int32_t decimals)
{
        char *buffer_ptr;
        buffer_ptr = buffer;

        for (uint32_t i = 0; i < 3; i++) {
                *buffer_ptr++ = '|';
                buffer_ptr += fix16_vec3_str(&m0->row[i], buffer_ptr, decimals);
                *buffer_ptr++ = '|';
                *buffer_ptr++ = '\n';
        }
        *buffer_ptr = '\0';

        return (buffer_ptr - buffer);
}
