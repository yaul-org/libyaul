#ifndef MIC3D_RENDER_H
#define MIC3D_RENDER_H

#include <sys/cdefs.h>

#include <fix16.h>

#include <mic3d/types.h>

#include "gst.h"

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
        CLIP_FLAGS_TB     = CLIP_FLAGS_TOP | CLIP_FLAGS_BOTTOM
} clip_flags_t;

typedef struct {
        fix16_mat43_t view_matrix;
        const attribute_t *ro_attribute;

        attribute_t rw_attribute;
        indices_t indices;
        int16_vec2_t screen_points[4];
        fix16_t z_values[4];
        clip_flags_t clip_flags[4];
        clip_flags_t and_flags;
        clip_flags_t or_flags;
} __aligned(16) render_transform_t;

static_assert(sizeof(render_transform_t) == 128);

typedef struct render {
        /* Pools */
        fix16_t *z_values_pool;
        int16_vec2_t *screen_points_pool;
        fix16_t *depth_values_pool;
        vdp1_cmdt_t *cmdts_pool;
        gst_t *gst;

        /* Settings */
        fix16_t view_distance;
        fix16_t near;
        fix16_t far;
        fix16_t sort_scale;
        render_flags_t render_flags;

        const mesh_t *mesh;
        render_transform_t *render_transform;

        /* Sorting */
        vdp1_cmdt_t *sort_cmdt;
        vdp1_link_t sort_link;

        vdp1_cmdt_t *cmdts;
        uint32_t cmdt_count;
} __aligned(4) render_t;

void __render_init(void);

#endif /* MIC3D_RENDER_H */
