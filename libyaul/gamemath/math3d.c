/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

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
math3d_point_xform(const xform_config_t *xform_config,
    const fix16_vec3_t *point, xform_t *result)
{
    const fix16_mat43_t * const view_matrix = xform_config->view_matrix;

    const fix16_vec3_t * const m0 = (const fix16_vec3_t *)&view_matrix->row[0];
    const fix16_vec3_t * const m1 = (const fix16_vec3_t *)&view_matrix->row[1];
    const fix16_vec3_t * const m2 = (const fix16_vec3_t *)&view_matrix->row[2];

    const fix16_t z = fix16_vec3_dot(m2, point) + view_matrix->frow[2][3];
    const fix16_t clamped_z = fix16_clamp(z, xform_config->near, xform_config->far);

    cpu_divu_fix16_set(xform_config->view_distance, clamped_z);

    const fix16_t x = fix16_vec3_dot(m0, point) + view_matrix->frow[0][3];
    const fix16_t y = fix16_vec3_dot(m1, point) + view_matrix->frow[1][3];

    result->depth_value = cpu_divu_quotient_get();
    result->screen_point.x = fix16_int32_mul(result->depth_value, x);
    result->screen_point.y = fix16_int32_mul(result->depth_value, y);
}
