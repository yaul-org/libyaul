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
fix16_mat4_zero(fix16_mat4_t *m0)
{
        memset(&m0->arr[0], 0, sizeof(fix16_mat4_t));
}

void
fix16_mat4_dup(const fix16_mat4_t * restrict m0, fix16_mat4_t * restrict result)
{
        (void)memcpy(result, m0, sizeof(fix16_mat4_t));
}

void
fix16_mat4_identity(fix16_mat4_t *m0)
{
        fix16_mat4_zero(m0);

        m0->frow[0][0] = FIX16_ONE;
        m0->frow[1][1] = FIX16_ONE;
        m0->frow[2][2] = FIX16_ONE;
        m0->frow[3][3] = FIX16_ONE;
}

void
fix16_mat4_transpose(const fix16_mat4_t * restrict m0,
    fix16_mat4_t * restrict result)
{
        result->frow[0][0] = m0->frow[0][0];
        result->frow[0][1] = m0->frow[1][0];
        result->frow[0][2] = m0->frow[2][0];
        result->frow[0][3] = m0->frow[3][0];

        result->frow[1][0] = m0->frow[0][1];
        result->frow[1][1] = m0->frow[1][1];
        result->frow[1][2] = m0->frow[2][1];
        result->frow[1][3] = m0->frow[3][1];

        result->frow[2][0] = m0->frow[0][2];
        result->frow[2][1] = m0->frow[1][2];
        result->frow[2][2] = m0->frow[2][2];
        result->frow[2][3] = m0->frow[3][2];

        result->frow[3][0] = m0->frow[0][3];
        result->frow[3][1] = m0->frow[1][3];
        result->frow[3][2] = m0->frow[2][3];
        result->frow[3][3] = m0->frow[3][3];
}

void
fix16_mat4_str(const fix16_mat4_t *m0, char *buf, int decimals)
{
        /* Each row vector is 72 bytes. So,
         * (((72 + 2 + 1) * 4) + 1) (301) bytes is required */

        uint32_t row_idx;

        for (row_idx = 0; row_idx < 4; row_idx++) {
                char *buf_start;
                char *buf_end;

                *buf++ = '|';
                buf_start = buf;
                buf_end = buf;
                fix16_vec4_str(&m0->row[row_idx], buf, decimals);

                for (; *buf_end != '\0'; buf_end++);

                buf += buf_end - buf_start;
                *buf++ = '|';
                *buf++ = '\n';
        }

        *(--buf) = '\0';
}
