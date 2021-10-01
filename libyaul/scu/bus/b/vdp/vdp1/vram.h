/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP1_VRAM_H_
#define _YAUL_VDP1_VRAM_H_

#include <stdint.h>
#include <stdbool.h>

#include <vdp1/cmdt.h>

__BEGIN_DECLS

#define VDP1_VRAM_DEFAULT_CMDT_COUNT    (2048)
#define VDP1_VRAM_DEFAULT_TEXTURE_SIZE  (0x0006BFE0)
#define VDP1_VRAM_DEFAULT_GOURAUD_COUNT (1024)
#define VDP1_VRAM_DEFAULT_CLUT_COUNT    (256)

#define VDP1_FB_POINT(x, y)     (0x25C80000UL + (((x) & 0x1) << 18) + (y))
#define VDP1_CMD_TABLE(x, y)    (0x25C00000UL + ((x) << 5) + (((y) << 1) & 0x1F))

#define VDP1_VRAM_SIZE  0x00080000UL /* In bytes */

#define VDP1_FB_SIZE    0x00040000UL

typedef struct vdp1_gouraud_table {
        color_rgb1555_t colors[4];
} __aligned(8) vdp1_gouraud_table_t;

typedef struct vdp1_clut {
        struct {
                union {
                        union {
                                vdp1_color_bank_type_0_t type_0;
                                vdp1_color_bank_type_1_t type_1;
                                vdp1_color_bank_type_2_t type_2;
                                vdp1_color_bank_type_3_t type_3;
                                vdp1_color_bank_type_4_t type_4;
                                vdp1_color_bank_type_5_t type_5;
                                vdp1_color_bank_type_6_t type_6;
                                vdp1_color_bank_type_7_t type_7;
                                vdp1_color_bank_type_8_t type_8;
                                vdp1_color_bank_type_9_t type_9;
                                vdp1_color_bank_type_a_t type_a;
                                vdp1_color_bank_type_b_t type_b;
                                vdp1_color_bank_type_c_t type_c;
                                vdp1_color_bank_type_d_t type_d;
                                vdp1_color_bank_type_e_t type_e;
                                vdp1_color_bank_type_f_t type_f;
                        } sprite_type;

                        color_rgb1555_t color;
                };
        } entries[16];
} __aligned(32) vdp1_clut_t;

typedef struct vdp1_vram_partitions {
        vdp1_cmdt_t *cmdt_base;
        uint32_t cmdt_size;

        void *texture_base;
        uint32_t texture_size;

        vdp1_gouraud_table_t *gouraud_base;
        uint32_t gouraud_size;

        vdp1_clut_t *clut_base;
        uint32_t clut_size;

        vdp1_vram_t *remaining_base;
        uint32_t remaining_size;
} vdp1_vram_partitions_t;

extern void vdp1_vram_partitions_set(uint32_t, uint32_t, uint32_t, uint32_t);

extern void vdp1_vram_partitions_get(vdp1_vram_partitions_t *);

__END_DECLS

#endif /* !_YAUL_VDP1_VRAM_H_ */
