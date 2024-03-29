/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP1_CMDT_H_
#define _YAUL_VDP1_CMDT_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>
#include <string.h>

#include <gamemath/color/rgb1555.h>
#include <gamemath/int16.h>

#include <scu.h>

#include <vdp1/map.h>

#include <vdp2/sprite.h>

__BEGIN_DECLS

typedef enum vdp1_cmdt_command {
    VDP1_CMDT_NORMAL_SPRITE     = 0,
    VDP1_CMDT_SCALED_SPRITE     = 1,
    VDP1_CMDT_DISTORTED_SPRITE  = 2,
    VDP1_CMDT_POLYGON           = 4,
    VDP1_CMDT_POLYLINE          = 5,
    VDP1_CMDT_LINE              = 6,
    VDP1_CMDT_USER_CLIP_COORD   = 8,
    VDP1_CMDT_SYSTEM_CLIP_COORD = 9,
    VDP1_CMDT_LOCAL_COORD       = 10
} vdp1_cmdt_command_t;

typedef enum vdp1_cmdt_link_type {
    VDP1_CMDT_LINK_TYPE_JUMP_NEXT   = 0,
    VDP1_CMDT_LINK_TYPE_JUMP_ASSIGN = 1 << 12,
    VDP1_CMDT_LINK_TYPE_JUMP_CALL   = 2 << 12,
    VDP1_CMDT_LINK_TYPE_JUMP_RETURN = 3 << 12,
    VDP1_CMDT_LINK_TYPE_SKIP_NEXT   = 4 << 12,
    VDP1_CMDT_LINK_TYPE_SKIP_ASSIGN = 5 << 12,
    VDP1_CMDT_LINK_TYPE_SKIP_CALL   = 6 << 12,
    VDP1_CMDT_LINK_TYPE_SKIP_RETURN = 7 << 12
} vdp1_cmdt_link_type_t;

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

typedef enum vdp1_cmdt_cc {
    VDP1_CMDT_CC_REPLACE          = 0,
    VDP1_CMDT_CC_SHADOW           = 1,
    VDP1_CMDT_CC_HALF_LUMINANCE   = 2,
    VDP1_CMDT_CC_HALF_TRANSPARENT = 3,
    VDP1_CMDT_CC_GOURAUD          = 4
} vdp1_cmdt_cc_t;

typedef enum vdp1_cmdt_cm {
    VDP1_CMDT_CM_CB_16     = 0,
    VDP1_CMDT_CM_CLUT_16   = 1,
    VDP1_CMDT_CM_CB_64     = 2,
    VDP1_CMDT_CM_CB_128    = 3,
    VDP1_CMDT_CM_CB_256    = 4,
    VDP1_CMDT_CM_RGB_32768 = 5
} vdp1_cmdt_cm_t;

typedef enum vdp1_cmdt_char_flip {
    VDP1_CMDT_CHAR_FLIP_NONE = 0,
    VDP1_CMDT_CHAR_FLIP_H    = 1 << 4,
    VDP1_CMDT_CHAR_FLIP_V    = 1 << 5,
    VDP1_CMDT_CHAR_FLIP_HV   = VDP1_CMDT_CHAR_FLIP_H | VDP1_CMDT_CHAR_FLIP_V
} vdp1_cmdt_char_flip_t;

typedef uint16_t vdp1_link_t;

typedef vdp2_sprite_type_0_t vdp1_color_bank_type_0_t;
typedef vdp2_sprite_type_1_t vdp1_color_bank_type_1_t;
typedef vdp2_sprite_type_2_t vdp1_color_bank_type_2_t;
typedef vdp2_sprite_type_3_t vdp1_color_bank_type_3_t;
typedef vdp2_sprite_type_4_t vdp1_color_bank_type_4_t;
typedef vdp2_sprite_type_5_t vdp1_color_bank_type_5_t;
typedef vdp2_sprite_type_6_t vdp1_color_bank_type_6_t;
typedef vdp2_sprite_type_7_t vdp1_color_bank_type_7_t;

