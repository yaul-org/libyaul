/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SEGA3D_H_
#define _SEGA3D_H_

#include <stdint.h>

#include <fix16.h>

#include <vdp1/cmdt.h>

#include "sega3d-types.h"
#include "sgl.h"
#include "ztp.h"

extern void sega3d_init(void);

extern TEXTURE *sega3d_tlist_alloc(Uint16 count);
extern void sega3d_tlist_free(void);
extern void sega3d_tlist_set(TEXTURE *textures, Uint16 count);

extern void sega3d_matrix_identity(MATRIX *matrix);
extern void sega3d_matrix_push(sega3d_matrix_type_t matrix_type);
extern void sega3d_matrix_pop(void);
extern const MATRIX *sega3d_matrix_top(void);
extern void sega3d_matrix_load(const MATRIX *matrix);
extern void sega3d_matrix_copy(MATRIX *matrix);
extern void sega3d_matrix_trans(FIXED tx, FIXED ty, FIXED tz);
extern void sega3d_matrix_trans_load(FIXED tx, FIXED ty, FIXED tz);
extern void sega3d_matrix_trans_reset(void);
extern void sega3d_matrix_rot_load(const ANGLE rx, const ANGLE ry, const ANGLE rz);
extern void sega3d_matrix_rot_x(const ANGLE angle);
extern void sega3d_matrix_rot_y(const ANGLE angle);
extern void sega3d_matrix_rot_z(const ANGLE angle);
extern void sega3d_matrix_transpose_in(MATRIX *matrix);
extern void sega3d_matrix_transpose(void);

extern void sega3d_display_level_set(uint16_t level);
extern void sega3d_perspective_set(ANGLE fov);
extern void sega3d_frustum_camera_set(const POINT position, const VECTOR rx,
    const VECTOR ry, const VECTOR rz);
extern void sega3d_info_get(sega3d_info_t *info);

extern void sega3d_fog_set(const sega3d_fog_t *fog);
extern void sega3d_fog_limits_set(FIXED start_z, FIXED end_z);

extern void sega3d_start(vdp1_cmdt_orderlist_t *orderlist,
    uint16_t orderlist_offset, vdp1_cmdt_t *cmdts);
extern void sega3d_finish(sega3d_results_t *results);

extern Uint16 sega3d_object_polycount_get(const sega3d_object_t *object);
extern void sega3d_object_transform(const sega3d_object_t *object,
    uint16_t xpdata_index);

extern sega3d_ztp_handle_t sega3d_ztp_parse(sega3d_object_t *object,
    const sega3d_ztp_t *ztp);
extern void sega3d_ztp_textures_parse(sega3d_ztp_handle_t *handle, void *vram,
    sega3d_ztp_tex_fn_t tex_fn);
extern void sega3d_ztp_update(sega3d_ztp_handle_t *handle,
    uint16_t texture_num);

#endif /* !_SEGA3D_H_ */
