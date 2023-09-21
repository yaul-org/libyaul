/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "internal.h"

void
camera_lookat(const camera_t *camera)
{
    assert(camera != NULL);

    render_t * const render = __state.render;

    fix16_mat43_t * const view_matrix = render->camera_matrix;

    fix16_mat43_lookat(&camera->position, &camera->target, &camera->up,
        view_matrix);
}

void
camera_moveto(const camera_t *camera)
{
    assert(camera != NULL);

    render_t * const render = __state.render;

    fix16_mat43_t * const view_matrix = render->camera_matrix;

    fix16_mat43_translation_set(&camera->position, view_matrix);
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

    forward->x = camera_matrix->frow[2][0];
    forward->y = camera_matrix->frow[2][1];
    forward->z = camera_matrix->frow[2][2];
}

const fix16_mat43_t *
camera_matrix_get(void)
{
    render_t * const render = __state.render;

    return render->camera_matrix;
}

void
__camera_matrix_invert(void)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const camera_matrix = render->camera_matrix;
    fix16_mat43_t * const inv_camera_matrix = render->inv_camera_matrix;

    fix16_mat43_invert(camera_matrix, inv_camera_matrix);
}
