/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include <sys/cdefs.h>

#include <vdp2/tvmd.h>

#include <cpu/divu.h>

#include "sega3d.h"
#include "sega3d-internal.h"

typedef enum {
        CLIP_FLAGS_NONE   = 0,
        CLIP_FLAGS_NEAR   = 1 << 0,
        CLIP_FLAGS_FAR    = 1 << 1,
        CLIP_FLAGS_LEFT   = 1 << 2,
        CLIP_FLAGS_RIGHT  = 1 << 3,
        CLIP_FLAGS_TOP    = 1 << 4,
        CLIP_FLAGS_BOTTOM = 1 << 5,
} clip_flags_t;

typedef struct {
        FIXED p[XYZ];
        FIXED proj[XY];
        FIXED inverse_top;
        FIXED inverse_right;
        FIXED z_avg;
        clip_flags_t clip_flags[4];
        color_rgb1555_t color;
} transform_t;

extern void _internal_tlist_init(void);
extern void _internal_matrix_init(void);

extern void _internal_sort_clear(void);
extern void _internal_sort_add(void *packet, int32_t pz);
extern void _internal_sort_iterate(iterate_fn fn);

static struct {
        bool initialized;

        sega3d_info_t view;

        FIXED cached_inv_top;
        FIXED cached_inv_right;
        int16_t cached_sw_2;
        int16_t cached_sh_2;

        /* Current object */
        sega3d_object_t *object;
        /* Set iteration function */
        sega3d_iterate_fn iterate_fn;

        /* Buffered for copying */
        vdp1_cmdt_list_t *copy_cmdt_list;
} _state;

static void _sort_iterate(sort_single_t *single);

static inline FIXED __always_inline __used
_vertex_transform(const FIXED *p, const FIXED *matrix)
{
        register int32_t tmp1;
        register int32_t tmp2;
        register int32_t pt;

        __asm__ volatile (
                "clrmac\n"
                "mov %[p], %[pt]\n"
                "mov %[matrix], %[tmp1]\n"
                "mac.l @%[pt]+, @%[tmp1]+\n"
                "mac.l @%[pt]+, @%[tmp1]+\n"
                "mac.l @%[pt]+, @%[tmp1]+\n"
                "sts macl, %[tmp2]\n"
                "mov.l @%[tmp1]+, %[pt]\n" 
                "add %[tmp2], %[pt]\n" 
                : [tmp1] "=&r" (tmp1),
                  [tmp2] "=&r" (tmp2),
                  [pt] "=&r" (pt)
                : [p] "r" (p),
                  [matrix] "r" (matrix));

        return pt;
}

void
sega3d_init(void)
{
        /* Prevent re-initialization */
        if (_state.initialized) {
                return;
        }

        _state.initialized = true;

        sega3d_perspective_set(DEGtoANG(90.0f));

        _state.copy_cmdt_list = vdp1_cmdt_list_alloc(PACKET_SIZE);
        assert(_state.copy_cmdt_list != NULL);

        _internal_tlist_init();
        _internal_matrix_init();
}

void
sega3d_perspective_set(ANGLE fov)
{
#define AW_2 FIX16(12.446f) /* ([film-aperature-width = 0.980] * inch->mm) / 2) */
#define AH_2 FIX16(9.3345f) /* ([film-aperature-height = 0.735] * inch->mm) / 2) */

        const FIXED fov_angle = fix16_mul(fov, FIX16_2PI) >> 1; 

        uint16_t i_width;
        uint16_t i_height;

        vdp2_tvmd_display_res_get(&i_width, &i_height);

        /* Let's maintain a 4:3 ratio */
        if (i_height < 240) {
                i_height = 240;
        }

        const FIXED screen_width = fix16_int32_from(i_width);
        const FIXED screen_height = fix16_int32_from(i_height);

        _state.cached_sw_2 = i_width / 2;
        _state.cached_sh_2 = i_height / 2;

        cpu_divu_fix16_set(screen_width, screen_height);
        _state.view.ratio = cpu_divu_quotient_get();

        cpu_divu_fix16_set(FIX16(1.0f), fix16_tan(fov_angle));
        _state.view.focal_length = fix16_mul(AW_2, cpu_divu_quotient_get());

        _state.view.near = _state.view.focal_length;

        cpu_divu_fix16_set(AH_2, _state.view.focal_length);
        _state.view.top = cpu_divu_quotient_get();
        _state.view.right = -fix16_mul(-_state.view.top, _state.view.ratio);
        _state.view.left = -_state.view.right;
        _state.view.bottom = -_state.view.top;

        cpu_divu_fix16_set(screen_height, _state.view.top << 1);
        _state.cached_inv_top = cpu_divu_quotient_get();

        cpu_divu_fix16_set(screen_width, _state.view.right << 1);
        _state.cached_inv_right = cpu_divu_quotient_get();
}

