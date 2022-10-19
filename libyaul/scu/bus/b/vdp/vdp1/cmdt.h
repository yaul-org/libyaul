/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP1_CMDT_H_
#define _YAUL_VDP1_CMDT_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <string.h>

#include <int16.h>

#include <scu.h>

#include <vdp1/map.h>

#include <vdp2/sprite.h>

#include <color.h>

__BEGIN_DECLS

#define VDP1_CMDT_PMOD_MSB_ENABLE               (1 << 15)
#define VDP1_CMDT_PMOD_HSS_ENABLE               (1 << 12)
#define VDP1_CMDT_PMOD_PRE_CLIPPING_DISABLE     (1 << 11)
#define VDP1_CMDT_PMOD_MESH_ENABLE              (1 << 8)
#define VDP1_CMDT_PMOD_END_CODE_DISABLE         (1 << 7)
#define VDP1_CMDT_PMOD_TRANS_PIXEL_DISABLE      (1 << 6)

typedef uint16_t vdp1_link_t;

typedef enum vdp1_cmdt_zoom_point {
        VDP1_CMDT_ZOOM_POINT_NONE         = 0x00,
        VDP1_CMDT_ZOOM_POINT_UPPER_LEFT   = 0x05,
        VDP1_CMDT_ZOOM_POINT_UPPER_CENTER = 0x06,
        VDP1_CMDT_ZOOM_POINT_UPPER_RIGHT  = 0x07,
        VDP1_CMDT_ZOOM_POINT_CENTER_LEFT  = 0x09,
        VDP1_CMDT_ZOOM_POINT_CENTER       = 0x0A,
        VDP1_CMDT_ZOOM_POINT_CENTER_RIGHT = 0x0B,
        VDP1_CMDT_ZOOM_POINT_LOWER_LEFT   = 0x0D,
        VDP1_CMDT_ZOOM_POINT_LOWER_CENTER = 0x0E,
        VDP1_CMDT_ZOOM_POINT_LOWER_RIGHT  = 0x0F
} vdp1_cmdt_zoom_point_t;

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

#define CMDT_PMOD_CC_REPLACE            (0)
#define CMDT_PMOD_CC_SHADOW             (1)
#define CMDT_PMOD_CC_HALF_LUMINANCE     (2)
#define CMDT_PMOD_CC_HALF_TRANSPARENT   (3)
#define CMDT_PMOD_CC_GOURAUD            (4)

#define CMDT_PMOD_CM_COLOR_BANK_16_COLORS  (0)
#define CMDT_PMOD_CM_COLOR_LUT_16_COLORS   (1)
#define CMDT_PMOD_CM_COLOR_BANK_64_COLORS  (2)
#define CMDT_PMOD_CM_COLOR_BANK_128_COLORS (3)
#define CMDT_PMOD_CM_COLOR_BANK_256_COLORS (4)
#define CMDT_PMOD_CM_RGB_32768_COLORS      (5)

typedef struct vdp1_color_bank_type_0 {
        vdp2_sprite_type_0_t data;
} __packed vdp1_color_bank_type_0_t;

typedef struct vdp1_color_bank_type_1 {
        vdp2_sprite_type_1_t data;
} __packed vdp1_color_bank_type_1_t;

typedef struct vdp1_color_bank_type_2 {
        vdp2_sprite_type_2_t data;
} __packed vdp1_color_bank_type_2_t;

typedef struct vdp1_color_bank_type_3 {
        vdp2_sprite_type_3_t data;
} __packed vdp1_color_bank_type_3_t;

typedef struct vdp1_color_bank_type_4 {
        vdp2_sprite_type_4_t data;
} __packed vdp1_color_bank_type_4_t;

typedef struct vdp1_color_bank_type_5 {
        vdp2_sprite_type_5_t data;
} __packed vdp1_color_bank_type_5_t;

typedef struct vdp1_color_bank_type_6 {
        vdp2_sprite_type_6_t data;
} __packed vdp1_color_bank_type_6_t;

