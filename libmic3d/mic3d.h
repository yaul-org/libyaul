/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_H_
#define _MIC3D_H_

#include <mic3d/config.h>
#include <mic3d/types.h>
#include <mic3d/sizes.h>
#include <mic3d/workarea.h>

#define SCREEN_WIDTH  352
#define SCREEN_HEIGHT 224

#define TEXTURE_SIZE(w, h)       ((uint16_t)((((w) >> 3) << 8) | ((h) & 255)))
#define TEXTURE_VRAM_INDEX(addr) ((uint16_t)((uintptr_t)(addr) >> 3))

__BEGIN_DECLS

void mic3d_init(mic3d_workarea_t *workarea);

void camera_lookat(const camera_t *camera);
void camera_moveto(const camera_t *camera);
void camera_forward_get(fix16_vec3_t *forward);
void camera_right_get(fix16_vec3_t *right);
void camera_up_get(fix16_vec3_t *up);

void render_enable(render_flags_t flags);
void render_disable(render_flags_t flags);
void render_flags_set(render_flags_t flags);
void render_perspective_set(angle_t fov_angle);
void render_near_level_set(uint32_t level);
void render_far_set(fix16_t far);
void render_world_matrix_set(const fix16_mat43_t *matrix);
void render_mesh_transform(const mesh_t *mesh);
void render_start(void);
void render_end(void);

void tlist_set(const texture_t *textures, uint16_t count);
const texture_t *tlist_get(void);

void light_gst_set(vdp1_gouraud_table_t *gouraud_tables, uint32_t count, vdp1_vram_t vram_base);

void gst_set(vdp1_vram_t vram_base);
void gst_unset(void);
void gst_put(const vdp1_gouraud_table_t *gouraud_tables, uint32_t put_count);
void gst_put_wait(void); /* XXX: Not yet implemented */

__END_DECLS

#endif /* _MIC3D_H_ */
