/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SEGA3D_TYPES_H_
#define _SEGA3D_TYPES_H_

#include <stdint.h>

#include <fix16.h>

#include <vdp1/cmdt.h>

#include "sgl.h"

typedef struct sega3d_object sega3d_object_t;

typedef void (*sega3d_iterate_fn)(sega3d_object_t *, const vdp1_cmdt_t *);

typedef enum sega3d_matrix_type {
        SEGA3D_MATRIX_TYPE_PUSH     = 0,
        SEGA3D_MATRIX_TYPE_MOVE_PTR = 1
} sega3d_matrix_type_t;

typedef enum sega3d_flags {
        SEGA3D_OBJECT_FLAGS_NONE         = 0,
        /// Display non-textured polygons
        SEGA3D_OBJECT_FLAGS_NON_TEXTURED = 1 << 0,
        /// Display wireframe
        SEGA3D_OBJECT_FLAGS_WIREFRAME    = 1 << 1,
        /// Cull in world space
        SEGA3D_OBJECT_FLAGS_CULL_VIEW    = 1 << 2,
        /// Cull in screen space
        SEGA3D_OBJECT_FLAGS_CULL_SCREEN  = 1 << 3,
        /// Cull object using a bounding sphere
        SEGA3D_OBJECT_FLAGS_CULL_SPHERE  = 1 << 4,
        /// Cull object using an AABB
        SEGA3D_OBJECT_FLAGS_CULL_AABB    = 1 << 5,
        /// Exclude from fog calculation
        SEGA3D_OBJECT_FLAGS_FOG_EXCLUDE  = 1 << 6
} sega3d_flags_t;

typedef struct sega3d_info {
        FIXED ratio;         /* Screen ratio */
        ANGLE fov;           /* FOV */
        FIXED near;          /* Distance between view point and near plane */
        FIXED far;           /* Distance between view point and far plane */
        /* View distance divided by level to determine where the near plane
         * is */
        uint16_t level;
        FIXED view_distance; /* Distance between view point (eye) and view plane */
} __aligned(4) sega3d_info_t;

static_assert(sizeof(sega3d_info_t) == 24);

typedef struct sega3d_fog {
        const color_rgb1555_t * const depth_colors;
        const uint8_t * const depth_z;
        uint8_t depth_count;
        FIXED step;
        FIXED start_z;
        FIXED end_z;
        color_rgb1555_t near_ambient_color;
        color_rgb1555_t far_ambient_color;
} sega3d_fog_t;

typedef struct sega3d_results {
        uint16_t object_count;
        uint16_t polygon_count;
} sega3d_results_t;

typedef struct sega3d_cull_sphere {
        FIXED origin[XYZ];
        FIXED radius;
} sega3d_cull_sphere_t;

typedef struct sega3d_cull_aabb {
        FIXED origin[XYZ];
        FIXED length[XYZ];
} sega3d_cull_aabb_t;

struct sega3d_object {
        sega3d_flags_t flags;

        void *xpdatas;
        uint16_t xpdata_count;

        void *cull_shape;

        void *user_data;
};

#endif /* !_SEGA3D_TYPES_H_ */
