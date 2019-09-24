/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_VRAM_H_
#define _VDP1_VRAM_H_

#include <stdint.h>
#include <stdbool.h>

#include <vdp1/cmdt.h>

__BEGIN_DECLS

#define VDP1_VRAM(x)            (0x25C00000 + (x))
#define VDP1_FB_POINT(x, y)     (0x25C80000 + (((x) & 0x1) << 18) + (y))
#define VDP1_CMD_TABLE(x, y)    (0x25C00000 + ((x) << 5) + (((y) << 1) & 0x1F))

#define VDP1_VRAM_SIZE  0x00080000 /* In bytes */

#define VDP1_FB_SIZE    0x00040000

struct vdp1_gouraud_table {
        color_rgb555_t colors[4];
} __aligned(8);

struct vdp1_clut {
        struct {
                union {
                        union {
                                struct vdp1_color_bank_type_0 type_0;
                                struct vdp1_color_bank_type_1 type_1;
                                struct vdp1_color_bank_type_2 type_2;
                                struct vdp1_color_bank_type_3 type_3;
                                struct vdp1_color_bank_type_4 type_4;
                                struct vdp1_color_bank_type_5 type_5;
                                struct vdp1_color_bank_type_6 type_6;
                                struct vdp1_color_bank_type_7 type_7;
                                struct vdp1_color_bank_type_8 type_8;
                                struct vdp1_color_bank_type_9 type_9;
                                struct vdp1_color_bank_type_a type_a;
                                struct vdp1_color_bank_type_b type_b;
                                struct vdp1_color_bank_type_c type_c;
                                struct vdp1_color_bank_type_d type_d;
                                struct vdp1_color_bank_type_e type_e;
                                struct vdp1_color_bank_type_f type_f;
                        } sprite_type;

                        color_rgb555_t color;
                };
        } entries[16];
} __aligned(32);

extern void vdp1_vram_partitions_set(uint32_t, uint32_t, uint32_t, uint32_t);

extern void *vdp1_vram_texture_base_get(void);
extern uint32_t vdp1_vram_texture_size_get(void);

extern struct vdp1_gouraud_table *vdp1_vram_gouraud_base_get(void);
extern uint32_t vdp1_vram_gouraud_size_get(void);

extern struct vdp1_clut *vdp1_vram_clut_base_get(void);
extern uint32_t vdp1_vram_clut_size_get(void);

extern void *vdp1_vram_remaining_get(void);

__END_DECLS

#endif /* !_VDP1_VRAM_H_ */
