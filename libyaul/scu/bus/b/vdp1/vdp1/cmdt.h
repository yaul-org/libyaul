/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_CMDT_H_
#define _VDP1_CMDT_H_

#include <inttypes.h>

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE(n)                                  \
        union {                                                                \
                struct {                                                       \
                        unsigned int msb:1; /* Bit 15 */                       \
                        unsigned int :2;                                       \
                        unsigned int high_speed_shrink:1;                      \
                        unsigned int pre_clipping:1;                           \
                        unsigned int user_clipping:2; /* ??? */                \
                        unsigned int mesh:1;                                   \
                        unsigned int end_code:1; /* Enable */                  \
                        unsigned int transparent_pixel:1; /* Enable */         \
                        unsigned int color_mode:3;                             \
                        unsigned int cc_mode:3;                                \
                } __aligned(2);                                                \
                                                                               \
                uint16_t raw;                                                  \
        } CC_CONCAT(n, _mode)

#define VDP1_CMDT_DUMMY_UNION_DECLARE(n)                                       \
        union {                                                                \
                uint16_t value;                                                \
        } CC_CONCAT(n, _extra)

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

struct vdp1_cmdt_gst {
        uint16_t entry[4];
} __aligned(8);

struct vdp1_cmdt_sprite {
#define CMDT_TYPE_NORMAL_SPRITE         0x0000
#define CMDT_TYPE_SCALED_SPRITE         0x0001
#define CMDT_TYPE_DISTORTED_SPRITE      0x0002
        uint16_t cs_type;

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
        } cs_zoom_point;

        VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE(cs);

        union {
                /* Mode 0, 2, 3, and 4 */
                uint32_t cs_color_bank;
                /* Mode 1 */
                uint32_t cs_clut;
        };

        uint32_t cs_char;
        uint16_t cs_width;
        uint16_t cs_height;

        union {
                struct {
                        int16_t x;
                        int16_t y;
                } cs_position;

                struct {
                        struct {
                                int16_t x;
                                int16_t y;
                        } point;

                        struct {
                                int16_t x;
                                int16_t y;
                        } display;
                } cs_zoom;

                struct {
                        struct {
                                int16_t x;
                                int16_t y;
                        } a, b, c, d;
                } cs_vertex;
        };

        uint32_t cs_grad;

        VDP1_CMDT_DUMMY_UNION_DECLARE(cs);
};

struct vdp1_cmdt_polygon {
        VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE(cp);

        uint16_t cp_color;

        struct {
                struct {
                        int16_t x;
                        int16_t y;
                } a, b, c, d;
        } cp_vertex;

        uint32_t cp_grad;

        VDP1_CMDT_DUMMY_UNION_DECLARE(cp);
};

struct vdp1_cmdt_polyline {
        VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE(cl);

        uint16_t cl_color;

        struct {
                struct {
                        int16_t x;
                        int16_t y;
                } a, b, c, d;
        } cl_vertex;

        uint32_t cl_grad;

        VDP1_CMDT_DUMMY_UNION_DECLARE(cl);
};

struct vdp1_cmdt_line {
        VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE(cl);

        uint16_t cl_color;

        struct {
                struct {
                        int16_t x;
                        int16_t y;
                } a, b;
        } cl_vertex;

        uint32_t cl_grad;

        VDP1_CMDT_DUMMY_UNION_DECLARE(cl);
};

struct vdp1_cmdt_local_coord {
        struct {
                int16_t x;
                int16_t y;
        } lc_coord;

        VDP1_CMDT_DUMMY_UNION_DECLARE(lc);
};

struct vdp1_cmdt_system_clip_coord {
        struct {
                int16_t x;
                int16_t y;
        } scc_coord;

        VDP1_CMDT_DUMMY_UNION_DECLARE(scc);
};

struct vdp1_cmdt_user_clip_coord {
        struct {
                int16_t x;
                int16_t y;
        } ucc_coords[2];

        VDP1_CMDT_DUMMY_UNION_DECLARE(ucc);
};

extern void vdp1_cmdt_list_init(void);
extern void vdp1_cmdt_list_begin(uint32_t);
extern void vdp1_cmdt_list_end(uint32_t);
extern void vdp1_cmdt_list_clear(uint32_t);
extern void vdp1_cmdt_list_clear_all(void);
extern void vdp1_cmdt_list_commit(void);

extern void vdp1_cmdt_sprite_draw(struct vdp1_cmdt_sprite *);
extern void vdp1_cmdt_polygon_draw(struct vdp1_cmdt_polygon *);
extern void vdp1_cmdt_polyline_draw(struct vdp1_cmdt_polyline *);
extern void vdp1_cmdt_line_draw(struct vdp1_cmdt_line *);
extern void vdp1_cmdt_user_clip_coord_set(struct vdp1_cmdt_user_clip_coord *);
extern void vdp1_cmdt_system_clip_coord_set(
        struct vdp1_cmdt_system_clip_coord *);
extern void vdp1_cmdt_local_coord_set(struct vdp1_cmdt_local_coord *);
extern void vdp1_cmdt_end(void);

#undef VDP1_CMDT_DRAW_MODE_STRUCT_DECLARE
#undef VDP1_CMDT_DUMMY_UNION_DECLARE

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_VDP1_CMDT_H_ */
