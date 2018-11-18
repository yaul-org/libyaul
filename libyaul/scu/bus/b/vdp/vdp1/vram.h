/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_VRAM_H_
#define _VDP1_VRAM_H_

#include <stdint.h>
#include <stdbool.h>

#include <vdp1/cmdt.h>

#define VDP1_VRAM_SIZE 0x00080000 /* In bytes */

struct vdp1_gouraud_table {
        color_rgb555_t vgt_color[4];
} __aligned(8);

struct vdp1_clut {
        color_rgb555_t vcl_color[16];
} __aligned(32);

extern void *vdp1_vram_texture_base_get(void);
extern struct vdp1_gouraud_table *vdp1_vram_gouraud_base_get(void);
extern struct vdp1_clut *vdp1_vram_clut_base_get(void);
extern void *vdp1_vram_remaining_get(void);

#endif /* !_VDP1_VRAM_H_ */
