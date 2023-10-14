/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <cpu/divu.h>

#include <gamemath/math3d.h>
#include <gamemath/fix16/fix16_trig.h>

#define MIN_FOV_ANGLE DEG2ANGLE( 20.0f)
#define MAX_FOV_ANGLE DEG2ANGLE(120.0f)

void
math3d_lookat(const lookat_t *lookat)
{
    /* forward = normalize(forward)
     * right   = normalize(cross(up, forward))
     * up      = cross(forward, right)
     *
     * LookAt matrix:
     *   rightx   righty   rightz
     *   upx      upy      upz
     *   forwardx forwardy forwardz */

    fix16_vec3_sub(lookat->from, lookat->to, lookat->basis_forward);
    fix16_vec3_normalize(lookat->basis_forward);

    fix16_vec3_cross(lookat->up, lookat->basis_forward, lookat->basis_right);
    fix16_vec3_normalize(lookat->basis_right);

    fix16_vec3_cross(lookat->basis_forward, lookat->basis_right, lookat->basis_up);
}

fix16_t
math3d_view_distance_calc(int16_t screen_width, angle_t fov_angle)
{
    fov_angle = clamp(fov_angle, MIN_FOV_ANGLE, MAX_FOV_ANGLE);

    const angle_t hfov_angle = fov_angle >> 1;
    const fix16_t screen_scale = fix16_int32_from(screen_width) >> 1;
    const fix16_t tan = fix16_tan(hfov_angle);

    return fix16_mul(screen_scale, tan);
}

void
math3d_point_perspective_xform(const fix16_mat43_t *view_matrix, fix16_t view_distance,
  const fix16_vec3_t *point, xform_t *result)
{
    assert(view_matrix != NULL);
    assert(point != NULL);
    assert(result != NULL);

    const fix16_vec3_t * const m0 = &view_matrix->rotation.row[0];
    const fix16_vec3_t * const m1 = &view_matrix->rotation.row[1];
    const fix16_vec3_t * const m2 = &view_matrix->rotation.row[2];

    fix16_vec3_t p;

    p.z = fix16_vec3_dot(m2, point) + view_matrix->translation.z;

    cpu_divu_fix16_set(view_distance, p.z);

    p.x = fix16_vec3_dot(m0, point) + view_matrix->translation.x;
    p.y = fix16_vec3_dot(m1, point) + view_matrix->translation.y;

    result->depth_value = cpu_divu_quotient_get();

    result->screen_point.x = fix16_high_mul(result->depth_value, p.x);
    result->screen_point.y = fix16_high_mul(result->depth_value, p.y);
}

void
math3d_point_orthographic_xform(const fix16_mat43_t *view_matrix,
    fix16_t ortho_size, const fix16_vec3_t *point, xform_t *result)
{
    assert(view_matrix != NULL);
    assert(point != NULL);
    assert(result != NULL);

    fix16_vec3_t p;
    fix16_mat43_pos3_mul(view_matrix, point, &p);

    result->screen_point.x = fix16_high_mul( ortho_size, p.x);
    result->screen_point.y = fix16_high_mul(-ortho_size, p.y);
    result->depth_value = p.z;
}
