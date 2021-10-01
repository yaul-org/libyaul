/*
 * Copyright (c) 2020
 * See LICENSE for details.
 *
 * XL2
 */

#ifndef _SEGA3D_ZTP_H_
#define _SEGA3D_ZTP_H_

#include <stdint.h>
#include <sys/cdefs.h>

#include "sega3d-types.h"
#include "sgl.h"

typedef struct sega3d_ztp_handle sega3d_ztp_handle_t;
typedef struct sega3d_ztp_tex sega3d_ztp_tex_t;

typedef void (*sega3d_ztp_tex_fn_t)(sega3d_ztp_handle_t *handle, const sega3d_ztp_tex_t *tex);

typedef enum {
        SEGA3D_ZTP_CMODE_CLUT    = 0,
        SEGA3D_ZTP_CMODE_16      = 1,
        SEGA3D_ZTP_CMODE_64      = 2,
        SEGA3D_ZTP_CMODE_128     = 3,
        SEGA3D_ZTP_CMODE_256     = 4,
        SEGA3D_ZTP_CMODE_RGB1555 = 5        
} sega3d_ztp_cmode_t;

typedef enum {
        SEGA3D_ZTP_PATCH_NONE    = 0,
        SEGA3D_ZTP_FLAG_USE_AABB = 1 << 0
} sega3d_ztp_flags_t;

struct sega3d_ztp_tex {
        /// Texture number.
        uint16_t index;
        /// Direct texture structure used.
        TEXTURE *texture;
        /// Current palette number.
        uint16_t palette_num;
        /// ZTP buffer offset to texture.
        void *offset_cg;
        /// Texture size in bytes.
        uint32_t cg_size;
        /// ZTP buffer offset to palette. NULL is specified if RGB1555 is used.
        void *offset_palette;
        /// Palette size in bytes. If RGB1555 is used, palette size is zero.
        uint16_t palette_size;
} __packed __aligned(4);

typedef struct {
        /// Pointer to ZTP data.
        const void *data;
        /// Flags specific to parsing.
        sega3d_ztp_flags_t flags;
        /// Specify an array of polygonal data structures.
        void *xpdatas;
        /// Specify an array of attribute structures for each polygon for
        /// duplication. Otherwise, use the existing attributes.
        void *attrs;
        /// Starting texture number from the global texture list.
        uint16_t texture_num;
        /// Starting palette number relative to VDP1 VRAM or VDP2 CRAM.
        uint16_t palette_num;
        /// Specify pointer to shape type.
        void *cull_shape;
} __packed __aligned(4) sega3d_ztp_t;

static_assert(sizeof(sega3d_ztp_t) == 24);

struct sega3d_ztp_handle {
        const sega3d_ztp_t *ztp;

        uint16_t xpdata_count;
        uint16_t polygon_count;
        uint16_t tex_count;

        /* /// Private use. */
        /* uint16_t _texture_num; */
        /* /// Private use. */
        /* uint16_t _palette_num; */
} __packed __aligned(4);

#endif /* !_SEGA3D_ZTP_H_ */
