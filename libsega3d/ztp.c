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

void
sega3d_ztp_pdata_patch(sega3d_object_t *object, sega3d_ztp_t *ztp)
{
        PDATA * const pdatas = malloc(ztp->pdata_count * sizeof(PDATA));
        assert(pdatas != NULL);

        object->pdatas = pdatas;
        object->pdata_count = ztp->pdata_count;

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
