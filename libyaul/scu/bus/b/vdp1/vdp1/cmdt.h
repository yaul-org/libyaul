/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CMDT_H_
#define _CMDT_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

struct vdp1_cmdt_grd {
        uint16_t entry[4];
} __attribute__ ((packed, aligned(8)));

struct vdp1_cmdt_polygon {
        uint16_t cmd_color;
        int16_t cmd_x1;
        int16_t cmd_y1;
        int16_t cmd_x2;
        int16_t cmd_y2;
        int16_t cmd_x3;
        int16_t cmd_y3;
        int16_t cmd_x4;
        int16_t cmd_y4;
} __attribute__ ((packed, aligned(32)));

extern void vdp1_cmdt_list_init(void);
extern void vdp1_cmdt_list_begin(uint32_t);
extern void vdp1_cmdt_list_end(uint32_t);
extern void vdp1_cmdt_list_clear(uint32_t);

extern void vdp1_cmdt_polygon_draw(struct vdp1_cmdt_polygon *);
extern void vdp1_cmdt_user_clip_coord_set(uint8_t, uint8_t, uint8_t, uint8_t);
extern void vdp1_cmdt_sys_clip_coord_set(int16_t, int16_t);
extern void vdp1_cmdt_local_coord_set(int16_t, int16_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CMDT_H_ */
