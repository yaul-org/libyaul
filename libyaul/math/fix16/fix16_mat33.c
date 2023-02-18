/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/map.h>
#include <string.h>

#include "fix16.h"

static void _mat33_row_transpose(const fix16_t *arr, fix16_vec3_t *m0);

void
fix16_mat33_dup(const fix16_mat33_t *m0, fix16_mat33_t *result)
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
fix16_mat33_identity(fix16_mat33_t *m0)
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
fix16_mat33_transpose(const fix16_mat33_t * __restrict m0, fix16_mat33_t * __restrict result)
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

void
fix16_mat33_mul(const fix16_mat33_t *m0, const fix16_mat33_t *m1, fix16_mat33_t *result)
{
    fix16_vec3_t transposed_row;

    const fix16_vec3_t * const m00 = &m0->row[0];
    const fix16_vec3_t * const m01 = &m0->row[1];
    const fix16_vec3_t * const m02 = &m0->row[2];

    _mat33_row_transpose(&m1->arr[0], &transposed_row);
    result->frow[0][0] = fix16_vec3_dot(m00, &transposed_row);
    result->frow[1][0] = fix16_vec3_dot(m01, &transposed_row);
    result->frow[2][0] = fix16_vec3_dot(m02, &transposed_row);

    _mat33_row_transpose(&m1->arr[1], &transposed_row);
    result->frow[0][1] = fix16_vec3_dot(m00, &transposed_row);
    result->frow[1][1] = fix16_vec3_dot(m01, &transposed_row);
    result->frow[2][1] = fix16_vec3_dot(m02, &transposed_row);

    _mat33_row_transpose(&m1->arr[2], &transposed_row);
    result->frow[0][2] = fix16_vec3_dot(m00, &transposed_row);
    result->frow[1][2] = fix16_vec3_dot(m01, &transposed_row);
    result->frow[2][2] = fix16_vec3_dot(m02, &transposed_row);
}

void
fix16_mat33_vec3_mul(const fix16_mat33_t *m0, const fix16_vec3_t *v, fix16_vec3_t *result)
{
    result->x = fix16_vec3_dot(&m0->row[0], v);
    result->y = fix16_vec3_dot(&m0->row[1], v);
    result->z = fix16_vec3_dot(&m0->row[2], v);
}

size_t
fix16_mat33_str(const fix16_mat33_t *m0, char *buffer, int32_t decimals)
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

void
fix16_mat33_zero(fix16_mat33_t *m0)
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

static void
_mat33_row_transpose(const fix16_t *arr, fix16_vec3_t *m0)
{
    m0->x = arr[0];
    m0->y = arr[3];
    m0->z = arr[6];
}
