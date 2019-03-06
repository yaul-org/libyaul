/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE                                     \
        union {                                                                \
                struct {                                                       \
                        unsigned int msb_enable:1;                /* Bit 15 */ \
                        unsigned int :2;                                       \
                        unsigned int hss_enable:1;                /* Bit 12 */ \
                        unsigned int pre_clipping_disable:1;      /* Bit 11 */ \
                        unsigned int user_clipping_mode:2;     /* Bits 10-9 */ \
                        unsigned int mesh_enable:1;                /* Bit 8 */ \
                        unsigned int end_code_disable:1;           /* Bit 7 */ \
                        unsigned int trans_pixel_disable:1;        /* Bit 6 */ \
                        unsigned int color_mode:3;              /* Bits 5-3 */ \
                        unsigned int cc_mode:3;                 /* Bits 2-0 */ \
                } __aligned(2);                                                \
                                                                               \
                uint16_t raw;                                                  \
        } draw_mode

#define VDP1_CMDT_SPRITE_TYPE_DECLARE                                          \
        union {                                                                \
                struct vdp2_sprite_type_0 type_0;                              \
                struct vdp2_sprite_type_1 type_1;                              \
                struct vdp2_sprite_type_2 type_2;                              \
                struct vdp2_sprite_type_3 type_3;                              \
                struct vdp2_sprite_type_4 type_4;                              \
                struct vdp2_sprite_type_5 type_5;                              \
                struct vdp2_sprite_type_6 type_6;                              \
                struct vdp2_sprite_type_7 type_7;                              \
                struct vdp2_sprite_type_8 type_8;                              \
                struct vdp2_sprite_type_9 type_9;                              \
                struct vdp2_sprite_type_a type_a;                              \
                struct vdp2_sprite_type_b type_b;                              \
                struct vdp2_sprite_type_c type_c;                              \
                struct vdp2_sprite_type_d type_d;                              \
                struct vdp2_sprite_type_e type_e;                              \
                struct vdp2_sprite_type_f type_f;                              \
                                                                               \
                uint16_t raw;                                                  \
        } sprite_type

#define VDP1_CMDT_SPRITE_DECLARE                                               \
        VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE;                                    \
                                                                               \
        union {                                                                \
                /* Mode 0, 2, 3, and 4 */                                      \
                VDP1_CMDT_SPRITE_TYPE_DECLARE;                                 \
                                                                               \
                /* Mode 1 */                                                   \
                uint32_t clut;                                                 \
        };                                                                     \
                                                                               \
        uint32_t char_base;                                                    \
        uint16_t width;                                                        \
        uint16_t height;                                                       \
                                                                               \
        uint32_t grad_base

#define VDP1_CMDT_NON_TEXTURED_DECLARE                                         \
        VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE;                                    \
                                                                               \
        union {                                                                \
                VDP1_CMDT_SPRITE_TYPE_DECLARE;                                 \
                                                                               \
                color_rgb555_t color;                                          \
        };                                                                     \
                                                                               \
        uint32_t grad_base

struct vdp1_cmdt;

struct vdp1_cmdt_list {
        struct vdp1_cmdt *cmdts;
        struct vdp1_cmdt *cmdt;
        uint16_t count;
} __aligned(4);

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

struct vdp1_cmdt_normal_sprite {
        VDP1_CMDT_SPRITE_DECLARE;

        union {
                int16_vector2_t position;

                struct {
                        int16_vector2_t a;
                } vertex;

                int16_t components[2];
                int16_vector2_t vertices[1];
        };
};

struct vdp1_cmdt_scaled_sprite {
        VDP1_CMDT_SPRITE_DECLARE;

        union {
#define CMDT_ZOOM_POINT_UPPER_LEFT      0x0030
#define CMDT_ZOOM_POINT_UPPER_CENTER    0x0050
#define CMDT_ZOOM_POINT_UPPER_RIGHT     0x0090
#define CMDT_ZOOM_POINT_CENTER_LEFT     0x0210
#define CMDT_ZOOM_POINT_CENTER          0x0410
#define CMDT_ZOOM_POINT_CENTER_RIGHT    0x0810
#define CMDT_ZOOM_POINT_LOWER_LEFT      0x2010
#define CMDT_ZOOM_POINT_LOWER_CENTER    0x4010
#define CMDT_ZOOM_POINT_LOWER_RIGHT     0x8010

                struct {
                        unsigned int lower_right:1;
                        unsigned int lower_center:1;
                        unsigned int lower_left:1;
                        unsigned int :1;
                        unsigned int center_right:1;
                        unsigned int center:1;
                        unsigned int center_left:1;
                        unsigned int :1;
                        unsigned int upper_right:1;
                        unsigned int upper_center:1;
                        unsigned int upper_left:1;
                        unsigned int enable:1;
                } __aligned(2);

                uint16_t raw;
        } zoom_point;

