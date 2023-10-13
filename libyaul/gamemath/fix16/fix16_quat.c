/*
 * Copyright (c)
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <gamemath/fix16/fix16_quat.h>
#include <gamemath/fix16/fix16_trig.h>

void
fix16_quat_euler(angle_t rx, angle_t ry, angle_t rz, fix16_quat_t *result)
{
    assert(result != NULL);

    /* Source: <https://stackoverflow.com/a/50012073/604033> */

    /* Order: YZX
     *
     * Heading  Y
     * Attitude Z
     * Bank     X */
    const angle_t heading  = angle_int32_to(ry) >> 1;
    const angle_t attitude = angle_int32_to(rz) >> 1;
    const angle_t bank     = angle_int32_to(rx) >> 1;

    const fix16_t c1 = fix16_cos(heading);
    const fix16_t c2 = fix16_cos(attitude);
    const fix16_t c3 = fix16_cos(bank);

    const fix16_t s1 = fix16_sin(heading);
    const fix16_t s2 = fix16_sin(attitude);
    const fix16_t s3 = fix16_sin(bank);

    const fix16_t s1s2 = fix16_mul(s1, s2);
    const fix16_t s1c2 = fix16_mul(s1, c2);
    const fix16_t c1c2 = fix16_mul(c1, c2);
    const fix16_t c1s2 = fix16_mul(c1, s2);

    result->w      = fix16_mul(c1c2,c3) - fix16_mul(s1s2,s3);
    result->comp.x = fix16_mul(s1s2,c3) + fix16_mul(c1c2,s3);
    result->comp.y = fix16_mul(s1c2,c3) + fix16_mul(c1s2,s3);
    result->comp.z = fix16_mul(c1s2,c3) - fix16_mul(s1c2,s3);
}

void
fix16_quat_axis_angle(const fix16_vec3_t *axis, angle_t angle, fix16_quat_t *result)
{
    /* Source: <https://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm> */

    const angle_t angle_div_2 = angle_int32_to(angle) >> 1;
    const fix16_t s = fix16_sin(angle_div_2);

    result->w = fix16_cos(angle_div_2);
    fix16_vec3_scaled(s, axis, &result->comp);
}

void
fix16_quat_conjugate(const fix16_quat_t *q0, fix16_quat_t *result)
{
    result->w = q0->w;
    result->comp.x = -q0->comp.x;
    result->comp.y = -q0->comp.y;
    result->comp.z = -q0->comp.z;
}

void
fix16_quat_mul(const fix16_quat_t *q0 __unused, const fix16_quat_t *q1 __unused, fix16_quat_t *result __unused)
{
    assert(q0 != NULL);
    assert(q1 != NULL);
    assert(result != NULL);
}

void
fix16_quat_vec3_mul(const fix16_quat_t *q0, const fix16_vec3_t *v0, fix16_vec3_t *result)
{
    assert(q0 != NULL);
    assert(v0 != NULL);
    assert(result != NULL);

    /* Source: <https://gamedev.stackexchange.com/a/50545>
     *
     * u: vector part of the quaternion
     * s: scalar part of the quaternion
     * v: vector
     *
     *  a = 2 * dot(u, v) *
     *  b = s*s - dot(u, u)
     *  c = 2 * s
     * v' = (a * u) + (b * v) + (c * cross(u, v)) */

    /* v'=qpq*
     *   =(u,s)(v,0)(-u,s) */

    const fix16_vec3_t * const u = &q0->comp;
    const fix16_t s = q0->w;

    const fix16_t dot_uv = fix16_vec3_dot(u, v0) << 1; /* Shift is multiplying by 2 */
    const fix16_t dot_uu = fix16_vec3_dot(u, u);
    const fix16_t ss = fix16_mul(s, s);
    const fix16_t a = dot_uv;
    const fix16_t b = ss - dot_uu;
    const fix16_t c = s << 1;

    fix16_vec3_t cross_uv;
    fix16_vec3_cross(u, v0, &cross_uv);

    /* Matrix multiplication is possible, but u, v0, and cross_uv need be
     * transposed */

    result->x = fix16_mul(a, u->x) + fix16_mul(b, v0->x) + fix16_mul(c, cross_uv.x);
    result->y = fix16_mul(a, u->y) + fix16_mul(b, v0->y) + fix16_mul(c, cross_uv.y);
    result->z = fix16_mul(a, u->z) + fix16_mul(b, v0->z) + fix16_mul(c, cross_uv.z);
}

size_t
fix16_quat_str(const fix16_quat_t *q0, char *buffer, int32_t decimals)
{
    assert(q0 != NULL);
    assert(buffer != NULL);

    char *buffer_ptr;
    buffer_ptr = buffer;

    *buffer_ptr++ = '<';
    buffer_ptr += fix16_vec3_str(&q0->comp, buffer_ptr, decimals);
    *buffer_ptr++ = ',';
    buffer_ptr += fix16_str(q0->w, buffer_ptr, decimals);
    *buffer_ptr++ = '>';

    *buffer_ptr = '\0';

    return (buffer_ptr - buffer);

    return 0;
}
