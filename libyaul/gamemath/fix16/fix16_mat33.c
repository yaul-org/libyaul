/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Mattias Jansson
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <scu/map.h>

#include <gamemath/angle.h>
#include <gamemath/fix16/fix16_mat33.h>
#include <gamemath/fix16/fix16_trig.h>
#include <gamemath/math3d.h>

void
fix16_mat33_zero(fix16_mat33_t *m0)
{
    fix16_t *arr_ptr;
    arr_ptr = (fix16_t *)m0;

    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr   = FIX16(0.0);
}

void
fix16_mat33_identity(fix16_mat33_t *m0)
{
    fix16_t *arr_ptr;
    arr_ptr = (fix16_t *)m0;

    *arr_ptr++ = FIX16(1.0);  /* M[0,0] (0) */
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(1.0);  /* M[1,1] (4) */
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr++ = FIX16(0.0);
    *arr_ptr   = FIX16(1.0);  /* M[2,2] (8) */
}

void
fix16_mat33_dup(const fix16_mat33_t *m0, fix16_mat33_t *result)
{
    const fix16_t *arr_ptr;
    arr_ptr = (fix16_t *)m0;

    fix16_t *result_arr_ptr;
    result_arr_ptr = (fix16_t *)result;

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
    assert(m0 != NULL);
    assert(result != NULL);
    assert(m0 != result);

    result->row[0].x = m0->row[0].x;
    result->row[0].y = m0->row[1].x;
    result->row[0].z = m0->row[2].x;

    result->row[1].x = m0->row[0].y;
    result->row[1].y = m0->row[1].y;
    result->row[1].z = m0->row[2].y;

    result->row[2].x = m0->row[0].z;
    result->row[2].y = m0->row[1].z;
    result->row[2].z = m0->row[2].z;
}

void
fix16_mat33_inplace_transpose(fix16_mat33_t *m0)
{
    assert(m0 != NULL);

    const fix16_t m01 = m0->row[0].y;
    const fix16_t m02 = m0->row[0].z;

    m0->row[0].y = m0->row[1].x;
    m0->row[0].z = m0->row[2].x;

    const fix16_t m12 = m0->row[1].z;

    m0->row[1].x = m01;
    m0->row[1].z = m0->row[2].y;

    m0->row[2].x = m02;
    m0->row[2].y = m12;
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

    result->row[0].x = fix16_vec3_dot(&m0->row[0], &transpose.row[0]);
    result->row[0].y = fix16_vec3_dot(&m0->row[0], &transpose.row[1]);
    result->row[0].z = fix16_vec3_dot(&m0->row[0], &transpose.row[2]);

    result->row[1].x = fix16_vec3_dot(&m0->row[1], &transpose.row[0]);
    result->row[1].y = fix16_vec3_dot(&m0->row[1], &transpose.row[1]);
    result->row[1].z = fix16_vec3_dot(&m0->row[1], &transpose.row[2]);

    result->row[2].x = fix16_vec3_dot(&m0->row[2], &transpose.row[0]);
    result->row[2].y = fix16_vec3_dot(&m0->row[2], &transpose.row[1]);
    result->row[2].z = fix16_vec3_dot(&m0->row[2], &transpose.row[2]);
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

    const fix16_t m01 = m0->row[0].y;
    const fix16_t m02 = m0->row[0].z;
    const fix16_t m11 = m0->row[1].y;
    const fix16_t m12 = m0->row[1].z;
    const fix16_t m21 = m0->row[2].y;
    const fix16_t m22 = m0->row[2].z;

    result->row[0].y =  fix16_mul(m01, cos_value) + fix16_mul(m02, sin_value);
    result->row[0].z = -fix16_mul(m01, sin_value) + fix16_mul(m02, cos_value);

    result->row[1].y =  fix16_mul(m11, cos_value) + fix16_mul(m12, sin_value);
    result->row[1].z = -fix16_mul(m11, sin_value) + fix16_mul(m12, cos_value);

    result->row[2].y =  fix16_mul(m21, cos_value) + fix16_mul(m22, sin_value);
    result->row[2].z = -fix16_mul(m21, sin_value) + fix16_mul(m22, cos_value);
}

