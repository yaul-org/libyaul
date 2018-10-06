/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <vdp1/cmdt.h>
#include <vdp1/vram.h>

#include "vdp-internal.h"

void
vdp1_cmdt_sprite_draw(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_sprite *sprite)
{
        bool scale;
        uint16_t zp;

        switch (sprite->cs_type) {
        case CMDT_TYPE_NORMAL_SPRITE:
                cmdt->cmd_ctrl = 0x0000;

                cmdt->cmd_pmod = (sprite->cs_mode.raw & 0x9FFF) ^ 0x00C0;

                cmdt->cmd_xa = sprite->cs_position.x;
                cmdt->cmd_ya = sprite->cs_position.y;
                break;
        case CMDT_TYPE_SCALED_SPRITE:
                zp = sprite->cs_zoom_point.enable
                    ? dlog2(sprite->cs_zoom_point.raw & ~0x0010)
                    : 0x0000;

                cmdt->cmd_ctrl = (zp << 8) | 0x0001;
                cmdt->cmd_pmod = sprite->cs_mode.raw;

                scale = zp == 0x0000;
                if (scale) {
                        /* Scale with two vertices, A and C. No zoom point */
                        cmdt->cmd_xa = sprite->cs_vertex.a.x;
                        cmdt->cmd_ya = sprite->cs_vertex.a.y;
                        cmdt->cmd_xc = sprite->cs_vertex.c.x;
                        cmdt->cmd_yc = sprite->cs_vertex.c.y;
                } else {
                        cmdt->cmd_xa = sprite->cs_zoom.point.x;
                        cmdt->cmd_ya = sprite->cs_zoom.point.y;
                        cmdt->cmd_xb = sprite->cs_zoom.display.x;
                        cmdt->cmd_yb = sprite->cs_zoom.display.y;
                }
                break;
        case CMDT_TYPE_DISTORTED_SPRITE:
                cmdt->cmd_ctrl = 0x0002;
                cmdt->cmd_pmod = sprite->cs_mode.raw;

                /* CCW starting from vertex D */
                cmdt->cmd_xd = sprite->cs_vertex.d.x;
                cmdt->cmd_yd = sprite->cs_vertex.d.y;
                cmdt->cmd_xa = sprite->cs_vertex.a.x;
                cmdt->cmd_ya = sprite->cs_vertex.a.y;
                cmdt->cmd_xb = sprite->cs_vertex.b.x;
                cmdt->cmd_yb = sprite->cs_vertex.b.y;
                cmdt->cmd_xc = sprite->cs_vertex.c.x;
                cmdt->cmd_yc = sprite->cs_vertex.c.y;
                break;
        }

        cmdt->cmd_link = 0x0000;

        switch (sprite->cs_mode.color_mode) {
        case 0:
                cmdt->cmd_colr = sprite->cs_color_bank.raw & 0xFFF0;
                break;
        case 1:
                cmdt->cmd_colr = (uint16_t)((sprite->cs_clut >> 3) & 0xFFFF);
                break;
        case 2:
                cmdt->cmd_colr = sprite->cs_color_bank.raw & 0xFFC0;
                break;
        case 3:
                cmdt->cmd_colr = sprite->cs_color_bank.raw & 0xFF80;
                break;
        case 4:
                cmdt->cmd_colr = sprite->cs_color_bank.raw & 0xFF00;
                break;
        case 5:
                break;
        }

        cmdt->cmd_srca = (sprite->cs_char >> 3) & 0xFFFF;
        cmdt->cmd_size = (((sprite->cs_width >> 3) << 8) |
            sprite->cs_height) & 0x3FFF;

        /* Gouraud shading processing is valid when a color calculation
         * mode is specified */
        cmdt->cmd_grda = (sprite->cs_grad >> 3) & 0xFFFF;
}