typedef struct vdp1_color_bank_type_7 {
        vdp2_sprite_type_7_t data;
} __packed vdp1_color_bank_type_7_t;

typedef struct vdp1_color_bank_type_8 {
        unsigned int :8;
        vdp2_sprite_type_8_t data;
} __packed vdp1_color_bank_type_8_t;

typedef struct vdp1_color_bank_type_9 {
        unsigned int :8;
        vdp2_sprite_type_9_t data;
} __packed vdp1_color_bank_type_9_t;

typedef struct vdp1_color_bank_type_a {
        unsigned int :8;
        vdp2_sprite_type_a_t data;
} __packed vdp1_color_bank_type_a_t;

typedef struct vdp1_color_bank_type_b {
        unsigned int :8;
        vdp2_sprite_type_b_t data;
} __packed vdp1_color_bank_type_b_t;

typedef struct vdp1_color_bank_type_c {
        unsigned int :8;
        vdp2_sprite_type_c_t data;
} __packed vdp1_color_bank_type_c_t;

typedef struct vdp1_color_bank_type_d {
        unsigned int :8;
        vdp2_sprite_type_d_t data;
} __packed vdp1_color_bank_type_d_t;

typedef struct vdp1_color_bank_type_e {
        unsigned int :8;
        vdp2_sprite_type_e_t data;
} __packed vdp1_color_bank_type_e_t;

typedef struct vdp1_color_bank_type_f {
        unsigned int :8;
        vdp2_sprite_type_f_t data;
} __packed vdp1_color_bank_type_f_t;

typedef struct vdp1_cmdt {
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
} __aligned(32) vdp1_cmdt_t;

typedef union vdp1_cmdt_draw_mode {
        struct {
                unsigned int msb_enable:1;           /* Bit 15 */
                unsigned int :2;
                unsigned int hss_enable:1;           /* Bit 12 */
                unsigned int pre_clipping_disable:1; /* Bit 11 */
                unsigned int user_clipping_mode:2;   /* Bits 10-9 */
                unsigned int mesh_enable:1;          /* Bit 8 */
                unsigned int end_code_disable:1;     /* Bit 7 */
                unsigned int trans_pixel_disable:1;  /* Bit 6 */
                unsigned int color_mode:3;           /* Bits 5-3 */
                unsigned int cc_mode:3;              /* Bits 2-0 */
        } bits __aligned(2);

        uint16_t raw;
} vdp1_cmdt_draw_mode_t;

typedef union vdp1_cmdt_color_bank {
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

        uint16_t raw;
} vdp1_cmdt_color_bank_t;

typedef struct vdp1_cmdt_list {
        vdp1_cmdt_t *cmdts;
        uint16_t count;
} __aligned(4) vdp1_cmdt_list_t;

typedef struct {
        unsigned int :32;
        unsigned int :32;
        vdp1_cmdt_t *cmdt;
} __packed __aligned(4) vdp1_cmdt_orderlist_t;

static inline uint16_t __always_inline
vdp1_cmdt_current_get(void)
{
        return (MEMORY_READ(16, VDP1(COPR)) >> 2);
}

static inline uint16_t __always_inline
vdp1_cmdt_last_get(void)
{
        return (MEMORY_READ(16, VDP1(LOPR)) >> 2);
}

static inline void __always_inline
vdp1_cmdt_orderlist_end(vdp1_cmdt_orderlist_t *cmdt_orderlist)
{
        scu_dma_xfer_t *dma_xfer;
        dma_xfer = (scu_dma_xfer_t *)cmdt_orderlist;

        dma_xfer->src |= SCU_DMA_INDIRECT_TABLE_END;
}

extern vdp1_cmdt_list_t *vdp1_cmdt_list_alloc(uint16_t count);
extern void vdp1_cmdt_list_free(vdp1_cmdt_list_t *cmdt_list);
extern void vdp1_cmdt_list_init(vdp1_cmdt_list_t *cmdt_list, vdp1_cmdt_t *cmdts);

