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

struct sega3d_object {
        void *pdata;
        vdp1_cmdt_list_t *cmdt_list;
        uint16_t offset;

        sega3d_iterate_fn iterate_fn;

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
extern void sega3d_matrix_copy(void);
extern void sega3d_matrix_translate(FIXED tx, FIXED ty, FIXED tz);
extern void sega3d_matrix_scale(FIXED sx, FIXED sy, FIXED sz);

extern Uint16 sega3d_object_polycount_get(sega3d_object_t *object);
extern void sega3d_object_prepare(sega3d_object_t *object);
extern void sega3d_object_transform(sega3d_object_t *object);
extern void sega3d_object_iterate(sega3d_object_t *object);

/// Standard iteration functions
extern void sega3d_standard_iterate(sega3d_object_t *object, const vdp1_cmdt_t *cmdt);

#endif /* SEGA3D_H_ */
