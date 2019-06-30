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
        memset(&m0->arr[0], 0, sizeof(fix16_t) * 4 * 4);
}

void
fix16_mat4_dup(const fix16_mat4_t *m0, fix16_mat4_t *result)
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
fix16_mat4_multiply(const fix16_mat4_t *m0, const fix16_mat4_t *m1,
                       fix16_mat4_t *result)
{
#define DOT_PRODUCT(i, j) fix16_add(                                           \
                fix16_add(fix16_mul(m0->frow[i][0], m1->frow[0][j]),           \
                    fix16_mul(m0->frow[i][1], m1->frow[1][j])),                \
                fix16_add(fix16_mul(m0->frow[i][2], m1->frow[2][j]),           \
                    fix16_mul(m0->frow[i][3], m1->frow[3][j])))

        result->frow[0][0] = DOT_PRODUCT(0, 0);
        result->frow[0][1] = DOT_PRODUCT(0, 1);
        result->frow[0][2] = DOT_PRODUCT(0, 2);
        result->frow[0][3] = DOT_PRODUCT(0, 3);

        result->frow[1][0] = DOT_PRODUCT(1, 0);
        result->frow[1][1] = DOT_PRODUCT(1, 1);
        result->frow[1][2] = DOT_PRODUCT(1, 2);
        result->frow[1][3] = DOT_PRODUCT(1, 3);

        result->frow[2][0] = DOT_PRODUCT(2, 0);
        result->frow[2][1] = DOT_PRODUCT(2, 1);
        result->frow[2][2] = DOT_PRODUCT(2, 2);
        result->frow[2][3] = DOT_PRODUCT(2, 3);

        result->frow[3][0] = DOT_PRODUCT(3, 0);
        result->frow[3][1] = DOT_PRODUCT(3, 1);
        result->frow[3][2] = DOT_PRODUCT(3, 2);
        result->frow[3][3] = DOT_PRODUCT(3, 3);
}

void
fix16_mat4_multiply2(const fix16_mat4_t *m0, const fix16_mat4_t *m1,
                        fix16_mat4_t *result)
{
#define DOT_PRODUCT(i, j) fix16_add(                                           \
                fix16_add(fix16_mul(m0->frow[i][0], m1->frow[0][j]),           \
                    fix16_mul(m0->frow[i][1], m1->frow[1][j])),                \
                fix16_add(fix16_mul(m0->frow[i][2], m1->frow[2][j]),           \
                    fix16_mul(m0->frow[i][3], m1->frow[3][j])))

        result->frow[0][0] = DOT_PRODUCT(0, 0);
        result->frow[0][1] = DOT_PRODUCT(0, 1);
        result->frow[0][2] = DOT_PRODUCT(0, 2);
        result->frow[0][3] = FIX16(0.0f);

        result->frow[1][0] = DOT_PRODUCT(1, 0);
        result->frow[1][1] = DOT_PRODUCT(1, 1);
        result->frow[1][2] = DOT_PRODUCT(1, 2);
        result->frow[1][3] = FIX16(0.0f);

        result->frow[2][0] = DOT_PRODUCT(2, 0);
        result->frow[2][1] = DOT_PRODUCT(2, 1);
        result->frow[2][2] = DOT_PRODUCT(2, 2);
        result->frow[2][3] = FIX16(0.0f);

        result->frow[3][0] = DOT_PRODUCT(3, 0);
        result->frow[3][1] = DOT_PRODUCT(3, 1);
        result->frow[3][2] = DOT_PRODUCT(3, 2);
        result->frow[3][3] = FIX16_ONE;
}

void
fix16_mat4_transpose(const fix16_mat4_t *m0, fix16_mat4_t *result)
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
fix16_mat4_inverse(const fix16_mat4_t *m0, fix16_mat4_t *result)
{
        /* Only applicable to affine transformation matrix */

        /* Last row: [0 0 0 1] */
        result->frow[3][0] = FIX16(0.0f);
        result->frow[3][1] = FIX16(0.0f);
        result->frow[3][2] = FIX16(0.0f);
        result->frow[3][3] = FIX16_ONE;

        /* Transpose 3x3 sub-mat */
        result->frow[0][0] = m0->frow[0][0];
        result->frow[0][1] = m0->frow[1][0];
        result->frow[0][2] = m0->frow[2][0];

        result->frow[1][0] = m0->frow[0][1];
        result->frow[1][1] = m0->frow[1][1];
        result->frow[1][2] = m0->frow[2][1];

        result->frow[2][0] = m0->frow[0][2];
        result->frow[2][1] = m0->frow[1][2];
        result->frow[2][2] = m0->frow[2][2];

        /* -(M^T)b */
        fix16_t x;
        x = m0->frow[0][3];
        fix16_t y;
        y = m0->frow[1][3];
        fix16_t z;
        z = m0->frow[2][3];

        result->frow[0][3] = -fix16_add(fix16_add(
                                                fix16_mul(result->frow[0][0], x),
                                                fix16_mul(result->frow[0][1], y)),
                                        fix16_mul(result->frow[0][2], z));
        result->frow[1][3] = -fix16_add(fix16_add(
                                                fix16_mul(result->frow[1][0], x),
                                                fix16_mul(result->frow[1][1], y)),
                                        fix16_mul(result->frow[1][2], z));
        result->frow[2][3] = -fix16_add(fix16_add(
                                                fix16_mul(result->frow[2][0], x),
                                                fix16_mul(result->frow[2][1], y)),
                                        fix16_mul(result->frow[2][2], z));
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