void
vdp1_cmdt_polygon_draw(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_polygon *polygon)
{
        cmdt->cmd_ctrl = 0x0004;
        /* Force bit 6 and 7 to be set */
        cmdt->cmd_pmod = polygon->cp_mode.raw | 0x00C0;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_colr = polygon->cp_color;

        /*-
         * CCW starting from vertex D
         * D------------C
         * |            |
         * |            |
         * |            |
         * |            |
         * A------------B
         */

        cmdt->cmd_xa = polygon->cp_vertex.a.x;
        cmdt->cmd_ya = polygon->cp_vertex.a.y;
        cmdt->cmd_xb = polygon->cp_vertex.b.x;
        cmdt->cmd_yb = polygon->cp_vertex.b.y;
        cmdt->cmd_xc = polygon->cp_vertex.c.x;
        cmdt->cmd_yc = polygon->cp_vertex.c.y;
        cmdt->cmd_xd = polygon->cp_vertex.d.x;
        cmdt->cmd_yd = polygon->cp_vertex.d.y;

        /* Gouraud shading processing is valid when a color calculation
         * mode is specified */
        cmdt->cmd_grda = (polygon->cp_grad >> 3) & 0xFFFF;
}

void
vdp1_cmdt_polyline_draw(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_polyline *polyline)
{
        cmdt->cmd_ctrl = 0x0005;
        cmdt->cmd_pmod = polyline->cl_mode.raw | 0x00C0;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_colr = polyline->cl_color;
        /* CCW starting from vertex D */
        cmdt->cmd_xd = polyline->cl_vertex.d.x;
        cmdt->cmd_yd = polyline->cl_vertex.d.y;
        cmdt->cmd_xa = polyline->cl_vertex.a.x;
        cmdt->cmd_ya = polyline->cl_vertex.a.y;
        cmdt->cmd_xb = polyline->cl_vertex.b.x;
        cmdt->cmd_yb = polyline->cl_vertex.b.y;
        cmdt->cmd_xc = polyline->cl_vertex.c.x;
        cmdt->cmd_yc = polyline->cl_vertex.c.y;
        /* Gouraud shading processing is valid when a color calculation
         * mode is specified */
        cmdt->cmd_grda = (polyline->cl_grad >> 3) & 0xFFFF;
}

void
vdp1_cmdt_line_draw(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_line *line)
{
        cmdt->cmd_ctrl = 0x0006;
        cmdt->cmd_pmod = line->cl_mode.raw | 0x00C0;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_colr = line->cl_color;
        /* CCW starting from vertex D */
        cmdt->cmd_xa = line->cl_vertex.a.x;
        cmdt->cmd_ya = line->cl_vertex.a.y;
        cmdt->cmd_xb = line->cl_vertex.b.x;
        cmdt->cmd_yb = line->cl_vertex.b.y;
        /* Gouraud shading processing is valid when a color calculation
         * mode is specified */
        cmdt->cmd_grda = (line->cl_grad >> 3) & 0xFFFF;
}

void
vdp1_cmdt_user_clip_coord_set(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_user_clip_coord *user_clip)
{
        int16_t x0;
        x0 = user_clip->ucc_coords[0].x;

        int16_t y0;
        y0 = user_clip->ucc_coords[0].y;

        int16_t x1;
        x1 = user_clip->ucc_coords[1].x;

        int16_t y1;
        y1 = user_clip->ucc_coords[1].y;

        assert(((x0 <= x1) && (y0 <= y1)));

        cmdt->cmd_ctrl = 0x0008;
        /* Upper-left (x1, y1) */
        cmdt->cmd_xa = x0 & 0x01FF;
        cmdt->cmd_ya = y0 & 0x00FF;
        /* Lower-right (x2, y2) */
        cmdt->cmd_xc = x1 & 0x01FF;
        cmdt->cmd_yc = y1 & 0x00FF;
}

void
vdp1_cmdt_system_clip_coord_set(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_system_clip_coord *system_clip)
{
        cmdt->cmd_ctrl = 0x0009;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_xc = system_clip->scc_coord.x;
        cmdt->cmd_yc = system_clip->scc_coord.y;
}

void
vdp1_cmdt_local_coord_set(struct vdp1_cmdt *cmdt,
    struct vdp1_cmdt_local_coord *local)
{
        cmdt->cmd_ctrl = 0x000A;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_xa = local->lc_coord.x;
        cmdt->cmd_ya = local->lc_coord.y;
}

void
vdp1_cmdt_end(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl |= 0x8000;
}