typedef vdp2_sprite_type_8_t vdp1_color_bank_type_8_t;
typedef vdp2_sprite_type_9_t vdp1_color_bank_type_9_t;
typedef vdp2_sprite_type_a_t vdp1_color_bank_type_a_t;
typedef vdp2_sprite_type_b_t vdp1_color_bank_type_b_t;
typedef vdp2_sprite_type_c_t vdp1_color_bank_type_c_t;
typedef vdp2_sprite_type_d_t vdp1_color_bank_type_d_t;
typedef vdp2_sprite_type_e_t vdp1_color_bank_type_e_t;
typedef vdp2_sprite_type_f_t vdp1_color_bank_type_f_t;

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
        vdp1_cmdt_cm_t color_mode:3;         /* Bits 5-3 */
        vdp1_cmdt_cc_t cc_mode:3;            /* Bits 2-0 */
    } __packed;

    uint16_t raw;
} __packed vdp1_cmdt_draw_mode_t;

static_assert(sizeof(vdp1_cmdt_draw_mode_t) == 2);

typedef union vdp1_cmdt_color_bank {
    vdp1_color_bank_type_0_t type_0;
    vdp1_color_bank_type_1_t type_1;
    vdp1_color_bank_type_2_t type_2;
    vdp1_color_bank_type_3_t type_3;
    vdp1_color_bank_type_4_t type_4;
    vdp1_color_bank_type_5_t type_5;
    vdp1_color_bank_type_6_t type_6;
    vdp1_color_bank_type_7_t type_7;

    /* For sprite types 8-F, align so that it's a 16-bit value */
    struct {
        unsigned int :8;
        vdp1_color_bank_type_8_t type_8;
    };

    struct {
        unsigned int :8;
        vdp1_color_bank_type_9_t type_9;
    };

    struct {
        unsigned int :8;
        vdp1_color_bank_type_a_t type_a;
    };

    struct {
        unsigned int :8;
        vdp1_color_bank_type_b_t type_b;
    };

    struct {
        unsigned int :8;
        vdp1_color_bank_type_c_t type_c;
    };

    struct {
        unsigned int :8;
        vdp1_color_bank_type_d_t type_d;
    };

    struct {
        unsigned int :8;
        vdp1_color_bank_type_e_t type_e;
    };

    struct {
        unsigned int :8;
        vdp1_color_bank_type_f_t type_f;
    };

    uint16_t raw;
} __packed vdp1_cmdt_color_bank_t;

static_assert(sizeof(vdp1_cmdt_color_bank_t) == 2);

typedef struct vdp1_cmdt {
    uint16_t cmd_ctrl;
    uint16_t cmd_link;

    union {
        uint16_t cmd_pmod;

        vdp1_cmdt_draw_mode_t cmd_draw_mode;
    };

    uint16_t cmd_colr;
    uint16_t cmd_srca;
    uint16_t cmd_size;

    union {
        struct {
            int16_t cmd_xa;
            int16_t cmd_ya;
            int16_t cmd_xb;
            int16_t cmd_yb;
            int16_t cmd_xc;
            int16_t cmd_yc;
            int16_t cmd_xd;
            int16_t cmd_yd;
        } __packed;

        int16_vec2_t cmd_vertices[4];
    };

    uint16_t cmd_grda;

    uint16_t reserved;
} __aligned(32) vdp1_cmdt_t;

static_assert(sizeof(vdp1_cmdt_t) == 32);

typedef struct vdp1_cmdt_list {
    vdp1_cmdt_t *cmdts;
    uint16_t count;
} __aligned(4) vdp1_cmdt_list_t;

static_assert(sizeof(vdp1_cmdt_list_t) == 8);

typedef struct vdp1_cmdt_orderlist {
    unsigned int :32;
    unsigned int :32;
    vdp1_cmdt_t *cmdt;
} __packed __aligned(4) vdp1_cmdt_orderlist_t;

static_assert(sizeof(vdp1_cmdt_orderlist_t) == 12);

