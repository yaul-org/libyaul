/*
 * Copyright (c) 2022-2023 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _MIC3D_H_
#define _MIC3D_H_

#include <mic3d/types.h>

#define SCREEN_WIDTH  352
#define SCREEN_HEIGHT 224

#define POLYGON_COUNT      512
#define POINTS_COUNT       (POLYGON_COUNT * 4)
#define SORT_DEPTH         512
#define MATRIX_STACK_COUNT 16
#define CMDT_COUNT         2048

#define TEXTURE_SIZE(w, h)       ((uint16_t)((((w) >> 3) << 8) | ((h) & 255)))
#define TEXTURE_VRAM_INDEX(addr) ((uint16_t)((uintptr_t)(addr) >> 3))

void mic3d_init(void);

void camera_lookat(const camera_t *camera);

void render_enable(render_flags_t flags);
void render_disable(render_flags_t flags);
void render_perspective_set(angle_t fov_angle);
void render_near_level_set(uint32_t level);
void render_far_set(fix16_t far);
void render_mesh_transform(const mesh_t *mesh);
void render(void);

texture_t *tlist_acquire(uint32_t count);
void tlist_release(void);
void tlist_set(texture_t *textures, uint16_t count);
texture_t *tlist_get(void);

void matrix_push(void);
void matrix_ptr_push(void);
void matrix_pop(void);
fix16_mat43_t *matrix_top(void);
void matrix_copy(fix16_mat43_t *m0);
void matrix_set(const fix16_mat43_t *m0);
void matrix_x_translate(fix16_t x);
void matrix_y_translate(fix16_t y);
void matrix_z_translate(fix16_t z);
void matrix_translate(const fix16_vec3_t *t);
void matrix_translation_set(const fix16_vec3_t *t);
void matrix_translation_get(fix16_vec3_t *t);
void matrix_x_rotate(angle_t angle);
void matrix_y_rotate(angle_t angle);
void matrix_z_rotate(angle_t angle);

void light_gst_set(vdp1_gouraud_table_t *gouraud_tables, uint32_t count, vdp1_vram_t vram_base);

void gst_set(vdp1_vram_t vram_base);
void gst_unset(void);
void gst_put(const vdp1_gouraud_table_t *gouraud_tables, uint32_t put_count);
void gst_put_wait(void); /* XXX: Not yet implemented */

#endif /* _MIC3D_H_ */
