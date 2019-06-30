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
        (void)memset(&m0->arr[0], 0, sizeof(fix16_t) * 3 * 3);
}

void
fix16_mat3_dup(const fix16_mat3_t *m0, fix16_mat3_t *result)
{
        (void)memcpy(result, m0, sizeof(fix16_mat3_t));
}

void
fix16_mat3_identity(fix16_mat3_t *m0)
{
        fix16_mat3_zero(m0);

        m0->frow[0][0] = FIX16_ONE;
        m0->frow[1][1] = FIX16_ONE;
        m0->frow[2][2] = FIX16_ONE;
}

void
fix16_mat3_multiply(const fix16_mat3_t *m0, const fix16_mat3_t *m1,
    fix16_mat3_t *result)
{
#define DOT_PRODUCT(i, j)                                                      \
        fix16_add(fix16_add(fix16_mul(m0->frow[i][0], m1->frow[0][j]),         \
                            fix16_mul(m0->frow[i][1], m1->frow[1][j])),        \
                  fix16_mul(m0->frow[i][2], m1->frow[2][j]))

        result->frow[0][0] = DOT_PRODUCT(0, 0);
        result->frow[0][1] = DOT_PRODUCT(0, 1);
        result->frow[0][2] = DOT_PRODUCT(0, 2);

        result->frow[1][0] = DOT_PRODUCT(1, 0);
        result->frow[1][1] = DOT_PRODUCT(1, 1);
        result->frow[1][2] = DOT_PRODUCT(1, 2);

        result->frow[2][0] = DOT_PRODUCT(2, 0);
        result->frow[2][1] = DOT_PRODUCT(2, 1);
        result->frow[2][2] = DOT_PRODUCT(2, 2);
}

void
fix16_mat3_transpose(const fix16_mat3_t *m0, fix16_mat3_t *result)
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
fix16_mat3_inverse(const fix16_mat3_t *m0 __unused,
    fix16_mat3_t *result __unused)
{
}

void
fix16_mat3_str(const fix16_mat3_t *m0, char *buf, int decimals)
{
        uint32_t row_idx;
        for (row_idx = 0; row_idx < 3; row_idx++) {
                char *buf_start;
                char *buf_end;

                *buf++ = '|';
                buf_start = buf;
                buf_end = buf;
                fix16_vec3_str(&m0->row[row_idx], buf, decimals);
                for (; *buf_end != '\0'; buf_end++);
                buf += buf_end - buf_start;
                *buf++ = '|';
                *buf++ = '\n';
        }

        *(--buf) = '\0';
}
