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
fix16_mat_zero(fix16_mat_t *m0)
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

        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr   = FIX16_ZERO;
}

void
fix16_mat_dup(const fix16_mat_t *m0, fix16_mat_t *result)
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

        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr++ = *arr_ptr++;
        *result_arr_ptr   = *arr_ptr;
}

void
fix16_mat_identity(fix16_mat_t *m0)
{
        fix16_t *arr_ptr;
        arr_ptr = m0->arr;

        *arr_ptr++ = FIX16_ONE;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;

        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ONE;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;

        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ZERO;
        *arr_ptr++ = FIX16_ONE;
        *arr_ptr   = FIX16_ZERO;
}

size_t
fix16_mat_str(const fix16_mat_t *m0, char *buffer, int32_t decimals)
{
        char *buffer_ptr;
        buffer_ptr = buffer;

        for (uint32_t i = 0; i < 3; i++) {
                *buffer++ = '|';
                buffer_ptr += fix16_vec4_str(&m0->row[i], buffer_ptr, decimals);
                *buffer_ptr++ = '|';
                *buffer_ptr++ = '\n';
        }
        *buffer_ptr = '\0';

        return (buffer_ptr - buffer);
}
