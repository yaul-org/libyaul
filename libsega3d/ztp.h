/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * XL2
 */

#ifndef SEGA3D_ZTP_H_
#define SEGA3D_ZTP_H_

#include <stdint.h>
#include <sys/cdefs.h>

#include "sgl.h"

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
        uint16_t frame_count;
        /* Interpolation factor of 2, means the number of frames between 2 key
         * frames */
        uint16_t framerate;
        void *animation;
} __packed __aligned(4) sega3d_ztp_t;

static_assert(sizeof(sega3d_ztp_t) == 44);

#endif /* SEGA3D_ZTP_H_ */
