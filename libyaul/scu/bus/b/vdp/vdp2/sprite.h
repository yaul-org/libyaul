/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP2_SPRITE_H_
#define _YAUL_VDP2_SPRITE_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

#define VDP2_SRPITE_TYPE_0_PR_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_0_CC_MASK      ((1 << 3) - 1)
#define VDP2_SPRITE_TYPE_0_DC_MASK      ((1 << 11) - 1)

#define VDP2_SRPITE_TYPE_1_PR_MASK      ((1 << 3) - 1)
#define VDP2_SPRITE_TYPE_1_CC_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_1_DC_MASK      ((1 << 11) - 1)

#define VDP2_SRPITE_TYPE_2_SDW_MASK     ((1 << 1) - 1)
#define VDP2_SRPITE_TYPE_2_PR_MASK      ((1 << 1) - 1)
#define VDP2_SPRITE_TYPE_2_CC_MASK      ((1 << 3) - 1)
#define VDP2_SPRITE_TYPE_2_DC_MASK      ((1 << 11) - 1)

#define VDP2_SRPITE_TYPE_3_SDW_MASK     ((1 << 1) - 1)
#define VDP2_SRPITE_TYPE_3_PR_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_3_CC_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_3_DC_MASK      ((1 << 11) - 1)

#define VDP2_SRPITE_TYPE_4_SDW_MASK     ((1 << 1) - 1)
#define VDP2_SRPITE_TYPE_4_PR_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_4_CC_MASK      ((1 << 3) - 1)
#define VDP2_SPRITE_TYPE_4_DC_MASK      ((1 << 10) - 1)

#define VDP2_SRPITE_TYPE_5_SDW_MASK     ((1 << 1) - 1)
#define VDP2_SRPITE_TYPE_5_PR_MASK      ((1 << 3) - 1)
#define VDP2_SPRITE_TYPE_5_CC_MASK      ((1 << 1) - 1)
#define VDP2_SPRITE_TYPE_5_DC_MASK      ((1 << 11) - 1)

#define VDP2_SRPITE_TYPE_6_SDW_MASK     ((1 << 1) - 1)
#define VDP2_SRPITE_TYPE_6_PR_MASK      ((1 << 3) - 1)
#define VDP2_SPRITE_TYPE_6_CC_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_6_DC_MASK      ((1 << 10) - 1)

#define VDP2_SRPITE_TYPE_7_SDW_MASK     ((1 << 1) - 1)
#define VDP2_SRPITE_TYPE_7_PR_MASK      ((1 << 3) - 1)
#define VDP2_SPRITE_TYPE_7_CC_MASK      ((1 << 3) - 1)
#define VDP2_SPRITE_TYPE_7_DC_MASK      ((1 << 9) - 1)

#define VDP2_SRPITE_TYPE_8_PR_MASK      ((1 << 1) - 1)
#define VDP2_SPRITE_TYPE_8_DC_MASK      ((1 << 7) - 1)

#define VDP2_SRPITE_TYPE_9_PR_MASK      ((1 << 1) - 1)
#define VDP2_SRPITE_TYPE_9_CC_MASK      ((1 << 1) - 1)
#define VDP2_SPRITE_TYPE_9_DC_MASK      ((1 << 6) - 1)

#define VDP2_SRPITE_TYPE_A_PR_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_A_DC_MASK      ((1 << 6) - 1)

#define VDP2_SRPITE_TYPE_B_CC_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_B_DC_MASK      ((1 << 6) - 1)

#define VDP2_SRPITE_TYPE_C_SP_MASK      ((1 << 1) - 1)
#define VDP2_SPRITE_TYPE_C_DC_MASK      ((1 << 7) - 1)

#define VDP2_SRPITE_TYPE_D_SP_MASK      ((1 << 1) - 1)
#define VDP2_SRPITE_TYPE_D_SC_MASK      ((1 << 1) - 1)
#define VDP2_SPRITE_TYPE_D_DC_MASK      ((1 << 6) - 1)

