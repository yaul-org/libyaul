/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <scu/map.h>
#include <string.h>

#include <gamemath/angle.h>
#include <gamemath/fix16/fix16_mat33.h>
#include <gamemath/fix16/fix16_trig.h>
#include <gamemath/math3d.h>

static void _mat33_row_transpose(const fix16_t *arr, fix16_vec3_t *m0);

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
fix16_mat33_transpose(const fix16_mat33_t * __restrict m0,
    fix16_mat33_t * __restrict result)
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
fix16_mat33_lookat(const fix16_vec3_t *from, const fix16_vec3_t *to,
  const fix16_vec3_t *up, fix16_mat33_t *result)
{
    const lookat_t lookat = {
        .from          = from,
        .to            = to,
        .up            = up,
        .basis_right   = &result->row[0],
        .basis_up      = &result->row[1],
        .basis_forward = &result->row[2],
    };

    math3d_lookat(&lookat);
}

void
fix16_mat33_mul(const fix16_mat33_t *m0, const fix16_mat33_t *m1,
  fix16_mat33_t *result)
{
    fix16_mat33_t transpose;
    fix16_mat33_transpose(m1, &transpose);

    result->frow[0][0] = fix16_vec3_dot(&m0->row[0], &transpose.row[0]);
    result->frow[0][1] = fix16_vec3_dot(&m0->row[0], &transpose.row[1]);
    result->frow[0][2] = fix16_vec3_dot(&m0->row[0], &transpose.row[2]);

    result->frow[1][0] = fix16_vec3_dot(&m0->row[1], &transpose.row[0]);
    result->frow[1][1] = fix16_vec3_dot(&m0->row[1], &transpose.row[1]);
    result->frow[1][2] = fix16_vec3_dot(&m0->row[1], &transpose.row[2]);

    result->frow[2][0] = fix16_vec3_dot(&m0->row[2], &transpose.row[0]);
    result->frow[2][1] = fix16_vec3_dot(&m0->row[2], &transpose.row[1]);
    result->frow[2][2] = fix16_vec3_dot(&m0->row[2], &transpose.row[2]);
}

void
fix16_mat33_vec3_mul(const fix16_mat33_t *m0, const fix16_vec3_t *v,
  fix16_vec3_t *result)
{
    result->x = fix16_vec3_dot(&m0->row[0], v);
    result->y = fix16_vec3_dot(&m0->row[1], v);
    result->z = fix16_vec3_dot(&m0->row[2], v);
}

void
fix16_mat33_x_rotate(const fix16_mat33_t *m0, angle_t angle,
    fix16_mat33_t *result)
{
    fix16_t sin_value;
    fix16_t cos_value;

    fix16_sincos(angle, &sin_value, &cos_value);

    const fix16_t m01 = m0->frow[0][1];
    const fix16_t m02 = m0->frow[0][2];
    const fix16_t m11 = m0->frow[1][1];
    const fix16_t m12 = m0->frow[1][2];
    const fix16_t m21 = m0->frow[2][1];
    const fix16_t m22 = m0->frow[2][2];

    result->frow[0][1] =  fix16_mul(m01, cos_value) + fix16_mul(m02, sin_value);
    result->frow[0][2] = -fix16_mul(m01, sin_value) + fix16_mul(m02, cos_value);

    result->frow[1][1] =  fix16_mul(m11, cos_value) + fix16_mul(m12, sin_value);
    result->frow[1][2] = -fix16_mul(m11, sin_value) + fix16_mul(m12, cos_value);

    result->frow[2][1] =  fix16_mul(m21, cos_value) + fix16_mul(m22, sin_value);
    result->frow[2][2] = -fix16_mul(m21, sin_value) + fix16_mul(m22, cos_value);
}