static inline uint16_t __always_inline
vdp1_cmdt_current_get(void)
{
    volatile vdp1_ioregs_t * const vdp1_ioregs = (volatile vdp1_ioregs_t *)VDP1_IOREG_BASE;

    return (vdp1_ioregs->copr >> 2);
}

static inline uint16_t __always_inline
vdp1_cmdt_last_get(void)
{
    volatile vdp1_ioregs_t * const vdp1_ioregs = (volatile vdp1_ioregs_t *)VDP1_IOREG_BASE;

    return (vdp1_ioregs->lopr >> 2);
}

static inline void __always_inline
vdp1_cmdt_orderlist_end(vdp1_cmdt_orderlist_t *cmdt_orderlist)
{
    scu_dma_xfer_t * const dma_xfer = (scu_dma_xfer_t *)cmdt_orderlist;

    dma_xfer->src |= SCU_DMA_INDIRECT_TABLE_END;
}

extern vdp1_cmdt_list_t *vdp1_cmdt_list_alloc(uint16_t count);
extern void vdp1_cmdt_list_free(vdp1_cmdt_list_t *cmdt_list);
extern void vdp1_cmdt_list_init(vdp1_cmdt_list_t *cmdt_list, vdp1_cmdt_t *cmdts);

extern void vdp1_cmdt_orderlist_init(vdp1_cmdt_orderlist_t *cmdt_orderlist,
  uint16_t count);
extern vdp1_cmdt_orderlist_t *vdp1_cmdt_orderlist_alloc(uint16_t count);
extern void vdp1_cmdt_orderlist_free(vdp1_cmdt_orderlist_t *cmdt_orderlist);
extern void vdp1_cmdt_orderlist_vram_patch(vdp1_cmdt_orderlist_t *cmdt_orderlist,
  const vdp1_cmdt_t *cmdt_base, uint16_t count);

static inline void __always_inline
vdp1_cmdt_draw_mode_set(vdp1_cmdt_t *cmdt,
  vdp1_cmdt_draw_mode_t draw_mode)
{
    /* Values 0x4, 0x5, 0x6 for comm indicate a non-textured command table,
     * and we want to set the bits 7 and 6 without branching */
    const uint16_t comm = (cmdt->cmd_ctrl & 0x0004);
    const uint16_t pmod_bits = (comm << 5) | (comm << 4);

    cmdt->cmd_pmod = pmod_bits | draw_mode.raw;
}

static inline void __always_inline
vdp1_cmdt_zoom_set(vdp1_cmdt_t *cmdt, vdp1_cmdt_zoom_point_t zoom_point)
{
    cmdt->cmd_ctrl &= 0xF0F0;
    cmdt->cmd_ctrl |= (zoom_point << 8) | 0x0001;
}

static inline void __always_inline
vdp1_cmdt_color_set(vdp1_cmdt_t *cmdt, rgb1555_t color)
{
    cmdt->cmd_colr = color.raw;
}

static inline void __always_inline
vdp1_cmdt_color_bank_set(vdp1_cmdt_t *cmdt,
  const vdp1_cmdt_color_bank_t color_bank)
{
    cmdt->cmd_colr = color_bank.raw;
}

static inline void __always_inline
vdp1_cmdt_color_mode0_set(vdp1_cmdt_t *cmdt,
  const vdp1_cmdt_color_bank_t color_bank)
{
    cmdt->cmd_pmod &= 0xFFC7;
    cmdt->cmd_colr = color_bank.raw & 0xFFF0;
}

static inline void __always_inline
vdp1_cmdt_color_mode1_set(vdp1_cmdt_t *cmdt, vdp1_vram_t base)
{
    cmdt->cmd_pmod &= 0xFFC7;
    cmdt->cmd_pmod |= 0x0008;
    cmdt->cmd_colr = (uint16_t)((base >> 3) & 0xFFFF);
}

static inline void __always_inline
vdp1_cmdt_color_mode2_set(vdp1_cmdt_t *cmdt,
  vdp1_cmdt_color_bank_t color_bank)
{
    cmdt->cmd_pmod &= 0xFFC7;
    cmdt->cmd_pmod |= 0x0010;
    cmdt->cmd_colr = color_bank.raw & 0xFFF0;
}