#define VDP2_SRPITE_TYPE_E_SP_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_E_DC_MASK      ((1 << 6) - 1)

#define VDP2_SRPITE_TYPE_F_SC_MASK      ((1 << 2) - 1)
#define VDP2_SPRITE_TYPE_F_DC_MASK      ((1 << 6) - 1)

typedef uint32_t vdp2_sprite_register_t;
typedef uint32_t vdp2_sprite_type_t;

typedef struct vdp2_sprite_type_0 {
        unsigned int pr:2; /* Bit 15 */ /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed vdp2_sprite_type_0_t;

typedef struct vdp2_sprite_type_1 {
        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed vdp2_sprite_type_1_t;

typedef struct vdp2_sprite_type_2 {
        unsigned int sdw:1; /* Shadow (or sprite window) bit */
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed vdp2_sprite_type_2_t;

typedef struct vdp2_sprite_type_3 {
        unsigned int sdw:1; /* Shadow (or sprite window) bit */
        unsigned int pr:2; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed vdp2_sprite_type_3_t;

typedef struct vdp2_sprite_type_4 {
        unsigned int sdw:1; /* Shadow (or sprite window) bit */
        unsigned int pr:2; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:10; /* Dot color data bits */
} __packed vdp2_sprite_type_4_t;

typedef struct vdp2_sprite_type_5 {
        unsigned int sdw:1; /* Shadow (or sprite window) bit */
        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:1; /* Color calculation ratio bit(s) */
        unsigned int dc:11; /* Dot color data bits */
} __packed vdp2_sprite_type_5_t;

typedef struct vdp2_sprite_type_6 {
        unsigned int sdw:1; /* Shadow (or sprite window) bit */
        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:10; /* Dot color data bits */
} __packed vdp2_sprite_type_6_t;

typedef struct vdp2_sprite_type_7 {
        unsigned int sdw:1; /* Shadow (or sprite window) bit */
        unsigned int pr:3; /* Priority bit(s) */
        unsigned int cc:3; /* Color calculation ratio bit(s) */
        unsigned int dc:9; /* Dot color data bits */
} __packed vdp2_sprite_type_7_t;

typedef struct vdp2_sprite_type_8 {
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int dc:7; /* Dot color data bits */
} __packed vdp2_sprite_type_8_t;

typedef struct vdp2_sprite_type_9 {
        unsigned int pr:1; /* Priority bit(s) */
        unsigned int cc:1; /* Color calculation ratio bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed vdp2_sprite_type_9_t;

typedef struct vdp2_sprite_type_a {
        unsigned int pr:2; /* Priority bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed vdp2_sprite_type_a_t;

typedef struct vdp2_sprite_type_b {
        unsigned int cc:2; /* Color calculation ratio bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed vdp2_sprite_type_b_t;

typedef struct vdp2_sprite_type_c {
        unsigned int sp:1; /* Priority, CRAM addr. shared bit(s) */
        unsigned int dc:7; /* Dot color data bits */
} __packed vdp2_sprite_type_c_t;

typedef struct vdp2_sprite_type_d {
        unsigned int sp:1; /* Priority, CRAM addr. shared bit(s) */
        unsigned int sc:1; /* Color calculation ratio, CRAM addr. shared
                            * bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed vdp2_sprite_type_d_t;

typedef struct vdp2_sprite_type_e {
        unsigned int sp:2; /* Priority, CRAM addr. shared bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed vdp2_sprite_type_e_t;

typedef struct vdp2_sprite_type_f {
        unsigned int sc:2; /* Color calculation ratio, CRAM addr. shared
                            * bit(s) */
        unsigned int dc:6; /* Dot color data bits */
} __packed vdp2_sprite_type_f_t;

extern void vdp2_sprite_priority_set(vdp2_sprite_register_t, uint8_t);

__END_DECLS

#endif /* !_YAUL_VDP2_SPRITE_H_ */
