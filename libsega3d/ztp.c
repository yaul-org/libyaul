/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>

#include <cpu/divu.h>
#include <vdp.h>

#include "sega3d-internal.h"

/* This is the internal representation. We want to not expose the current
 * implementation */
typedef struct {
        uint16_t width;
        uint16_t height;
        uint16_t color_mode;  /* Color type: 16 or 256 */
        uint16_t color_count;
        uint16_t *data;       /* The palette index for the pixels */
} __packed internal_tex_t;

void
sega3d_ztp_pdata_patch(sega3d_object_t *object, const sega3d_ztp_t *ztp)
{
        PDATA * const pdatas = malloc(ztp->pdata_count * sizeof(PDATA));
        assert(pdatas != NULL);

        object->pdatas = pdatas;
        object->pdata_count = ztp->pdata_count;
        object->origin[X] = ztp->origin[X];
        object->origin[Y] = ztp->origin[Y];
        object->origin[Z] = ztp->origin[Z];

        object->cull_type = SEGA3D_CULL_TYPE_BOX;

        sega3d_box_t * const box = malloc(sizeof(sega3d_box_t));
        assert(box != NULL);

        box->length[X] = ztp->length[X];
        box->length[Y] = ztp->length[Y];
        box->length[Z] = ztp->length[Z];

        object->cull_data = box;

        uintptr_t base_p = ((uintptr_t)ztp + sizeof(sega3d_ztp_t) + ztp->tex_size);

        for (uint16_t i = 0; i < ztp->pdata_count; i++) {
                PDATA * const pdata = &pdatas[i];

                const XPDATA * const base_pdata = (const XPDATA *)base_p;

                base_p += sizeof(XPDATA);

                pdata->pntbl = (POINT *)base_p;
                base_p += (sizeof(POINT) * base_pdata->nbPoint);

                pdata->pltbl = (POLYGON *)base_p;
                base_p += (sizeof(POLYGON) * base_pdata->nbPolygon);

                pdata->attbl = (ATTR *)base_p;
                base_p += (sizeof(ATTR) * base_pdata->nbPolygon);

                /* Normals */
                base_p += (sizeof(VECTOR) * base_pdata->nbPoint);

                pdata->nbPoint = base_pdata->nbPoint;
                pdata->nbPolygon = base_pdata->nbPolygon;
        }
}

void
sega3d_ztp_texs_get(const sega3d_ztp_t *ztp, sega3d_ztp_tex_t *ztp_texs)
{
        assert(ztp_texs != NULL);

        sega3d_ztp_tex_t *ztp_tex;
        ztp_tex = ztp_texs;

        uintptr_t base_p = ((uintptr_t)ztp + sizeof(sega3d_ztp_t));

        for (uint16_t tex_num = 0; tex_num < ztp->tex_count; tex_num++) {
                const internal_tex_t * const tex = (internal_tex_t *)base_p;

                base_p += sizeof(internal_tex_t);

                ztp_tex->width = tex->width;
                ztp_tex->height = tex->height;
                ztp_tex->cg = (void *)base_p;

                uint32_t tex_size;
                tex_size = (tex->width * tex->height);

                /* Right now, only 16-color (CLUT? color bank?) is supported */
                switch (tex->color_mode) {
                default:
                        ztp_tex->color_mode = SEGA3D_ZTP_CMODE_CLUT;
                        tex_size /= 2;
                        break;
                }

                base_p += tex_size;
                ztp_tex++;
        }

        ztp_tex = ztp_texs;

        for (uint16_t tex_num = 0; tex_num < ztp->tex_count; tex_num++) {
                ztp_tex->palette = (void *)base_p;

                uint32_t palette_size;
                palette_size = 0;

                switch (ztp_tex->color_mode) {
                case SEGA3D_ZTP_CMODE_CLUT:
                case SEGA3D_ZTP_CMODE_16:
                        palette_size = 16 * sizeof(color_rgb1555_t);
                        break;
                case SEGA3D_ZTP_CMODE_64:
                        palette_size = 64 * sizeof(color_rgb1555_t);
                        break;
                case SEGA3D_ZTP_CMODE_128:
                        palette_size = 128 * sizeof(color_rgb1555_t);
                        break;
                case SEGA3D_ZTP_CMODE_256:
                        palette_size = 256 * sizeof(color_rgb1555_t);
                        break;
                case SEGA3D_ZTP_CMODE_RGB1555:
                        break;
                }

                base_p += palette_size;
        }
}
