/*
 * No copyright.
 */

#ifndef SEGA3D_H_
#define SEGA3D_H_

#include <stdint.h>

#include <fix16.h>

#include <vdp1/cmdt.h>

#include "sgl.h"

typedef struct sega3d_object sega3d_object_t;

typedef void (*sega3d_iterate_fn)(sega3d_object_t *, const vdp1_cmdt_t *);

typedef enum {
        MATRIX_TYPE_PUSH     = 0,
        MATRIX_TYPE_MOVE_PTR = 1
} matrix_type_t;

typedef enum {
        SEGA3D_OBJECT_FLAGS_NONE          = 0,
        /// Display non-textured polygons
        SEGA3D_OBJECT_FLAGS_NON_TEXTURED  = 1 << 0,
        /// Display wireframe
        SEGA3D_OBJECT_FLAGS_WIREFRAME     = 1 << 1,
        /// Cull in world space
        SEGA3D_OBJECT_FLAGS_CULL_VIEW     = 1 << 2,
        /// Cull in screen space
        SEGA3D_OBJECT_FLAGS_CULL_SCREEN   = 1 << 3
} sega3d_flags_t;

typedef struct {
        FIXED top;
        FIXED left;
        FIXED right;
        FIXED bottom;
        FIXED ratio;
        FIXED near;
        FIXED focal_length;
} __aligned(4) sega3d_info_t;

typedef struct {
        const color_rgb1555_t * const depth_colors;
        const uint8_t * const depth_z;
        uint8_t pow;
        FIXED step;
        color_rgb1555_t near_ambient_color;
        color_rgb1555_t far_ambient_color;

        uint16_t gouraud_idx;
} sega3d_fog_t;

typedef struct {
        uint16_t count;
} sega3d_results_t;

struct sega3d_object {
        void *pdata;
        sega3d_flags_t flags;
        void *data;
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

extern void sega3d_matrix_push(matrix_type_t matrix_type);
extern void sega3d_matrix_pop(void);
extern const MATRIX *sega3d_matrix_top(void);
extern void sega3d_matrix_load(const MATRIX *matrix);
extern void sega3d_matrix_copy(MATRIX *matrix);
extern void sega3d_matrix_inverse_push(void);
extern void sega3d_matrix_translate(FIXED tx, FIXED ty, FIXED tz);
extern void sega3d_matrix_rotate_x(const ANGLE angle);
extern void sega3d_matrix_rotate_y(const ANGLE angle);
extern void sega3d_matrix_rotate_z(const ANGLE angle);

extern void sega3d_perspective_set(ANGLE fov);
extern void sega3d_info_get(sega3d_info_t *info);

extern void sega3d_fog_set(const sega3d_fog_t *fog);
extern void sega3d_fog_limits_set(FIXED start_z, FIXED end_z);

extern void sega3d_start(vdp1_cmdt_orderlist_t *orderlist, uint16_t orderlist_offset, vdp1_cmdt_t *cmdts);
extern void sega3d_finish(sega3d_results_t *results);

extern Uint16 sega3d_object_polycount_get(const sega3d_object_t *object);
extern void sega3d_object_transform(const sega3d_object_t *object);

#endif /* SEGA3D_H_ */
