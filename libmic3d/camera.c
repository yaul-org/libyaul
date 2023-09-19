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

    /* normalize(forward)
     * right = normalize(cross(forward, up))
     * up = cross(forward, right) */

    render_t * const render = __state.render;

    fix16_mat43_t * const view_matrix = render->matrices.view;

    fix16_vec3_t * const forward = (fix16_vec3_t *)&view_matrix->row[2];

    fix16_vec3_sub(&camera->target, &camera->position, forward);
    fix16_vec3_normalize(forward);

    fix16_vec3_t * const right = (fix16_vec3_t *)&view_matrix->row[0];
    fix16_vec3_cross(forward, &camera->up, right);
    fix16_vec3_normalize(right);

    fix16_vec3_t * const up = (fix16_vec3_t *)&view_matrix->row[1];
    fix16_vec3_cross(forward, right, up);
    fix16_vec3_normalize(up);
}

void
camera_moveto(const camera_t *camera)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const view_matrix = render->matrices.view;

    view_matrix->frow[0][3] = camera->position.x;
    view_matrix->frow[1][3] = camera->position.y;
    view_matrix->frow[2][3] = camera->position.z;
}

void
camera_forward_get(fix16_vec3_t* forward)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const view_matrix = render->matrices.view;

    forward->x = -view_matrix->frow[2][0];
    forward->y = -view_matrix->frow[2][1];
    forward->z = -view_matrix->frow[2][2];

    // XXX: Does the vector need to be normalized?
    fix16_vec3_normalize(forward);
}

void
camera_up_get(fix16_vec3_t* up)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const view_matrix = render->matrices.view;

    up->x = view_matrix->frow[1][0];
    up->y = view_matrix->frow[1][1];
    up->z = view_matrix->frow[1][2];

    // XXX: Does the vector need to be normalized?
    fix16_vec3_normalize(up);
}

void
camera_right_get(fix16_vec3_t* right)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const view_matrix = render->matrices.view;

    right->x = view_matrix->frow[0][0];
    right->y = view_matrix->frow[0][1];
    right->z = view_matrix->frow[0][2];

    // XXX: Does the vector need to be normalized?
    fix16_vec3_normalize(right);
}

void
__camera_view_invert(void)
{
    render_t * const render = __state.render;

    fix16_mat43_t * const view_matrix = render->matrices.view;
    fix16_mat43_t * const inv_view_matrix = render->matrices.inv_view;

    fix16_mat43_invert(view_matrix, inv_view_matrix);
}