void
sega3d_info_get(sega3d_info_t *info)
{
        (void)memcpy(info, &_state.view, sizeof(sega3d_info_t));
}

Uint16
sega3d_object_polycount_get(const sega3d_object_t *object)
{
        const PDATA *pdata;
        pdata = object->pdata;
        
        return pdata->nbPolygon;
}

void
sega3d_object_prepare(sega3d_object_t *object)
{
        const PDATA *pdata;
        pdata = object->pdata;

        assert(pdata != NULL);
        
        for (uint32_t i = 0; i < pdata->nbPolygon; i++) {
                vdp1_cmdt_t *cmdt;
                cmdt = &_state.copy_cmdt_list->cmdts[i];

                const ATTR *attr;
                attr = &pdata->attbl[i];

                cmdt->cmd_ctrl = attr->dir; /* We care about (Dir) and (Comm) bits */
                cmdt->cmd_link = 0x0000;
                cmdt->cmd_pmod = attr->atrb;
                cmdt->cmd_colr = attr->colno;

                /* For debugging */
                if ((object->flags & SEGA3D_OBJECT_FLAGS_WIREFRAME) == SEGA3D_OBJECT_FLAGS_WIREFRAME) {
                        cmdt->cmd_ctrl = 0x0005;
                        cmdt->cmd_pmod = 0x00C0;
                        cmdt->cmd_colr = 0xFFFF;
                }

                /* Even when there is not texture list, there is the default
                 * texture that zeroes out CMDSRCA and CMDSIZE */
                const TEXTURE *texture;
                texture = sega3d_tlist_tex_get(attr->texno);

#ifdef DEBUG
                const uint16_t tlist_count = sega3d_tlist_count_get();

                /* If the texture number is zero, it could imply no texture.
                 * Even if the texture list is empty, it's considered valid */
                if ((tlist_count > 0) && (attr->texno != 0)) {
                        assert(attr->texno < tlist_count);
                }
#endif /* DEBUG */

                cmdt->cmd_srca = texture->CGadr;
                cmdt->cmd_size = texture->HVsize;

                cmdt->cmd_grda = attr->gstb;
        }
}