static inline void __always_inline
vdp1_cmdt_color_mode3_set(vdp1_cmdt_t *cmdt,
  vdp1_cmdt_color_bank_t color_bank)
{
    cmdt->cmd_pmod &= 0xFFC7;
    cmdt->cmd_pmod |= 0x0018;
    cmdt->cmd_colr = color_bank.raw & 0xFF80;
}

static inline void __always_inline
vdp1_cmdt_color_mode4_set(vdp1_cmdt_t *cmdt,
  vdp1_cmdt_color_bank_t color_bank)
{
    cmdt->cmd_pmod &= 0xFFC7;
    cmdt->cmd_pmod |= 0x0020;
    cmdt->cmd_colr = color_bank.raw & 0xFF00;
}

static inline void __always_inline
vdp1_cmdt_char_base_set(vdp1_cmdt_t *cmdt, vdp1_vram_t base)
{
    cmdt->cmd_srca = (base >> 3) & 0xFFFF;
}

static inline void __always_inline
vdp1_cmdt_char_size_set(vdp1_cmdt_t *cmdt, uint16_t width, uint16_t height)
{
    cmdt->cmd_size = (((width >> 3) << 8) | height) & 0x3FFF;
}

static inline void __always_inline
vdp1_cmdt_char_flip_set(vdp1_cmdt_t *cmdt, vdp1_cmdt_char_flip_t flip)
{
    cmdt->cmd_ctrl &= 0xFFCF;
    cmdt->cmd_ctrl |= flip;
}

static inline void __always_inline
vdp1_cmdt_vtx_zoom_point_set(vdp1_cmdt_t *cmdt, int16_vec2_t p)
{
    cmdt->cmd_vertices[0] = p;
}

static inline void __always_inline
vdp1_cmdt_vtx_zoom_display_set(vdp1_cmdt_t *cmdt, int16_vec2_t p)
{
    cmdt->cmd_vertices[1] = p;
}

static inline void __always_inline
vdp1_cmdt_vtx_scale_set(vdp1_cmdt_t *cmdt, int16_vec2_t ulp, int16_vec2_t lrp)
{
    cmdt->cmd_vertices[0] = ulp;
    cmdt->cmd_vertices[2] = lrp;
}

static inline void __always_inline
vdp1_cmdt_vtx_local_coord_set(vdp1_cmdt_t *cmdt, int16_vec2_t p)
{
    cmdt->cmd_vertices[0] = p;
}

static inline void __always_inline
vdp1_cmdt_vtx_system_clip_coord_set(vdp1_cmdt_t *cmdt, int16_vec2_t p)
{
    cmdt->cmd_vertices[2] = p;
}

static inline void __always_inline
vdp1_cmdt_vtx_user_clip_coord_set(vdp1_cmdt_t *cmdt, int16_vec2_t ulp, int16_vec2_t lrp)
{
    cmdt->cmd_vertices[0] = ulp;
    cmdt->cmd_vertices[2] = lrp;
}

static inline void __always_inline
vdp1_cmdt_vtx_set(vdp1_cmdt_t * __restrict cmdt, const int16_vec2_t * __restrict vertices)
{
    cmdt->cmd_vertices[0] = vertices[0];
    cmdt->cmd_vertices[1] = vertices[1];
    cmdt->cmd_vertices[2] = vertices[2];
    cmdt->cmd_vertices[3] = vertices[3];
}

static inline void __always_inline
vdp1_cmdt_gouraud_base_set(vdp1_cmdt_t *cmdt, vdp1_vram_t base)
{
    /* Gouraud shading processing is valid when a color calculation mode is
     * specified */
    cmdt->cmd_grda = (base >> 3) & 0xFFFF;
}

static inline vdp1_cmdt_command_t __always_inline
vdp1_cmdt_command_get(const vdp1_cmdt_t *cmdt)
{
    return (vdp1_cmdt_command_t)(cmdt->cmd_ctrl & ~0x7FF0);
}