extern vdp1_cmdt_orderlist_t *vdp1_cmdt_orderlist_alloc(uint16_t count);
extern void vdp1_cmdt_orderlist_free(vdp1_cmdt_orderlist_t *cmdt_orderlist);
extern void vdp1_cmdt_orderlist_init(vdp1_cmdt_orderlist_t *cmdt_orderlist,
    uint16_t count);
extern void vdp1_cmdt_orderlist_vram_patch(vdp1_cmdt_orderlist_t *cmdt_orderlist,
    const vdp1_cmdt_t *cmdt_base, uint16_t count);

static inline void __always_inline
vdp1_cmdt_param_draw_mode_set(vdp1_cmdt_t *cmdt,
    vdp1_cmdt_draw_mode_t draw_mode)
{
        /* Values 0x4, 0x5, 0x6 for comm indicate a non-textured command table,
         * and we want to set the bits 7 and 6 without branching */
        const uint16_t comm = (cmdt->cmd_ctrl & 0x0004);
        const uint16_t pmod_bits = (comm << 5) | (comm << 4);

        cmdt->cmd_pmod = pmod_bits | draw_mode.raw;
}

static inline void __always_inline
vdp1_cmdt_param_zoom_set(vdp1_cmdt_t *cmdt, vdp1_cmdt_zoom_point_t zoom_point)
{
        cmdt->cmd_ctrl &= 0xF0F0;
        cmdt->cmd_ctrl |= (zoom_point << 8) | 0x0001;
}

static inline void __always_inline
vdp1_cmdt_param_char_base_set(vdp1_cmdt_t *cmdt, vdp1_vram_t base)
{
        cmdt->cmd_srca = (base >> 3) & 0xFFFF;
}

static inline void __always_inline
vdp1_cmdt_param_color_set(vdp1_cmdt_t *cmdt, rgb1555_t color)
{
        cmdt->cmd_colr = color.raw;
}

static inline void __always_inline
vdp1_cmdt_param_color_bank_set(vdp1_cmdt_t *cmdt,
    const vdp1_cmdt_color_bank_t color_bank)
{
        cmdt->cmd_colr = color_bank.raw;
}

static inline void __always_inline
vdp1_cmdt_param_color_mode0_set(vdp1_cmdt_t *cmdt,
    const vdp1_cmdt_color_bank_t color_bank)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_colr = color_bank.raw & 0xFFF0;
}

static inline void __always_inline
vdp1_cmdt_param_color_mode1_set(vdp1_cmdt_t *cmdt, vdp1_vram_t base)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_pmod |= 0x0008;
        cmdt->cmd_colr = (uint16_t)((base >> 3) & 0xFFFF);
}

static inline void __always_inline
vdp1_cmdt_param_color_mode2_set(vdp1_cmdt_t *cmdt,
    vdp1_cmdt_color_bank_t color_bank)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_pmod |= 0x0010;
        cmdt->cmd_colr = color_bank.raw & 0xFFF0;
}

static inline void __always_inline
vdp1_cmdt_param_color_mode3_set(vdp1_cmdt_t *cmdt,
    vdp1_cmdt_color_bank_t color_bank)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_pmod |= 0x0018;
        cmdt->cmd_colr = color_bank.raw & 0xFF80;
}

static inline void __always_inline
vdp1_cmdt_param_color_mode4_set(vdp1_cmdt_t *cmdt,
    vdp1_cmdt_color_bank_t color_bank)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_pmod |= 0x0020;
        cmdt->cmd_colr = color_bank.raw & 0xFF00;
}

static inline void __always_inline
vdp1_cmdt_param_size_set(vdp1_cmdt_t *cmdt, uint16_t width, uint16_t height)
{
        cmdt->cmd_size = (((width >> 3) << 8) | height) & 0x3FFF;
}

