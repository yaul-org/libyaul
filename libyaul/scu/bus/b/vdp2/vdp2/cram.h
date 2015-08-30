/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_CRAM_H_
#define _VDP2_CRAM_H_

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Color RAM (CRAM) stores color data of sprites and scroll screens.
 *
 * R/W access from the CPU or DMA controller(s) is possible, but the
 * image may be disturbed by the access timing. Access through CPU or
 * DMA controller(s) is possible only in word units and long word
 * units. Access in bytes is now allowed. */

#define CRAM_MODE_0_OFFSET(x, y, z)     (0x25F00000 + ((x) << 9) +             \
    ((y) << 5) + ((z) << 1))
#define CRAM_MODE_1_OFFSET(x, y, z)     CRAM_MODE_0_OFFSET(x, y, z)
#define CRAM_MODE_2_OFFSET(x, y, z)     (0x25F00000 + ((x) << 10) +            \
    ((y) << 6) + ((z) << 2))

/*
 * CRAM Modes
 *
 * Mode 0: RGB 555 1,024 colors mirrored
 * Mode 1: RGB 555 2,048 colors
 * Mode 2: RGB 888 1,024 colors
 */

#define CRAM_MODE_0_SIZE        0x0800
#define CRAM_MODE_1_SIZE        0x1000
#define CRAM_MODE_2_SIZE        0x1000

#define COLOR_RGB555(r, g, b)   (0x8000 | ((b) << 10) | ((g) << 5) | (r))
#define COLOR_RGB888(r, g, b)   (0x8000 | ((b) << 16) | ((g) << 8) | (r))
#define COLOR_RGB888_TO_RGB555(r, g, b) (0x8000 | (((b) >> 3) << 10) |         \
    (((g) >> 3) << 5) | ((r) >> 3))

/*
 * Sprite Types
 *
 * XXX
 */

#define vdp1_cram_sprite_type_0 vdp2_cram_sprite_type_0
struct vdp2_cram_sprite_type_0 {
        unsigned int pr:2; /* Bit 15 */ /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed __aligned(16);

#define vdp1_cram_sprite_type_1 vdp2_cram_sprite_type_1
struct vdp2_cram_sprite_type_1 {
        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed __aligned(16);

#define vdp1_cram_sprite_type_2 vdp2_cram_sprite_type_2
struct vdp2_cram_sprite_type_2 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed __aligned(16);

#define vdp1_cram_sprite_type_3 vdp2_cram_sprite_type_3
struct vdp2_cram_sprite_type_3 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:2; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed __aligned(16);

#define vdp1_cram_sprite_type_4 vdp2_cram_sprite_type_4
struct vdp2_cram_sprite_type_4 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:2; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:10; /* Dot color data bits */
} __packed __aligned(16);

#define vdp1_cram_sprite_type_5 vdp2_cram_sprite_type_5
struct vdp2_cram_sprite_type_5 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:1; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed __aligned(16);

#define vdp1_cram_sprite_type_6 vdp2_cram_sprite_type_6
struct vdp2_cram_sprite_type_6 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:10; /* Dot color data bits */
} __packed __aligned(16);

#define vdp1_cram_sprite_type_7 vdp2_cram_sprite_type_7
struct vdp2_cram_sprite_type_7 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:9; /* Dot color data bits */
} __packed __aligned(16);

#define vdp1_cram_sprite_type_8 vdp2_cram_sprite_type_8
struct vdp2_cram_sprite_type_8 {
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int dc:7; /* Dot color data bits */
} __packed __aligned(8);

#define vdp1_cram_sprite_type_9 vdp2_cram_sprite_type_9
struct vdp2_cram_sprite_type_9 {
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int cc:1; /* Color calculation ratio bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed __aligned(8);

#define vdp1_cram_sprite_type_a vdp2_cram_sprite_type_a
struct vdp2_cram_sprite_type_a {
        unsigned int pr:2; /* Priority bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed __aligned(8);

#define vdp1_cram_sprite_type_b vdp2_cram_sprite_type_b
struct vdp2_cram_sprite_type_b {
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed __aligned(8);

#define vdp1_cram_sprite_type_c vdp2_cram_sprite_type_c
struct vdp2_cram_sprite_type_c {
        unsigned int sp:1; /* Priority, CRAM addr. shared bit(s) */
        unsigned int dc:7; /* Dot color data bits */
} __packed __aligned(8);

#define vdp1_cram_sprite_type_d vdp2_cram_sprite_type_d
struct vdp2_cram_sprite_type_d {
        unsigned int sp:1; /* Priority, CRAM addr. shared bit(s) */
        unsigned int sc:1; /* Color calculation ratio, CRAM addr. shared
                            * bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed __aligned(8);

#define vdp1_cram_sprite_type_e vdp2_cram_sprite_type_e
struct vdp2_cram_sprite_type_e {
        unsigned int sp:2; /* Priority, CRAM addr. shared bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed __aligned(8);

#define vdp1_cram_sprite_type_f vdp2_cram_sprite_type_f
struct vdp2_cram_sprite_type_f {
        unsigned int sc:2; /* Color calculation ratio, CRAM addr. shared
                            * bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed __aligned(8);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_VDP2_CRAM_H_ */
