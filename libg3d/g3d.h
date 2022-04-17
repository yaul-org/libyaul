/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _G3D_H_
#define _G3D_H_

#include <stdint.h>

#include <fix16.h>

#include <vdp1/cmdt.h>

#include <g3d/types.h>
#include <g3d/sgl.h>
#include <g3d/perf.h>
#include <g3d/s3d.h>

extern void g3d_init(void);

extern TEXTURE *g3d_tlist_alloc(Uint16 count);
extern void g3d_tlist_free(void);
extern void g3d_tlist_set(TEXTURE *textures, Uint16 count);

extern void g3d_plist_set(PALETTE *palettes, Uint16 count);

extern void g3d_matrix_identity(MATRIX *matrix);
extern void g3d_matrix_push(g3d_matrix_type_t matrix_type);
extern void g3d_matrix_pop(void);
extern const MATRIX *g3d_matrix_top(void);
extern void g3d_matrix_load(const MATRIX *matrix);
extern void g3d_matrix_copy(MATRIX *matrix);
extern void g3d_matrix_trans(FIXED tx, FIXED ty, FIXED tz);
extern void g3d_matrix_trans_load(FIXED tx, FIXED ty, FIXED tz);
extern void g3d_matrix_trans_reset(void);
extern void g3d_matrix_rot_load(const ANGLE rx, const ANGLE ry, const ANGLE rz);
extern void g3d_matrix_rot_x(const ANGLE angle);
extern void g3d_matrix_rot_y(const ANGLE angle);
extern void g3d_matrix_rot_z(const ANGLE angle);
extern void g3d_matrix_transpose_in(MATRIX *matrix);
extern void g3d_matrix_transpose(void);

extern void g3d_display_level_set(uint16_t level);
extern void g3d_perspective_set(ANGLE fov);
extern void g3d_frustum_camera_set(const POINT position, const VECTOR rx,
    const VECTOR ry, const VECTOR rz);
extern void g3d_info_get(g3d_info_t *info);

extern void g3d_fog_set(const g3d_fog_t *fog);
extern void g3d_fog_limits_set(FIXED start_z, FIXED end_z);

extern void g3d_start(vdp1_cmdt_orderlist_t *orderlist,
    uint16_t orderlist_offset, vdp1_cmdt_t *cmdts);
extern void g3d_finish(g3d_results_t *results);

extern Uint16 g3d_object_polycount_get(const g3d_object_t *object);
extern void g3d_object_transform(const g3d_object_t *object,
    uint16_t xpdata_index);

#endif /* !_G3D_H_ */
