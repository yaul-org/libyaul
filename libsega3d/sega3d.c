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

#include <cpu/divu.h>

#include "sega3d.h"
#include "sega3d-internal.h"

extern void _internal_matrix_init(void);

extern void _internal_sort_clear(void);
extern void _internal_sort_add(void *packet, int32_t pz);
extern void _internal_sort_iterate(iterate_fn fn);

static struct {
        bool initialized;

        FIXED distance;

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
        _state.distance = -PROJECTION_DISTANCE;
        _state.copy_cmdt_list = vdp1_cmdt_list_alloc(PACKET_SIZE);
        assert(_state.copy_cmdt_list != NULL);

        _internal_matrix_init();
}

Uint16
sega3d_object_polycount_get(sega3d_object_t *object)
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

        vdp1_cmdt_list_t *cmdt_list;
        cmdt_list = object->cmdt_list;

        assert(pdata != NULL);
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);
        
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
                if (false) {
                        cmdt->cmd_ctrl = 0x0005;
                        cmdt->cmd_pmod = 0x00C0;
                        cmdt->cmd_colr = 0xFFFF;
                }

                /* Even when there is not texture list, there is the default
                 * texture that zeroes out CMDSRCA and CMDSIZE */
                const TEXTURE *texture;
                texture = sega3d_tlist_tex_get(attr->texno);

#ifdef DEBUG
                /* If the texture number is zero, it could imply no texture.
                 * Even if the texture list is empty, it's considered valid */
                if (attr->texno != 0) {
                        assert(attr->texno < sega3d_tlist_count_get());
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

                /* Keep track of the projected Z values for averaging */
                FIXED z_projs[4] __unused;
                FIXED z_avg;
                z_avg = 0;

                for (uint32_t v = 0; v < 4; v++) {
                        uint16_t vertex;
                        vertex = polygon->Vertices[v];
                        const POINT *point;
                        point = &points[vertex];

                        const FIXED px = (*point)[X];
                        const FIXED py = (*point)[Y];
                        const FIXED pz = (*point)[Z];

                        FIXED proj[XYZ];

                        proj[Z] = (tz + fix16_mul(row2_x, px) + fix16_mul(row2_y, py) + fix16_mul(row2_z, pz));

                        const FIXED divisor = (_state.distance - proj[Z]);

                        /* Fire up CPU-DIVU to calculate reciprocal */
                        cpu_divu_fix16_set(_state.distance, divisor);

                        proj[X] = (tx + fix16_mul(row0_x, px) + fix16_mul(row0_y, py) + fix16_mul(row0_z, pz));
                        proj[Y] = (ty + fix16_mul(row1_x, px) + fix16_mul(row1_y, py) + fix16_mul(row1_z, pz));

                        /* Fetch results */
                        const FIXED inverse_z = cpu_divu_quotient_get();

                        proj[X] = fix16_mul(proj[X], inverse_z);
                        proj[Y] = fix16_mul(proj[Y], inverse_z);
                        proj[Z] = fix16_mul(proj[Z], inverse_z);

                        z_projs[v] = proj[Z];

                        z_avg += proj[Z];

                        int16_vector2_t proj_2d;
                        proj_2d.x = (proj[X] >> 16);
                        proj_2d.y = (proj[Y] >> 16);

                        vdp1_cmdt_param_vertex_set(copy_cmdt, v, &proj_2d);
                }

                const FIXED z_center = fix16_mul(z_avg, toFIXED(0.25f));

                _internal_sort_add(copy_cmdt, z_center >> 16);
        }
}

void
sega3d_object_iterate(sega3d_object_t *object)
{
        _state.object = object;
        _state.iterate_fn = object->iterate_fn;

        if (_state.iterate_fn == NULL) {
                _state.iterate_fn = sega3d_standard_iterate;
        }

        _internal_sort_iterate(_sort_iterate);
}

void
sega3d_standard_iterate(sega3d_object_t *object, const vdp1_cmdt_t *cmdt)
{
        vdp1_cmdt_list_t *transform_cmdt_list;
        transform_cmdt_list = object->cmdt_list;

        vdp1_cmdt_t *transform_cmdt;
        transform_cmdt = &transform_cmdt_list->cmdts[object->offset + transform_cmdt_list->count];

        *transform_cmdt = *cmdt;

        transform_cmdt_list->count++;
}

static void
_sort_iterate(sort_single_t *single)
{
        const vdp1_cmdt_t *sort_cmdt;
        sort_cmdt = single->packet;

        _state.iterate_fn(_state.object, sort_cmdt);
}
