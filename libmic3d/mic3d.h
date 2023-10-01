/*
 * Copyright (c) Israel Jacquez
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

/* TODO: Remove hard coded screen resolution */
#define SCREEN_WIDTH  352
#define SCREEN_HEIGHT 224

#define TEXTURE_SIZE(w, h)       ((uint16_t)((((w) >> 3) << 8) | ((h) & 255)))
#define TEXTURE_VRAM_INDEX(addr) ((uint16_t)((uintptr_t)(addr) >> 3))

__BEGIN_DECLS

extern void mic3d_init(workarea_mic3d_t *workarea);

extern void camera_type_set(camera_type_t type);
extern void camera_lookat(const camera_t *camera);
extern void camera_moveto(const camera_t *camera);
extern fix16_mat43_t *camera_matrix_get(void);

extern void render_enable(render_flags_t flags);
extern void render_disable(render_flags_t flags);
extern void render_flags_set(render_flags_t flags);
extern void render_perspective_set(angle_t fov_angle);
extern void render_orthographic_set(fix16_t scale);
extern void render_near_level_set(uint32_t level);
extern void render_far_level_set(uint32_t level);
extern void render_sort_depth_set(sort_list_t *sort_list_buffer, uint16_t count);
extern void render_start(void);
extern void render_end(void);
extern void render_mesh_xform(const mesh_t *mesh,
  const fix16_mat43_t *world_matrix);
extern void render_cmdt_insert(const vdp1_cmdt_t *cmdt, fix16_t depth_z);
extern void render_cmdt_nocheck_insert(const vdp1_cmdt_t *cmdt,
  fix16_t depth_z);
extern void render_cmdts_reserve(uint32_t cmdt_count); /* XXX: Not yet implemented */
extern void render_cmdts_relinquish(void); /* XXX: Not yet implemented */
extern void render_point_xform(const fix16_mat43_t *world_matrix,
  const fix16_vec3_t *point, xform_t *xform);

extern void render_debug_log(char *buffer, size_t len); /* XXX: Not yet implemented */

extern void tlist_set(const texture_t *textures, uint16_t count);
extern const texture_t *tlist_get(void);

extern void light_gst_set(vdp1_gouraud_table_t *gouraud_tables, uint32_t count,
  vdp1_vram_t vram_base);

extern void gst_set(vdp1_vram_t vram_base);
extern void gst_unset(void);
extern void gst_put(const vdp1_gouraud_table_t *gouraud_tables,
    uint32_t put_count);
extern void gst_put_wait(void); /* XXX: Not yet implemented */

__END_DECLS

#endif /* _MIC3D_H_ */
