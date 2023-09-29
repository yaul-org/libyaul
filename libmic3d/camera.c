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

    /* fix16_mat33_t lookat_matrix; */

    const lookat_t lookat = {
        .from          = &camera->position,
        .to            = &camera->target,
        .up            = &camera->up,
        .basis_right   = (fix16_vec3_t *)&camera_matrix->row[0],
        .basis_up      = (fix16_vec3_t *)&camera_matrix->row[1],
        .basis_forward = (fix16_vec3_t *)&camera_matrix->row[2],
    };

    math3d_lookat(&lookat);
}

void
camera_moveto(const camera_t *camera)
{
    assert(camera != NULL);

    render_t * const render = __state.render;

    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    const fix16_vec3_t neg_t = {
        .x = -camera->position.x,
        .y = -camera->position.y,
        .z = -camera->position.z
    };

    const fix16_vec3_t * const m00 = (const fix16_vec3_t *)&camera_matrix->row[0];
    const fix16_vec3_t * const m01 = (const fix16_vec3_t *)&camera_matrix->row[1];
    const fix16_vec3_t * const m02 = (const fix16_vec3_t *)&camera_matrix->row[2];

    camera_matrix->frow[0][3] = fix16_vec3_dot(m00, &neg_t);
    camera_matrix->frow[1][3] = fix16_vec3_dot(m01, &neg_t);
    camera_matrix->frow[2][3] = fix16_vec3_dot(m02, &neg_t);
}

void
camera_right_get(fix16_vec3_t* right)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    right->x = camera_matrix->frow[0][0];
    right->y = camera_matrix->frow[0][1];
    right->z = camera_matrix->frow[0][2];
}

void
camera_up_get(fix16_vec3_t* up)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    up->x = camera_matrix->frow[1][0];
    up->y = camera_matrix->frow[1][1];
    up->z = camera_matrix->frow[1][2];
}

void
camera_forward_get(fix16_vec3_t* forward)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const camera_matrix = render->camera_matrix;

    forward->x = -camera_matrix->frow[2][0];
    forward->y = -camera_matrix->frow[2][1];
    forward->z = -camera_matrix->frow[2][2];
}

const fix16_mat43_t *
camera_matrix_get(void)
{
    render_t * const render = __state.render;

    return render->camera_matrix;
}