void
fix16_mat33_y_rotate(const fix16_mat33_t *m0, angle_t angle,
    fix16_mat33_t *result)
{
    fix16_t sin_value;
    fix16_t cos_value;

    fix16_sincos(angle, &sin_value, &cos_value);

    const fix16_t m00 = m0->row[0].x;
    const fix16_t m02 = m0->row[0].z;
    const fix16_t m10 = m0->row[1].x;
    const fix16_t m12 = m0->row[1].z;
    const fix16_t m20 = m0->row[2].x;
    const fix16_t m22 = m0->row[2].z;

    result->row[0].x = fix16_mul(m00, cos_value) - fix16_mul(m02, sin_value);
    result->row[0].z = fix16_mul(m00, sin_value) + fix16_mul(m02, cos_value);
    result->row[1].x = fix16_mul(m10, cos_value) - fix16_mul(m12, sin_value);
    result->row[1].z = fix16_mul(m10, sin_value) + fix16_mul(m12, cos_value);
    result->row[2].x = fix16_mul(m20, cos_value) - fix16_mul(m22, sin_value);
    result->row[2].z = fix16_mul(m20, sin_value) + fix16_mul(m22, cos_value);
}

void
fix16_mat33_z_rotate(const fix16_mat33_t *m0, angle_t angle,
    fix16_mat33_t *result)
{
    fix16_t sin_value;
    fix16_t cos_value;

    fix16_sincos(angle, &sin_value, &cos_value);

    const fix16_t m00 = m0->row[0].x;
    const fix16_t m01 = m0->row[0].y;
    const fix16_t m10 = m0->row[1].x;
    const fix16_t m11 = m0->row[1].y;
    const fix16_t m20 = m0->row[2].x;
    const fix16_t m21 = m0->row[2].y;

    result->row[0].x =  fix16_mul(m00, cos_value) + fix16_mul(m01, sin_value);
    result->row[0].y = -fix16_mul(m00, sin_value) + fix16_mul(m01, cos_value);
    result->row[1].x =  fix16_mul(m10, cos_value) + fix16_mul(m11, sin_value);
    result->row[1].y = -fix16_mul(m10, sin_value) + fix16_mul(m11, cos_value);
    result->row[2].x =  fix16_mul(m20, cos_value) + fix16_mul(m21, sin_value);
    result->row[2].y = -fix16_mul(m20, sin_value) + fix16_mul(m21, cos_value);
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
        {{ FIX16(1.0), FIX16(0.0), FIX16(0.0) },
         { FIX16(0.0),         ac,         as },
         { FIX16(0.0),        -as,         ac }}
    };

    const fix16_mat33_t mat_b = {
        {{         bc, FIX16(0.0),        -bs },
         { FIX16(0.0), FIX16(1.0), FIX16(0.0) },
         {         bs, FIX16(0.0),         bc }}
    };

    fix16_mat33_t mat_tmp;

    fix16_mat33_mul(&mat_a, &mat_b, &mat_tmp);

    const fix16_mat33_t mat_c = {
        {{         cc,         cs, FIX16(0.0) },
         {        -cs,         cc, FIX16(0.0) },
         { FIX16(0.0), FIX16(0.0), FIX16(1.0) }}
    };

    fix16_mat33_mul(&mat_tmp, &mat_c, result);
}

size_t
fix16_mat33_str(const fix16_mat33_t *m0, char *buffer, int32_t decimals)
{
    char *buffer_ptr;
    buffer_ptr = buffer;

    *buffer_ptr++ = '|';
      buffer_ptr += fix16_vec3_str(&m0->row[0], buffer_ptr, decimals);
    *buffer_ptr++ = '|';
    *buffer_ptr++ = '\n';

    *buffer_ptr++ = '|';
      buffer_ptr += fix16_vec3_str(&m0->row[1], buffer_ptr, decimals);
    *buffer_ptr++ = '|';
    *buffer_ptr++ = '\n';

    *buffer_ptr++ = '|';
      buffer_ptr += fix16_vec3_str(&m0->row[2], buffer_ptr, decimals);
    *buffer_ptr++ = '|';

    *buffer_ptr   = '\0';

    return (buffer_ptr - buffer);
}
