/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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
        uint16_t *data;
} __packed _tex_v1_t;

typedef struct {
        uint16_t pdata_count; /* Total amount of PDATA */
        uint16_t tex_count;   /* Total amount of textures */
        uint32_t tex_size;    /* Total size of the textures in bytes */
        uint32_t pdata_size;  /* Total size of PDATA in bytes */
        /* Origin point used to "center" the culling/collision data and your
         * model's position. Should be 0,0,0 unless you have an offset */
        FIXED origin[XYZ];
        /* Length along the X,Y,Z axis. Together with the origin, that gives you
         * the bounding box for quick broad collision testing */
        FIXED length[XYZ];
        uint16_t frame_count; /* Number of animation frames */
        /* Interpolation factor of 2, means the number of frames between 2 key
         * frames */
        uint16_t framerate;
        void *animation;
} __packed __aligned(4) _header_v1_t;

static void _attr_texture_num_adjust(sega3d_ztp_handle_t *handle,
    uint16_t xpdata_index, uint16_t texture_num);
static void _attr_palette_num_adjust(sega3d_ztp_handle_t *handle,
    uint16_t xpdata_index, uint16_t palette_num);

sega3d_ztp_handle_t
sega3d_ztp_parse(sega3d_object_t *object, const sega3d_ztp_t *ztp)
{
        sega3d_ztp_handle_t handle;

        /* handle._texture_num = ztp->texture_num; */
        /* handle._palette_num = ztp->palette_num; */

        const _header_v1_t * const header = ztp->data;

        XPDATA * const xpdatas = ztp->xpdatas;
        assert(xpdatas != NULL);

        ATTR * const attrs = ztp->attrs;

        object->pdatas = xpdatas;
        object->pdata_count = header->pdata_count;

        if ((ztp->flags & SEGA3D_ZTP_FLAG_USE_AABB) != SEGA3D_ZTP_PATCH_NONE) {
                sega3d_cull_aabb_t * const aabb = ztp->cull_shape;
                assert(aabb != NULL);

                aabb->origin[X] = header->origin[X];
                aabb->origin[Y] = header->origin[Y];
                aabb->origin[Z] = header->origin[Z];

                aabb->length[X] = header->length[X];
                aabb->length[Y] = header->length[Y];
                aabb->length[Z] = header->length[Z];

                object->cull_shape = aabb;
        }

        uintptr_t base_p = ((uintptr_t)header + sizeof(_header_v1_t) + header->tex_size);

        uint16_t polygon_count = 0;

        for (uint16_t i = 0; i < header->pdata_count; i++) {
                XPDATA * const xpdata = &xpdatas[i];

                const XPDATA * const base_pdata = (const XPDATA *)base_p;

                base_p += sizeof(XPDATA);

                xpdata->pntbl = (POINT *)base_p;
                base_p += (sizeof(POINT) * base_pdata->nbPoint);

                xpdata->pltbl = (POLYGON *)base_p;
                base_p += (sizeof(POLYGON) * base_pdata->nbPolygon);

                const uint32_t attbl_size = sizeof(ATTR) * base_pdata->nbPolygon;

                if (attrs == NULL) {
                        xpdata->attbl = (ATTR *)base_p;
                } else {
                        xpdata->attbl = &attrs[polygon_count];

                        (void)memcpy(xpdata->attbl, (ATTR *)base_p, attbl_size);
                }
                base_p += attbl_size;

                /* Normals */
                base_p += (sizeof(VECTOR) * base_pdata->nbPoint);

                xpdata->nbPoint = base_pdata->nbPoint;
                xpdata->nbPolygon = base_pdata->nbPolygon;

                polygon_count += base_pdata->nbPolygon;
        }

        handle.ztp = ztp;
        handle.pdata_count = header->pdata_count;
        handle.polygon_count = polygon_count;
        handle.tex_count = header->tex_count;

        for (uint16_t i = 0; i < header->pdata_count; i++) { 
                _attr_texture_num_adjust(&handle, i, ztp->texture_num);
                _attr_palette_num_adjust(&handle, i, ztp->palette_num);
        }

        return handle;
}