static inline void __always_inline
vdp1_cmdt_param_horizontal_flip_set(vdp1_cmdt_t *cmdt, bool flip)
{
        cmdt->cmd_ctrl &= 0xFFEF;
        cmdt->cmd_ctrl |= ((uint16_t)flip & 0x0001) << 4;
}

static inline void __always_inline
vdp1_cmdt_param_vertical_flip_set(vdp1_cmdt_t *cmdt, bool flip)
{
        cmdt->cmd_ctrl &= 0xFFDF;
        cmdt->cmd_ctrl |= ((uint16_t)flip & 0x0001) << 5;
}

static inline void __always_inline
vdp1_cmdt_param_vertex_set(vdp1_cmdt_t *cmdt,
    uint16_t vertex_index,
    const int16_vec2_t *p)
{
        int16_vec2_t * const vertex =
            (int16_vec2_t *)(&cmdt->cmd_xa + ((vertex_index & 0x3) << 1));

        vertex->x = p->x;
        vertex->y = p->y;
}

static inline void __always_inline
vdp1_cmdt_param_vertices_set(vdp1_cmdt_t *cmdt, const int16_vec2_t *p)
{
        (void)memcpy(&cmdt->cmd_xa, p, sizeof(int16_vec2_t) * 4);
}

static inline void __always_inline
vdp1_cmdt_param_gouraud_base_set(vdp1_cmdt_t *cmdt, vdp1_vram_t base)
{
        /* Gouraud shading processing is valid when a color calculation mode is
         * specified */
        cmdt->cmd_grda = (base >> 3) & 0xFFFF;
}

static inline void __always_inline
vdp1_cmdt_normal_sprite_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x0000;
}

static inline void __always_inline
vdp1_cmdt_scaled_sprite_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x0001;
}

static inline void __always_inline
vdp1_cmdt_distorted_sprite_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x0002;
}

static inline void __always_inline
vdp1_cmdt_polygon_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x0004;
}

static inline void __always_inline
vdp1_cmdt_polyline_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x0005;
}

static inline void __always_inline
vdp1_cmdt_line_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x0006;
}

static inline void __always_inline
vdp1_cmdt_user_clip_coord_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x0008;
}

static inline void __always_inline
vdp1_cmdt_system_clip_coord_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x0009;
}

static inline void __always_inline
vdp1_cmdt_local_coord_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x7FF0;
        cmdt->cmd_ctrl |= 0x000A;
}

static inline void __always_inline
vdp1_cmdt_end_clear(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= ~0x8000;
}

static inline void __always_inline
vdp1_cmdt_end_set(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl |= 0x8000;
}

static inline void __always_inline
vdp1_cmdt_jump_clear(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
}

static inline void __always_inline
vdp1_cmdt_jump_assign(vdp1_cmdt_t *cmdt, vdp1_link_t index)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x1000;
        cmdt->cmd_link = index << 2;
}

static inline void __always_inline
vdp1_cmdt_jump_call(vdp1_cmdt_t *cmdt, vdp1_link_t index)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x2000;
        cmdt->cmd_link = index << 2;
}

static inline void __always_inline
vdp1_cmdt_jump_skip_assign(vdp1_cmdt_t *cmdt, vdp1_link_t index)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x5000;
        cmdt->cmd_link = index << 2;
}

static inline void __always_inline
vdp1_cmdt_jump_skip_call(vdp1_cmdt_t *cmdt, vdp1_link_t index)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x6000;
        cmdt->cmd_link = index << 2;
}

static inline void __always_inline
vdp1_cmdt_jump_next(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
}

static inline void __always_inline
vdp1_cmdt_jump_return(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x3000;
}

static inline void __always_inline
vdp1_cmdt_jump_skip_next(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x4000;
}

static inline void __always_inline
vdp1_cmdt_jump_skip_return(vdp1_cmdt_t *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x7000;
}

__END_DECLS

#endif /* !_YAUL_VDP1_CMDT_H_ */