static inline void __always_inline
vdp1_cmdt_command_set(vdp1_cmdt_t *cmdt, vdp1_cmdt_command_t command)
{
    cmdt->cmd_ctrl &= 0x7FF0;
    cmdt->cmd_ctrl |= command;
}

static inline void __always_inline
vdp1_cmdt_normal_sprite_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_NORMAL_SPRITE);
}

static inline void __always_inline
vdp1_cmdt_scaled_sprite_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_SCALED_SPRITE);
}

static inline void __always_inline
vdp1_cmdt_distorted_sprite_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_DISTORTED_SPRITE);
}

static inline void __always_inline
vdp1_cmdt_polygon_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_POLYGON);
}

static inline void __always_inline
vdp1_cmdt_polyline_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_POLYLINE);
}

static inline void __always_inline
vdp1_cmdt_line_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_LINE);
}

static inline void __always_inline
vdp1_cmdt_user_clip_coord_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_USER_CLIP_COORD);
}

static inline void __always_inline
vdp1_cmdt_system_clip_coord_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_SYSTEM_CLIP_COORD);
}

static inline void __always_inline
vdp1_cmdt_local_coord_set(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_command_set(cmdt, VDP1_CMDT_LOCAL_COORD);
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
vdp1_cmdt_link_type_set(vdp1_cmdt_t *cmdt, vdp1_cmdt_link_type_t link_type)
{
    cmdt->cmd_ctrl &= 0x8FFF;
    cmdt->cmd_ctrl |= link_type;
}

static inline void __always_inline
vdp1_cmdt_link_set(vdp1_cmdt_t *cmdt, vdp1_link_t link)
{
    cmdt->cmd_link = link << 2;
}

static inline void __always_inline
vdp1_cmdt_jump_next(vdp1_cmdt_t *cmdt)
{
    cmdt->cmd_ctrl &= 0x8FFF;
}

static inline void __always_inline
vdp1_cmdt_jump_assign(vdp1_cmdt_t *cmdt, vdp1_link_t link)
{
    vdp1_cmdt_link_type_set(cmdt, VDP1_CMDT_LINK_TYPE_JUMP_ASSIGN);
    vdp1_cmdt_link_set(cmdt, link);
}

static inline void __always_inline
vdp1_cmdt_jump_call(vdp1_cmdt_t *cmdt, vdp1_link_t link)
{
    vdp1_cmdt_link_type_set(cmdt, VDP1_CMDT_LINK_TYPE_JUMP_CALL);
    vdp1_cmdt_link_set(cmdt, link);
}

static inline void __always_inline
vdp1_cmdt_jump_return(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_link_type_set(cmdt, VDP1_CMDT_LINK_TYPE_JUMP_RETURN);
}

static inline void __always_inline
vdp1_cmdt_jump_skip_next(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_link_type_set(cmdt, VDP1_CMDT_LINK_TYPE_SKIP_NEXT);
}

static inline void __always_inline
vdp1_cmdt_jump_skip_assign(vdp1_cmdt_t *cmdt, vdp1_link_t link)
{

    vdp1_cmdt_link_type_set(cmdt, VDP1_CMDT_LINK_TYPE_SKIP_ASSIGN);
    vdp1_cmdt_link_set(cmdt, link);
}

static inline void __always_inline
vdp1_cmdt_jump_skip_call(vdp1_cmdt_t *cmdt, vdp1_link_t link)
{
    vdp1_cmdt_link_type_set(cmdt, VDP1_CMDT_LINK_TYPE_SKIP_CALL);
    vdp1_cmdt_link_set(cmdt, link);
}

static inline void __always_inline
vdp1_cmdt_jump_skip_return(vdp1_cmdt_t *cmdt)
{
    vdp1_cmdt_link_type_set(cmdt, VDP1_CMDT_LINK_TYPE_SKIP_RETURN);
}

__END_DECLS

#endif /* !_YAUL_VDP1_CMDT_H_ */