void
sega3d_ztp_textures_parse(sega3d_ztp_handle_t *handle, void *vram,
    sega3d_ztp_tex_fn_t tex_fn)
{
        const sega3d_ztp_t * const ztp = handle->ztp;
        const _header_v1_t * const header = ztp->data;

        uintptr_t base_p = ((uintptr_t)header + sizeof(_header_v1_t));
        uintptr_t tex_base_p = base_p;
        /* Apparently the texture size field holds the size of the palette size
         * as well... */
        uintptr_t offset_palette = tex_base_p + header->tex_size -
            (header->tex_count * (16 * sizeof(color_rgb1555_t)));

        uintptr_t tex_vram_offset = (uintptr_t)vram;
        uint16_t palette_num = ztp->palette_num;

        TEXTURE * const textures = _internal_state->tlist->list;

        for (uint32_t i = 0; i < header->tex_count; i++) {
                const _tex_v1_t * const tex = (_tex_v1_t *)tex_base_p;

                tex_base_p += sizeof(_tex_v1_t);

                const uint16_t texture_num = ztp->texture_num + i;
                TEXTURE * const texture = &textures[texture_num];

                texture->Hsize = tex->width;
                texture->Vsize = tex->height;
                texture->CGadr = tex_vram_offset >> 3;
                texture->HVsize = TEXHVSIZE(tex->width, tex->height);

                uint32_t tex_size = tex->width * tex->height;
                uint16_t palette_size;

                /* Right now, only 16-color (CLUT? color bank?) is supported */
                switch (tex->color_mode) {
                case COL_16:
                        tex_size /= 2;
                        palette_size = (16 * sizeof(color_rgb1555_t));
                        break;
                case COL_32K:
                        tex_size *= 2;
                        offset_palette = 0;
                        palette_size = 0;
                        break;
                default:
                        offset_palette = 0;
                        palette_size = 0;
                }

                const sega3d_ztp_tex_t ztp_tex = {
                        .index = i,
                        .texture = texture,
                        .palette_num = palette_num,
                        .offset_cg = (void *)tex_base_p,
                        .cg_size = tex_size,
                        .offset_palette = (void *)offset_palette,
                        .palette_size = palette_size
                };

                if (tex_fn != NULL) {
                        tex_fn(handle, &ztp_tex);
                }

                tex_vram_offset += tex_size;
                tex_base_p += tex_size;

                offset_palette += palette_size;
                palette_num += (palette_size >> 3);
        }
}

/* XXX: It would be good to move this as a generic function */
void
sega3d_ztp_update(sega3d_ztp_handle_t *handle, uint16_t texture_num)
{
        for (uint32_t i = 0; i < handle->pdata_count; i++) {
                _attr_texture_num_adjust(handle, i, texture_num);
        }
}

static void
_attr_texture_num_adjust(sega3d_ztp_handle_t *handle, uint16_t xpdata_index,
    uint16_t texture_num)
{
        const sega3d_ztp_t * const ztp = handle->ztp;

        const XPDATA * const xpdatas = ztp->xpdatas;
        const XPDATA * const xpdata = &xpdatas[xpdata_index];

        ATTR * const attrs = xpdata->attbl;

        for (uint32_t i = 0; i < xpdata->nbPolygon; i++) {
                ATTR * const attr = &attrs[i];

                if ((attr->dir == FUNC_Texture) && ((attr->sort & UseTexture) == UseTexture)) {
                        attr->texno = texture_num++;
                }
        }
}

static void
_attr_palette_num_adjust(sega3d_ztp_handle_t *handle, uint16_t xpdata_index,
    uint16_t palette_num)
{
        const sega3d_ztp_t * const ztp = handle->ztp;

        const XPDATA * const xpdatas = ztp->xpdatas;
        const XPDATA * const xpdata = &xpdatas[xpdata_index];

        ATTR * const attrs = xpdata->attbl;

        const int16_t base_texture_num = ztp->texture_num;

        for (uint32_t i = 0; i < xpdata->nbPolygon; i++) {
                ATTR * const attr = &attrs[i];

                if ((attr->dir == FUNC_Texture) &&
                    ((attr->sort & UseTexture) == UseTexture) &&
                    ((attr->atrb & CL32KRGB) != CL32KRGB)) {
                        /* We want the relative texture number */
                        attr->colno = palette_num | ((attr->texno - base_texture_num) << 2);
                        dbgio_printf("attr->atrb: 0x%04X, attr->colno: 0x%04X, attr->texno: 0x%04X, prev: 0x%04X\n",
                            attr->atrb, attr->colno, attr->texno, base_texture_num);
                }
        }
}
