/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CMDT_H_
#define _CMDT_H_

#include <inttypes.h>

#include <common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VDP1_CMDT_DRAW_MODE_DECLARE_STRUCT(n)                                  \
        union {                                                                \
                struct {                                                       \
                        unsigned int msb:1;                                    \
                        unsigned int reserved:2;                               \
                        unsigned int high_speed_shrink:1;                      \
                        unsigned int pre_clipping:1;                           \
                        unsigned int user_clipping:2; /* ??? */                \
                        unsigned int mesh:1;                                   \
                        unsigned int end_code:1; /* Enable */                  \
                        unsigned int transparent_pixel:1; /* Enable */         \
                        unsigned int color_mode:3;                             \
                        unsigned int cc_mode:3;                                \
                } __attribute__ ((packed, aligned(2)));                        \
                                                                               \
                uint16_t value;                                                \
        } CC_CONCAT(n, _mode)

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
} __attribute__ ((packed, aligned(32)));

struct vdp1_cmdt_gst {
        uint16_t entry[4];
} __attribute__ ((packed, aligned(8)));

struct vdp1_cmdt_normal_sprite {
        VDP1_CMDT_DRAW_MODE_DECLARE_STRUCT(cns);

        union {
                /* Mode 0, 2, 3, and 4 */
                uint32_t cns_color_bank;
                /* Mode 1 */
                uint32_t cns_clut_addr;
        };

        uint32_t cns_char_addr;
        uint16_t cns_width;
        uint16_t cns_height;

        struct {
                int16_t x;
                int16_t y;
        } cns_position;

        uint32_t cns_grad_addr;
} __attribute__ ((packed, aligned(32)));

struct vdp1_cmdt_scaled_sprite {
        VDP1_CMDT_DRAW_MODE_DECLARE_STRUCT(css);

        uint32_t css_clut_addr;
        uint32_t css_char_addr;
        uint16_t css_width;
        uint16_t css_height;

        struct {
                int16_t x;
                int16_t y;
        } css_position;

        uint32_t css_grad_addr;
} __attribute__ ((packed, aligned(32)));

struct vdp1_cmdt_polygon {
        VDP1_CMDT_DRAW_MODE_DECLARE_STRUCT(cp);

        uint16_t cp_color;

        struct {
                struct {
                        int16_t x;
                        int16_t y;
                } a, b, c, d;
        } cp_vertices;

        uint32_t cp_grad_addr;
} __attribute__ ((packed, aligned(32)));

struct vdp1_cmdt_polyline {
        VDP1_CMDT_DRAW_MODE_DECLARE_STRUCT(cp);

        uint16_t cp_color;

        struct {
                struct {
                        int16_t x;
                        int16_t y;
                } a, b, c, d;
        } cp_vertices;

        uint32_t cp_grad_addr;
} __attribute__ ((packed, aligned(32)));

extern void vdp1_cmdt_list_init(void);
extern void vdp1_cmdt_list_begin(uint32_t);
extern void vdp1_cmdt_list_end(uint32_t);
extern void vdp1_cmdt_list_clear(uint32_t);
extern void vdp1_cmdt_list_clear_all(void);

extern void vdp1_cmdt_normal_sprite_draw(struct vdp1_cmdt_normal_sprite *);
extern void vdp1_cmdt_polygon_draw(struct vdp1_cmdt_polygon *);
extern void vdp1_cmdt_polyline_draw(struct vdp1_cmdt_polyline *);
extern void vdp1_cmdt_user_clip_coord_set(uint8_t, uint8_t, uint8_t, uint8_t);
extern void vdp1_cmdt_sys_clip_coord_set(int16_t, int16_t);
extern void vdp1_cmdt_local_coord_set(int16_t, int16_t);

#undef VDP1_CMDT_DRAW_MODE_DECLARE_STRUCT

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CMDT_H_ */
