/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_CMDT_H_
#define _VDP1_CMDT_H_

#include <sys/cdefs.h>

#include <stdint.h>

#include <int16.h>

#include <scu.h>

#include <vdp1/map.h>

#include <vdp2/sprite.h>

#include <color.h>

__BEGIN_DECLS

#define CMDT_ZOOM_POINT_NONE            (0x0)
#define CMDT_ZOOM_POINT_UPPER_LEFT      (0x5)
#define CMDT_ZOOM_POINT_UPPER_CENTER    (0x6)
#define CMDT_ZOOM_POINT_UPPER_RIGHT     (0x7)
#define CMDT_ZOOM_POINT_CENTER_LEFT     (0x9)
#define CMDT_ZOOM_POINT_CENTER          (0xA)
#define CMDT_ZOOM_POINT_CENTER_RIGHT    (0xB)
#define CMDT_ZOOM_POINT_LOWER_LEFT      (0xD)
#define CMDT_ZOOM_POINT_LOWER_CENTER    (0xE)
#define CMDT_ZOOM_POINT_LOWER_RIGHT     (0xF)

#define CMDT_VTX_NORMAL_SPRITE          (0)
#define CMDT_VTX_NORMAL_SPRITE_COUNT    (1)

#define CMDT_VTX_SCALE_SPRITE_UL        (0)
#define CMDT_VTX_SCALE_SPRITE_LR        (2)
#define CMDT_VTX_SCALE_SPRITE_COUNT     (2)

#define CMDT_VTX_ZOOM_SPRITE_POINT      (0)
#define CMDT_VTX_ZOOM_SPRITE_DISPLAY    (1)
#define CMDT_VTX_ZOOM_SPRITE_COUNT      (2)

#define CMDT_VTX_DISTORTED_SPRITE_A     (0)
#define CMDT_VTX_DISTORTED_SPRITE_B     (1)
#define CMDT_VTX_DISTORTED_SPRITE_C     (2)
#define CMDT_VTX_DISTORTED_SPRITE_D     (3)
#define CMDT_VTX_DISTORTED_SPRITE_COUNT (4)

#define CMDT_VTX_POLYGON_A              (0)
#define CMDT_VTX_POLYGON_B              (1)
#define CMDT_VTX_POLYGON_C              (2)
#define CMDT_VTX_POLYGON_D              (3)
#define CMDT_VTX_POLYGON_COUNT          (4)

#define CMDT_VTX_POLYLINE_A             (0)
#define CMDT_VTX_POLYLINE_B             (1)
#define CMDT_VTX_POLYLINE_C             (2)
#define CMDT_VTX_POLYLINE_D             (3)
#define CMDT_VTX_POLYLINE_COUNT         (4)

#define CMDT_VTX_LINE_A                 (0)
#define CMDT_VTX_LINE_B                 (1)
#define CMDT_VTX_LINE_COUNT             (2)

#define CMDT_VTX_LOCAL_COORD            (0)
#define CMDT_VTX_LOCAL_COORD_COUNT      (1)

#define CMDT_VTX_SYSTEM_CLIP            (2)
#define CMDT_VTX_SYSTEM_CLIP_COUNT      (1)

#define CMDT_VTX_USER_CLIP_UL           (0)
#define CMDT_VTX_USER_CLIP_LR           (2)
#define CMDT_VTX_USER_CLIP_COUNT        (2)

struct vdp1_color_bank_type_0 {
        struct vdp2_sprite_type_0 data;
} __packed;

struct vdp1_color_bank_type_1 {
        struct vdp2_sprite_type_1 data;
} __packed;

struct vdp1_color_bank_type_2 {
        struct vdp2_sprite_type_2 data;
} __packed;

struct vdp1_color_bank_type_3 {
        struct vdp2_sprite_type_3 data;
} __packed;

struct vdp1_color_bank_type_4 {
        struct vdp2_sprite_type_4 data;
} __packed;

struct vdp1_color_bank_type_5 {
        struct vdp2_sprite_type_5 data;
} __packed;

struct vdp1_color_bank_type_6 {
        struct vdp2_sprite_type_6 data;
} __packed;

struct vdp1_color_bank_type_7 {
        struct vdp2_sprite_type_7 data;
} __packed;

struct vdp1_color_bank_type_8 {
        unsigned int :8;
        struct vdp2_sprite_type_8 data;
} __packed;

struct vdp1_color_bank_type_9 {
        unsigned int :8;
        struct vdp2_sprite_type_9 data;
} __packed;

struct vdp1_color_bank_type_a {
        unsigned int :8;
        struct vdp2_sprite_type_a data;
} __packed;

struct vdp1_color_bank_type_b {
        unsigned int :8;
        struct vdp2_sprite_type_b data;
} __packed;

struct vdp1_color_bank_type_c {
        unsigned int :8;
        struct vdp2_sprite_type_c data;
} __packed;

struct vdp1_color_bank_type_d {
        unsigned int :8;
        struct vdp2_sprite_type_d data;
} __packed;

struct vdp1_color_bank_type_e {
        unsigned int :8;
        struct vdp2_sprite_type_e data;
} __packed;

struct vdp1_color_bank_type_f {
        unsigned int :8;
        struct vdp2_sprite_type_f data;
} __packed;