void
sega3d_object_transform(sega3d_object_t *object)
{
        _internal_sort_clear();

        const PDATA *pdata;
        pdata = object->pdata;

        const MATRIX *matrix;
        matrix = sega3d_matrix_top();

        const FIXED tx = (*matrix)[3][0];
        const FIXED ty = (*matrix)[3][1];
        const FIXED tz = (*matrix)[3][2];

        const FIXED row0_x = (*matrix)[0][0];
        const FIXED row0_y = (*matrix)[1][0];
        const FIXED row0_z = (*matrix)[2][0];

        const FIXED row1_x = (*matrix)[0][1];
        const FIXED row1_y = (*matrix)[1][1];
        const FIXED row1_z = (*matrix)[2][1];

        const FIXED row2_x = (*matrix)[0][2];
        const FIXED row2_y = (*matrix)[1][2];
        const FIXED row2_z = (*matrix)[2][2];

        const POINT *points;
        points = pdata->pntbl;

        for (uint32_t i = 0; i < pdata->nbPolygon; i++) {
                POLYGON *polygon;
                polygon = &pdata->pltbl[i];

                vdp1_cmdt_t *copy_cmdt;
                copy_cmdt = &_state.copy_cmdt_list->cmdts[i];

                int16_vec2_t *cmd_vertex;
                cmd_vertex = (int16_vec2_t *)(&copy_cmdt->cmd_xa);

                transform_t trans;
                trans.z_avg = 0;

                for (uint32_t v = 0; v < 4; v++) {
                        uint16_t vertex;
                        vertex = polygon->Vertices[v];
                        const POINT *point;
                        point = &points[vertex];

                        const FIXED px = (*point)[X];
                        const FIXED py = (*point)[Y];
                        const FIXED pz = (*point)[Z];

                        trans.p[Z] = (tz + fix16_mul(row2_x, px) + fix16_mul(row2_y, py) + fix16_mul(row2_z, pz));

                        trans.clip_flags[v] = CLIP_FLAGS_NONE;

                        /* In case the projected Z value is on or behind the near plane */
                        if (trans.p[Z] <= _state.view.near) {
                                trans.clip_flags[v] |= CLIP_FLAGS_NEAR;

                                trans.p[Z] = _state.view.near;
                        }

                        cpu_divu_fix16_set(_state.cached_inv_right, trans.p[Z]);

                        trans.z_avg += trans.p[Z];

                        trans.p[X] = (tx + fix16_mul(row0_x, px) + fix16_mul(row0_y, py) + fix16_mul(row0_z, pz));
                        trans.inverse_right = cpu_divu_quotient_get();
                        trans.p[X] = fix16_mul(trans.p[X], trans.inverse_right);

                        cpu_divu_fix16_set(_state.cached_inv_top, trans.p[Z]);

                        trans.proj[X] = fix16_int32_to(trans.p[X]);

                        if (trans.proj[X] < -_state.cached_sw_2) {
                                trans.clip_flags[v] |= CLIP_FLAGS_LEFT;
                        }
                        if (trans.proj[X] > _state.cached_sw_2) {
                                trans.clip_flags[v] |= CLIP_FLAGS_RIGHT;
                        }

                        trans.p[Y] = (ty + fix16_mul(row1_x, px) + fix16_mul(row1_y, py) + fix16_mul(row1_z, pz));
                        trans.inverse_top = cpu_divu_quotient_get();

                        trans.p[Y] = fix16_mul(trans.p[Y], trans.inverse_top);
                        trans.proj[Y] = fix16_int32_to(trans.p[Y]);

                        if (trans.proj[Y] > _state.cached_sh_2) {
                                trans.clip_flags[v] |= CLIP_FLAGS_TOP;
                        }
                        if (trans.proj[Y] < -_state.cached_sh_2) {
                                trans.clip_flags[v] |= CLIP_FLAGS_BOTTOM;
                        }

                        cmd_vertex[v].x = trans.proj[X];
                        cmd_vertex[v].y = trans.proj[Y];
                }

                const clip_flags_t clip_flags = (trans.clip_flags[0] &
                                                 trans.clip_flags[1] &
                                                 trans.clip_flags[2] &
                                                 trans.clip_flags[3]);

                if (clip_flags != CLIP_FLAGS_NONE) {
                        continue;
                }

                const FIXED z_center = trans.z_avg >> 2; /* Divide by 4 */

                _internal_sort_add(copy_cmdt, fix16_int32_to(z_center));
        }
}

void
sega3d_object_iterate(sega3d_object_t *object)
{
        _state.object = object;
        _state.iterate_fn = object->iterate_fn;

        _state.object->count = 0;

        if (_state.iterate_fn == NULL) {
                _state.iterate_fn = sega3d_standard_iterate;
        }

        _internal_sort_iterate(_sort_iterate);
}

void
sega3d_standard_iterate(sega3d_object_t *object, const vdp1_cmdt_t *cmdt)
{
        vdp1_cmdt_t *transform_cmdts;
        transform_cmdts = object->cmdts;

        vdp1_cmdt_t *transform_cmdt;
        transform_cmdt = &transform_cmdts[object->offset + object->count];

        *transform_cmdt = *cmdt;

        object->count++;
}

static void
_sort_iterate(sort_single_t *single)
{
        const vdp1_cmdt_t *sort_cmdt;
        sort_cmdt = single->packet;

        _state.iterate_fn(_state.object, sort_cmdt);
}
