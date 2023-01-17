/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP2_SCRN_BITMAP_H_
#define _YAUL_VDP2_SCRN_BITMAP_H_

#include <assert.h>
#include <stdint.h>

#include <vdp2/scrn_macros.h>
#include <vdp2/scrn_shared.h>

#include <vdp2/map.h>
#include <vdp2/vram.h>

__BEGIN_DECLS

typedef enum vdp2_scrn_bitmap_size {
        /// Not yet documented.
        VDP2_SCRN_BITMAP_SIZE_512X256  = 0x0000,
        /// Not yet documented.
        VDP2_SCRN_BITMAP_SIZE_512X512  = 0x0004,
        /// Not yet documented.
        VDP2_SCRN_BITMAP_SIZE_1024X256 = 0x0008,
        /// Not yet documented.
        VDP2_SCRN_BITMAP_SIZE_1024X512 = 0x000C
} __packed vdp2_scrn_bitmap_size_t;

typedef struct vdp2_scrn_bitmap_format {
        vdp2_scrn_t scroll_screen;
        vdp2_scrn_ccc_t ccc;
        vdp2_scrn_bitmap_size_t bitmap_size;
        vdp2_cram_t palette_base;
        vdp2_vram_t bitmap_base;
} __packed __aligned(4) vdp2_scrn_bitmap_format_t;

static_assert(sizeof(vdp2_scrn_bitmap_format_t) == 16);

extern void vdp2_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *bitmap_format);
extern void vdp2_scrn_bitmap_ccc_set(const vdp2_scrn_bitmap_format_t *bitmap_format);
extern void vdp2_scrn_bitmap_base_set(const vdp2_scrn_bitmap_format_t *bitmap_format);
extern void vdp2_scrn_bitmap_size_set(const vdp2_scrn_bitmap_format_t *bitmap_format);

__END_DECLS

#endif /* !_YAUL_VDP2_SCRN_BITMAP_H_ */
