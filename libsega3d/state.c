#include "sega3d-internal.h"

static sega3d_fog_t _fog;

static sega3d_info_t _info;

static MATRIX _matrices[MATRIX_STACK_MAX] __aligned(16);

static transform_proj_t _transform_proj_pool[VERTEX_POOL_SIZE];
static transform_t _transform;

static sort_list_t _sort_list[Z_RANGE] __aligned(16);
static sort_single_t _sort_single_pool[PACKET_SIZE] __aligned(16);

state_t _internal_state = {
        .flags = FLAGS_NONE,
        .fog = &_fog,
        .info = &_info,
        .transform = &_transform,
        .transform_proj_pool = _transform_proj_pool,
        .matrices = _matrices,
        .sort_list = _sort_list,
        .sort_single_pool = _sort_single_pool
};
