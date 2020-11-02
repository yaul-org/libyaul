/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <sys/cdefs.h>

#include <cpu/divu.h>

#include "sega3d.h"
#include "sega3d-internal.h"

extern void _internal_matrix_init(void);

struct {
        FIXED distance;
} _state;

void
sega3d_init(void)
{
        _state.distance = -PROJECTION_DISTANCE;

        _internal_matrix_init();
}

Uint16
sega3d_polycount_get(const PDATA *pdata)
{
        return pdata->nbPolygon;
}

void
sega3d_cmdt_prepare(const PDATA *pdata, vdp1_cmdt_list_t *cmdt_list, Uint16 offset)
{
        assert(pdata != NULL);
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);

        cmdt_list->count = pdata->nbPolygon;

        vdp1_cmdt_t *base_cmdts;
        base_cmdts = &cmdt_list->cmdts[offset];

        for (uint32_t i = 0; i < cmdt_list->count; i++) {
                vdp1_cmdt_t *cmdt;
                cmdt = &base_cmdts[i];

                const ATTR *attr;
                attr = &pdata->attbl[i];
                
                cmdt->cmd_ctrl = attr->dir; /* We care about (Dir) and (Comm) bits */
                cmdt->cmd_link = 0x0000;
                cmdt->cmd_pmod = attr->atrb;
                cmdt->cmd_colr = attr->colno;

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
sega3d_cmdt_transform(PDATA *pdata, vdp1_cmdt_list_t *cmdt_list, Uint16 offset)
{
        vdp1_cmdt_t *base_cmdt;
        base_cmdt = &cmdt_list->cmdts[offset];

        const MATRIX *matrix __unused;
        matrix = sega3d_matrix_top();

        const FIXED tx = (*matrix)[3][0];
        const FIXED ty = (*matrix)[3][1];
        const FIXED tz = (*matrix)[3][2];

        const FIXED row2_x = (*matrix)[2][0];
        const FIXED row2_y = (*matrix)[2][1];
        const FIXED row2_z = (*matrix)[2][2];

        const FIXED row1_x = (*matrix)[1][0];
        const FIXED row1_y = (*matrix)[1][1];
        const FIXED row1_z = (*matrix)[1][2];

        const FIXED row0_x = (*matrix)[0][0];
        const FIXED row0_y = (*matrix)[0][1];
        const FIXED row0_z = (*matrix)[0][2];

        const POINT *points;
        points = pdata->pntbl;

        for (uint32_t i = 0; i < pdata->nbPolygon; i++) {
                POLYGON *polygon;
                polygon = &pdata->pltbl[i];

                vdp1_cmdt_t *cmdt;
                cmdt = &base_cmdt[i];

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

                        cpu_divu_fix16_set(_state.distance, divisor);

                        proj[X] = (tx + fix16_mul(row0_x, px) + fix16_mul(row0_y, py) + fix16_mul(row0_z, pz));
                        proj[Y] = (ty + fix16_mul(row1_x, px) + fix16_mul(row1_y, py) + fix16_mul(row1_z, pz));

                        const FIXED quotient = cpu_divu_quotient_get();

                        proj[X] = fix16_mul(proj[X], quotient);
                        proj[Y] = fix16_mul(proj[Y], quotient);

                        int16_vector2_t proj_2d;
                        proj_2d.x = (proj[X] >> 16);
                        proj_2d.y = (proj[Y] >> 16);

                        vdp1_cmdt_param_vertex_set(cmdt, v, &proj_2d);
                }
        }
}
