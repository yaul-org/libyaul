/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef SEGA3D_H_
#define SEGA3D_H_

#include <stdint.h>

#include <fix16.h>

#include <vdp1/cmdt.h>

#include "sgl.h"
#include "ztp.h"

typedef struct sega3d_object sega3d_object_t;

typedef void (*sega3d_iterate_fn)(sega3d_object_t *, const vdp1_cmdt_t *);

typedef enum {
        SEGA3D_MATRIX_TYPE_PUSH     = 0,
        SEGA3D_MATRIX_TYPE_MOVE_PTR = 1
} sega3d_matrix_type_t;

typedef enum {
        SEGA3D_OBJECT_FLAGS_NONE          = 0,
        /// Display non-textured polygons
        SEGA3D_OBJECT_FLAGS_NON_TEXTURED  = 1 << 0,
        /// Display wireframe
        SEGA3D_OBJECT_FLAGS_WIREFRAME     = 1 << 1,
        /// Cull in world space
        SEGA3D_OBJECT_FLAGS_CULL_VIEW     = 1 << 2,
        /// Cull in screen space
        SEGA3D_OBJECT_FLAGS_CULL_SCREEN   = 1 << 3,
        /// Cull object
        SEGA3D_OBJECT_FLAGS_CULL_OBJECT   = 1 << 4,
} sega3d_flags_t;

typedef enum {
        SEGA3D_CULL_TYPE_NONE   = 0,
        SEGA3D_CULL_TYPE_SPHERE = 1 << 0,
        SEGA3D_CULL_TYPE_BOX    = 1 << 1
} sega3d_cull_type_t;

typedef struct {
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

typedef struct {
        const color_rgb1555_t * const depth_colors;
        const uint8_t * const depth_z;
        uint8_t depth_count;
        FIXED step;
        FIXED start_z;
        FIXED end_z;
        color_rgb1555_t near_ambient_color;
        color_rgb1555_t far_ambient_color;
} sega3d_fog_t;

typedef struct {
        uint16_t count;
} sega3d_results_t;

typedef struct {
        FIXED radius;
} sega3d_cull_sphere_t;

typedef struct {
        FIXED length[XYZ];
} sega3d_cull_box_t;

struct sega3d_object {
        sega3d_flags_t flags;

        void *pdatas;
        uint16_t pdata_count;

        FIXED origin[XYZ];
        sega3d_cull_type_t cull_type;
        void *cull_data;

        void *user_data;
};

extern void sega3d_init(void);

extern void sega3d_tlist_alloc(Uint16 count);
extern void sega3d_tlist_free(void);
extern void sega3d_tlist_set(TEXTURE *textures, Uint16 count);
extern Uint16 sega3d_tlist_count_get(void);
extern Uint16 sega3d_tlist_cursor_get(void);
extern void sega3d_tlist_cursor_reset(void);
extern TEXTURE *sega3d_tlist_tex_append(void);
extern TEXTURE *sega3d_tlist_tex_get(Uint16 cursor);

extern void sega3d_matrix_identity(MATRIX *matrix);
extern void sega3d_matrix_push(sega3d_matrix_type_t matrix_type);
extern void sega3d_matrix_pop(void);
extern const MATRIX *sega3d_matrix_top(void);
extern void sega3d_matrix_load(const MATRIX *matrix);
extern void sega3d_matrix_copy(MATRIX *matrix);
extern void sega3d_matrix_trans(FIXED tx, FIXED ty, FIXED tz);
extern void sega3d_matrix_trans_reset(void);
extern void sega3d_matrix_trans_load(FIXED tx, FIXED ty, FIXED tz);
extern void sega3d_matrix_rot_x(const ANGLE angle);
extern void sega3d_matrix_rot_y(const ANGLE angle);
extern void sega3d_matrix_rot_z(const ANGLE angle);
extern void sega3d_matrix_transpose(void);

extern void sega3d_display_level_set(uint16_t level);
extern void sega3d_perspective_set(ANGLE fov);
extern void sega3d_frustum_camera_set(const POINT *position, const VECTOR *rx,
    const VECTOR *ry, const VECTOR *rz);
extern void sega3d_info_get(sega3d_info_t *info);

extern void sega3d_fog_set(const sega3d_fog_t *fog);
extern void sega3d_fog_limits_set(FIXED start_z, FIXED end_z);

extern void sega3d_start(vdp1_cmdt_orderlist_t *orderlist, uint16_t orderlist_offset, vdp1_cmdt_t *cmdts);
extern void sega3d_finish(sega3d_results_t *results);

extern Uint16 sega3d_object_polycount_get(const sega3d_object_t *object);
extern void sega3d_object_transform(const sega3d_object_t *object, uint16_t pdata_index);

extern void sega3d_ztp_pdata_patch(sega3d_object_t *object, const sega3d_ztp_t *ztp);
extern void sega3d_ztp_texs_get(const sega3d_ztp_t *ztp, sega3d_ztp_tex_t *ztp_texs);

#endif /* SEGA3D_H_ */
