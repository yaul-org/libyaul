/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_SPRITE_H_
#define _VDP2_SPRITE_H_

#include <sys/cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define vdp1_sprite_type_0 vdp2_sprite_type_0
struct vdp2_sprite_type_0 {
        unsigned int pr:2; /* Bit 15 */ /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_1 vdp2_sprite_type_1
struct vdp2_sprite_type_1 {
        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_2 vdp2_sprite_type_2
struct vdp2_sprite_type_2 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_3 vdp2_sprite_type_3
struct vdp2_sprite_type_3 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:2; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_4 vdp2_sprite_type_4
struct vdp2_sprite_type_4 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:2; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:10; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_5 vdp2_sprite_type_5
struct vdp2_sprite_type_5 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:1; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_6 vdp2_sprite_type_6
struct vdp2_sprite_type_6 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:10; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_7 vdp2_sprite_type_7
struct vdp2_sprite_type_7 {
        union {
                unsigned int sd:1; /* Shadow bit */
                unsigned int sw:1; /* Sprite window bit */
        };

        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:9; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_8 vdp2_sprite_type_8
struct vdp2_sprite_type_8 {
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int dc:7; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_9 vdp2_sprite_type_9
struct vdp2_sprite_type_9 {
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int cc:1; /* Color calculation ratio bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_a vdp2_sprite_type_a
struct vdp2_sprite_type_a {
        unsigned int pr:2; /* Priority bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_b vdp2_sprite_type_b
struct vdp2_sprite_type_b {
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_c vdp2_sprite_type_c
struct vdp2_sprite_type_c {
        unsigned int sp:1; /* Priority, CRAM addr. shared bit(s) */
        unsigned int dc:7; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_d vdp2_sprite_type_d
struct vdp2_sprite_type_d {
        unsigned int sp:1; /* Priority, CRAM addr. shared bit(s) */
        unsigned int sc:1; /* Color calculation ratio, CRAM addr. shared
                            * bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_e vdp2_sprite_type_e
struct vdp2_sprite_type_e {
        unsigned int sp:2; /* Priority, CRAM addr. shared bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed;

#define vdp1_sprite_type_f vdp2_sprite_type_f
struct vdp2_sprite_type_f {
        unsigned int sc:2; /* Color calculation ratio, CRAM addr. shared
                            * bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed;

extern void vdp2_sprite_type_set(uint8_t);
extern void vdp2_sprite_priority_set(uint8_t, uint8_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_VDP2_SPRITE_H_ */
