/*
 * Copyright (c) 2013-2014
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/divu.h>

#include <gamemath/math3d.h>

void
math3d_point_transform(const transform_config_t *transform_config,
    const fix16_vec3_t *point, transform_result_t *result)
{
    const fix16_mat43_t * const view_matrix = transform_config->view_matrix;

    const fix16_vec3_t * const m0 = (const fix16_vec3_t *)&view_matrix->row[0];
    const fix16_vec3_t * const m1 = (const fix16_vec3_t *)&view_matrix->row[1];
    const fix16_vec3_t * const m2 = (const fix16_vec3_t *)&view_matrix->row[2];

    const fix16_t z = fix16_vec3_dot(m2, point) + view_matrix->frow[2][3];
    const fix16_t clamped_z = fix16_clamp(z, transform_config->near, transform_config->far);

    cpu_divu_fix16_set(transform_config->view_distance, clamped_z);

    const fix16_t x = fix16_vec3_dot(m0, point) + view_matrix->frow[0][3];
    const fix16_t y = fix16_vec3_dot(m1, point) + view_matrix->frow[1][3];

    result->depth_value = cpu_divu_quotient_get();
    result->screen_point.x = fix16_int32_mul(result->depth_value, x);
    result->screen_point.y = fix16_int32_mul(result->depth_value, y);
}
