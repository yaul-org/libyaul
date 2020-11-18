/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "sega3d-internal.h"

static sega3d_fog_t _fog;

static sega3d_info_t _info;

static MATRIX _matrices[MATRIX_STACK_MAX] __aligned(16);

static transform_proj_t _transform_proj_pool[VERTEX_POOL_SIZE];
static transform_t _transform;

static sort_list_t _sort_list[SORT_Z_RANGE] __aligned(16);
static sort_single_t _sort_single_pool[PACKET_SIZE] __aligned(16);

/* XXX: This can be simplified down to using 8-bit vectors for the normal */
static fix16_plane_t _clip_planes[6] __aligned(16);

static state_t _state = {
        .flags = FLAGS_NONE,
        .fog = &_fog,
        .info = &_info,
        .transform = &_transform,
        .transform_proj_pool = _transform_proj_pool,
        .clip_planes = _clip_planes,
        .matrices = _matrices,
        .sort_list = _sort_list,
        .sort_single_pool = _sort_single_pool
};

state_t * const _internal_state = &_state;
