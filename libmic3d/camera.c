/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <gamemath/fix16/fix16_mat33.h>

#include "internal.h"

void
camera_type_set(camera_type_t type)
{
    render_t * const render = __state.render;

    render->camera_type = type;
}

void
camera_lookat(const camera_t *camera)
{
    assert(camera != NULL);

    render_t * const render = __state.render;
    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    fix16_mat43_lookat(&camera->position, &camera->target, &camera->up, camera_matrix);
}

void
camera_moveto(const camera_t *camera)
{
    assert(camera != NULL);

    render_t * const render = __state.render;

    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    const fix16_vec3_t * const m00 = &camera_matrix->rotation.row[0];
    const fix16_vec3_t * const m01 = &camera_matrix->rotation.row[1];
    const fix16_vec3_t * const m02 = &camera_matrix->rotation.row[2];

    camera_matrix->translation.x = -fix16_vec3_dot(m00, &camera->position);
    camera_matrix->translation.y = -fix16_vec3_dot(m01, &camera->position);
    camera_matrix->translation.z = -fix16_vec3_dot(m02, &camera->position);
}

fix16_mat43_t *
camera_matrix_get(void)
{
    render_t * const render = __state.render;

    return render->camera_matrix;
}