void
fix16_mat33_y_rotate(const fix16_mat33_t *m0, angle_t angle,
    fix16_mat33_t *result)
{
    fix16_t sin_value;
    fix16_t cos_value;

    fix16_sincos(angle, &sin_value, &cos_value);

    const fix16_t m00 = m0->frow[0][0];
    const fix16_t m02 = m0->frow[0][2];
    const fix16_t m10 = m0->frow[1][0];
    const fix16_t m12 = m0->frow[1][2];
    const fix16_t m20 = m0->frow[2][0];
    const fix16_t m22 = m0->frow[2][2];

    result->frow[0][0] = fix16_mul(m00, cos_value) - fix16_mul(m02, sin_value);
    result->frow[0][2] = fix16_mul(m00, sin_value) + fix16_mul(m02, cos_value);
    result->frow[1][0] = fix16_mul(m10, cos_value) - fix16_mul(m12, sin_value);
    result->frow[1][2] = fix16_mul(m10, sin_value) + fix16_mul(m12, cos_value);
    result->frow[2][0] = fix16_mul(m20, cos_value) - fix16_mul(m22, sin_value);
    result->frow[2][2] = fix16_mul(m20, sin_value) + fix16_mul(m22, cos_value);
}

void
fix16_mat33_z_rotate(const fix16_mat33_t *m0, angle_t angle,
    fix16_mat33_t *result)
{
    fix16_t sin_value;
    fix16_t cos_value;

    fix16_sincos(angle, &sin_value, &cos_value);

    const fix16_t m00 = m0->frow[0][0];
    const fix16_t m01 = m0->frow[0][1];
    const fix16_t m10 = m0->frow[1][0];
    const fix16_t m11 = m0->frow[1][1];
    const fix16_t m20 = m0->frow[2][0];
    const fix16_t m21 = m0->frow[2][1];

    result->frow[0][0] =  fix16_mul(m00, cos_value) + fix16_mul(m01, sin_value);
    result->frow[0][1] = -fix16_mul(m00, sin_value) + fix16_mul(m01, cos_value);
    result->frow[1][0] =  fix16_mul(m10, cos_value) + fix16_mul(m11, sin_value);
    result->frow[1][1] = -fix16_mul(m10, sin_value) + fix16_mul(m11, cos_value);
    result->frow[2][0] =  fix16_mul(m20, cos_value) + fix16_mul(m21, sin_value);
    result->frow[2][1] = -fix16_mul(m20, sin_value) + fix16_mul(m21, cos_value);
}

void
fix16_mat33_rotation_create(angle_t rx, angle_t ry, angle_t rz,
    fix16_mat33_t *result)
{
    fix16_t as;
    fix16_t ac;

    fix16_sincos(rx, &as, &ac);

    fix16_t bs;
    fix16_t bc;

    fix16_sincos(ry, &bs, &bc);

    fix16_t cs;
    fix16_t cc;

    fix16_sincos(rz, &cs, &cc);

    const fix16_mat33_t mat_a = {
        {  FIX16_ONE, FIX16_ZERO, FIX16_ZERO,
          FIX16_ZERO,         ac,         as,
          FIX16_ZERO,        -as,         ac }
    };

    const fix16_mat33_t mat_b = {
        {         bc, FIX16_ZERO,        -bs,
          FIX16_ZERO,  FIX16_ONE, FIX16_ZERO,
                  bs, FIX16_ZERO,         bc }
    };

    fix16_mat33_t mat_tmp;

    fix16_mat33_mul(&mat_a, &mat_b, &mat_tmp);

    const fix16_mat33_t mat_c = {
        {         cc,         cs, FIX16_ZERO,
                 -cs,         cc, FIX16_ZERO,
          FIX16_ZERO, FIX16_ZERO,  FIX16_ONE }
    };

    fix16_mat33_mul(&mat_tmp, &mat_c, result);
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

static void
_mat33_row_transpose(const fix16_t *arr, fix16_vec3_t *m0)
{
    m0->x = arr[0];
    m0->y = arr[3];
    m0->z = arr[6];
}