        union {
                struct {
                        /* Vertex A */
                        int16_vector2_t ul;

                        /* Vertex B */
                        unsigned int :16;
                        unsigned int :16;

                        /* Vertex C */
                        int16_vector2_t lr;
                } scale;

                struct {
                        /* Vertex A */
                        int16_vector2_t point;
                        /* Vertex B */
                        int16_vector2_t display;

                        /* Vertex C */
                        unsigned int :16;
                        unsigned int :16;
                } zoom;

                struct {
                        int16_vector2_t a;
                        int16_vector2_t b;
                        int16_vector2_t c;
                } vertex;

                int16_t components[6];
                int16_vector2_t vertices[3];
        };
};

struct vdp1_cmdt_distorted_sprite {
        VDP1_CMDT_SPRITE_DECLARE;

        union {
                struct {
                        int16_vector2_t a;
                        int16_vector2_t b;
                        int16_vector2_t c;
                        int16_vector2_t d;
                } vertex;

                int16_t components[8];
                int16_vector2_t vertices[4];
        };
};

struct vdp1_cmdt_polygon {
        VDP1_CMDT_NON_TEXTURED_DECLARE;

        union {
                struct {
                        int16_vector2_t a;
                        int16_vector2_t b;
                        int16_vector2_t c;
                        int16_vector2_t d;
                } vertex;

                int16_t components[8];
                int16_vector2_t vertices[4];
        };
};

struct vdp1_cmdt_polyline {
        VDP1_CMDT_NON_TEXTURED_DECLARE;

        union {
                struct {
                        int16_vector2_t a;
                        int16_vector2_t b;
                        int16_vector2_t c;
                        int16_vector2_t d;
                } vertex;

                int16_t components[4];
                int16_vector2_t vertices[4];
        };
};

struct vdp1_cmdt_line {
        VDP1_CMDT_NON_TEXTURED_DECLARE;

        union {
                struct {
                        int16_vector2_t a;
                        int16_vector2_t b;
                } vertex;

                int16_t components[4];
                int16_vector2_t vertices[8];
        };
};

struct vdp1_cmdt_local_coord {
        int16_vector2_t coord;
};

struct vdp1_cmdt_system_clip_coord {
        int16_vector2_t coord;
};

struct vdp1_cmdt_user_clip_coord {
        int16_vector2_t coords[2];
};

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

extern struct vdp1_cmdt_list *vdp1_cmdt_list_alloc(uint16_t);
extern void vdp1_cmdt_list_free(struct vdp1_cmdt_list *);
extern void vdp1_cmdt_list_init(struct vdp1_cmdt_list *, struct vdp1_cmdt *, uint16_t);
extern void vdp1_cmdt_list_reset(struct vdp1_cmdt_list *);

extern struct vdp1_cmdt *vdp1_cmdt_base_get(void);

extern uint16_t vdp1_cmdt_normal_sprite_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_normal_sprite *);
extern uint16_t vdp1_cmdt_scaled_sprite_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_scaled_sprite *);
extern uint16_t vdp1_cmdt_distorted_sprite_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_distorted_sprite *);
extern uint16_t vdp1_cmdt_polygon_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_polygon *);
extern uint16_t vdp1_cmdt_polyline_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_polyline *);
extern uint16_t vdp1_cmdt_line_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_line *);
extern uint16_t vdp1_cmdt_user_clip_coord_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_user_clip_coord *);
extern uint16_t vdp1_cmdt_system_clip_coord_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_system_clip_coord *);
extern uint16_t vdp1_cmdt_local_coord_add(struct vdp1_cmdt_list *, const struct vdp1_cmdt_local_coord *);
extern void vdp1_cmdt_end(struct vdp1_cmdt_list *);

extern void vdp1_cmdt_jump_assign(struct vdp1_cmdt_list *, uint8_t, uint8_t);
extern void vdp1_cmdt_jump_call(struct vdp1_cmdt_list *, uint8_t, uint8_t);
extern void vdp1_cmdt_jump_skip_assign(struct vdp1_cmdt_list *, uint8_t, uint8_t);
extern void vdp1_cmdt_jump_skip_call(struct vdp1_cmdt_list *, uint8_t, uint8_t);

extern void vdp1_cmdt_jump_next(struct vdp1_cmdt_list *, uint8_t);
extern void vdp1_cmdt_jump_return(struct vdp1_cmdt_list *, uint8_t);
extern void vdp1_cmdt_jump_skip_next(struct vdp1_cmdt_list *, uint8_t);
extern void vdp1_cmdt_jump_skip_return(struct vdp1_cmdt_list *, uint8_t);

#undef VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE
#undef VDP1_CMDT_SPRITE_TYPE_DECLARE
#undef VDP1_CMDT_SPRITE_DECLARE
#undef VDP1_CMDT_NON_TEXTURED_DECLARE

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_VDP1_CMDT_H_ */
