#include "sega3d-internal.h"

static sega3d_fog_t _fog;

static MATRIX _matrices[MATRIX_STACK_MAX] __aligned(16);

static transform_proj_t _transform_proj_pool[VERTEX_POOL_SIZE] __aligned(16);
static transform_t _transform __aligned(16);

state_t _internal_state = {
        .flags = FLAGS_NONE,
        .fog = &_fog,
        .transform = &_transform,
        .transform_proj_pool = _transform_proj_pool,
        .matrices = _matrices
};
