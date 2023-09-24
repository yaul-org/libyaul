/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_RENDER_H_
#define _MIC3D_RENDER_H_

#include <sys/cdefs.h>

#include <gamemath/fix16.h>

#include "mic3d/types.h"

#include "gst.h"
#include "sort.h"

#define RENDER_FLAG_TEST(x) ((__state.render->render_flags & __CONCAT(RENDER_FLAGS_, x)) == __CONCAT(RENDER_FLAGS_, x))

typedef enum clip_bitmap {
    CLIP_BIT_LEFT   = 0,
    CLIP_BIT_RIGHT  = 1,
    CLIP_BIT_TOP    = 2,
    CLIP_BIT_BOTTOM = 3
} clip_bitmap_t;

typedef enum clip_flags {
    CLIP_FLAGS_NONE   = 0,
    CLIP_FLAGS_LEFT   = 1 << CLIP_BIT_LEFT,
    CLIP_FLAGS_RIGHT  = 1 << CLIP_BIT_RIGHT,
    CLIP_FLAGS_TOP    = 1 << CLIP_BIT_TOP,
    CLIP_FLAGS_BOTTOM = 1 << CLIP_BIT_BOTTOM,

    CLIP_FLAGS_LR     = CLIP_FLAGS_LEFT | CLIP_FLAGS_RIGHT,
    CLIP_FLAGS_TB     = CLIP_FLAGS_TOP  | CLIP_FLAGS_BOTTOM
} clip_flags_t;

typedef struct {
    attribute_t attribute;
    polygon_t polygon;
    int16_vec2_t screen_points[4];
    int16_t z_values[4];
    clip_flags_t clip_flags[4];
    clip_flags_t and_flags;
    clip_flags_t or_flags;
} __aligned(16) pipeline_t;

typedef struct render {
    /* Pools */
    struct {
        int16_vec2_t *screen_points_pool;
        int16_t *z_values_pool;
        fix16_t *depth_values_pool;
        vdp1_cmdt_t *cmdts_pool;
    };

    /* Settings */
    struct {
        camera_type_t camera_type;
        fix16_t view_distance;
        fix16_t near;
        fix16_t far;
        fix16_t ortho_size;
        int32_t sort_scale;
        fix16_t depth_scale;
        fix16_t depth_offset;
    };

    /* Rendering */
    struct {
        const mesh_t *mesh;
        const fix16_mat43_t *world_matrix;
        pipeline_t *pipeline;
        render_flags_t render_flags;
        vdp1_cmdt_t *cmdts;
    };

    /* Matrices */
    struct {
        fix16_mat43_t *camera_matrix;
        fix16_mat43_t *view_matrix;
    };

    /* Sorting */
    struct {
        vdp1_cmdt_t *sort_cmdt;
        vdp1_link_t sort_link;
    };
} __aligned(4) render_t;

void __render_init(void);

extern void __render_single(const sort_single_t *single);

#endif /* _MIC3D_RENDER_H_ */