struct vdp1_cmdt {
        uint16_t cmd_ctrl;
        uint16_t cmd_link;
        uint16_t cmd_pmod;
        uint16_t cmd_colr;
        uint16_t cmd_srca;
        uint16_t cmd_size;
        int16_t cmd_xa;
        int16_t cmd_ya;
        int16_t cmd_xb;
        int16_t cmd_yb;
        int16_t cmd_xc;
        int16_t cmd_yc;
        int16_t cmd_xd;
        int16_t cmd_yd;
        uint16_t cmd_grda;
        uint16_t reserved;
} __aligned(32);

typedef union {
        struct {
                unsigned int msb_enable:1;              /* Bit 15 */
                unsigned int :2;
                unsigned int hss_enable:1;              /* Bit 12 */
                unsigned int pre_clipping_disable:1;    /* Bit 11 */
                unsigned int user_clipping_mode:2;   /* Bits 10-9 */
                unsigned int mesh_enable:1;              /* Bit 8 */
                unsigned int end_code_disable:1;         /* Bit 7 */
                unsigned int trans_pixel_disable:1;      /* Bit 6 */
                unsigned int color_mode:3;            /* Bits 5-3 */
                unsigned int cc_mode:3;               /* Bits 2-0 */
        } bits __aligned(2);

        uint16_t raw;
} vdp1_cmdt_draw_mode;

typedef union {
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

        uint16_t raw;
} vdp1_cmdt_color_bank;

struct vdp1_cmdt_list {
        struct vdp1_cmdt *cmdts;
        uint16_t count;
} __aligned(4);

struct vdp1_cmdt_orderlist {
        struct {
                unsigned int end:1;
                unsigned int :30;
        } control __aligned(4);

        unsigned int :32;

        struct vdp1_cmdt *cmdt;
} __aligned(4);

static inline uint16_t __always_inline
vdp1_cmdt_current_get(void)
{
        return MEMORY_READ(16, VDP1(COPR)) >> 2;
}

static inline uint16_t __always_inline
vdp1_cmdt_last_get(void)
{
        return MEMORY_READ(16, VDP1(LOPR)) >> 2;
}

extern struct vdp1_cmdt *vdp1_cmdt_base_get(void);

extern struct vdp1_cmdt_list *vdp1_cmdt_list_alloc(uint16_t);
extern void vdp1_cmdt_list_free(struct vdp1_cmdt_list *);
extern void vdp1_cmdt_list_init(struct vdp1_cmdt_list *, struct vdp1_cmdt *);

extern struct vdp1_cmdt_orderlist *vdp1_cmdt_orderlist_alloc(uint16_t);
extern void vdp1_cmdt_orderlist_free(struct vdp1_cmdt_orderlist *);
extern void vdp1_cmdt_orderlist_init(struct vdp1_cmdt_orderlist *, uint16_t);
extern void vdp1_cmdt_orderlist_vram_patch(struct vdp1_cmdt_orderlist *, const uint32_t, const uint16_t);

extern void vdp1_cmdt_normal_sprite_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_scaled_sprite_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_distorted_sprite_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_polygon_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_polyline_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_line_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_user_clip_coord_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_system_clip_coord_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_local_coord_set(struct vdp1_cmdt *);
extern void vdp1_cmdt_end_set(struct vdp1_cmdt *);

extern void vdp1_cmdt_param_draw_mode_set(struct vdp1_cmdt *, const vdp1_cmdt_draw_mode);
extern void vdp1_cmdt_param_zoom_set(struct vdp1_cmdt *, const uint8_t);
extern void vdp1_cmdt_param_char_base_set(struct vdp1_cmdt *, uint32_t);
extern void vdp1_cmdt_param_color_set(struct vdp1_cmdt *, color_rgb555_t);
extern void vdp1_cmdt_param_color_mode0_set(struct vdp1_cmdt *, const vdp1_cmdt_color_bank);
extern void vdp1_cmdt_param_color_mode1_set(struct vdp1_cmdt *, uint32_t);
extern void vdp1_cmdt_param_color_mode2_set(struct vdp1_cmdt *, const vdp1_cmdt_color_bank);
extern void vdp1_cmdt_param_color_mode3_set(struct vdp1_cmdt *, const vdp1_cmdt_color_bank);
extern void vdp1_cmdt_param_color_mode4_set(struct vdp1_cmdt *, const vdp1_cmdt_color_bank);
extern void vdp1_cmdt_param_size_set(struct vdp1_cmdt *, uint16_t, uint16_t);
extern void vdp1_cmdt_param_horizontal_flip_set(struct vdp1_cmdt *, bool);
extern void vdp1_cmdt_param_vertical_flip_set(struct vdp1_cmdt *, bool);
extern void vdp1_cmdt_param_vertex_set(struct vdp1_cmdt *, uint16_t, const int16_vector2_t *);
extern void vdp1_cmdt_param_gouraud_base_set(struct vdp1_cmdt *, uint32_t);

extern void vdp1_cmdt_jump_clear(struct vdp1_cmdt *);

extern void vdp1_cmdt_jump_assign(struct vdp1_cmdt *, uint16_t);
extern void vdp1_cmdt_jump_call(struct vdp1_cmdt *, uint16_t);
extern void vdp1_cmdt_jump_skip_assign(struct vdp1_cmdt *, uint16_t);
extern void vdp1_cmdt_jump_skip_call(struct vdp1_cmdt *, uint16_t);

extern void vdp1_cmdt_jump_next(struct vdp1_cmdt *);
extern void vdp1_cmdt_jump_return(struct vdp1_cmdt *);
extern void vdp1_cmdt_jump_skip_next(struct vdp1_cmdt *);
extern void vdp1_cmdt_jump_skip_return(struct vdp1_cmdt *);

__END_DECLS

#endif /* !_VDP1_CMDT_H_ */
