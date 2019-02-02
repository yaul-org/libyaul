/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <vdp1/cmdt.h>
#include <vdp1/vram.h>

#include "vdp-internal.h"

static inline void __always_inline _cmdt_cmd_grda_set(struct vdp1_cmdt *, uint32_t);
static inline void __always_inline _cmdt_cmd_colr_set(struct vdp1_cmdt *, const struct vdp1_cmdt_sprite *);
static inline void __always_inline _cmdt_cmd_srca_set(struct vdp1_cmdt *, const struct vdp1_cmdt_sprite *);
static inline void __always_inline _cmdt_cmd_size_set(struct vdp1_cmdt *, const struct vdp1_cmdt_sprite *);

static inline void __always_inline _cmdt_list_assert(const struct vdp1_cmdt_list *);

struct vdp1_cmdt_list *
vdp1_cmdt_list_alloc(uint16_t count)
{
        assert(count > 0);

        struct vdp1_cmdt_list *cmdt_list;
        cmdt_list = malloc(sizeof(struct vdp1_cmdt_list));
        assert(cmdt_list != NULL);

        struct vdp1_cmdt *cmdts;
        cmdts = malloc(count * sizeof(struct vdp1_cmdt));
        assert(cmdts != NULL);

        vdp1_cmdt_list_init(cmdt_list, cmdts, count);

        return cmdt_list;
}

void
vdp1_cmdt_list_free(struct vdp1_cmdt_list *cmdt_list)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);

        free(cmdt_list->cmdts);
        free(cmdt_list);
}

void
vdp1_cmdt_list_init(struct vdp1_cmdt_list *cmdt_list, struct vdp1_cmdt *cmdts, uint16_t count)
{
        assert(cmdt_list != NULL);
        assert(cmdts != NULL);
        assert(count > 0);

        cmdt_list->cmdts = cmdts;
        cmdt_list->cmdt = cmdt_list->cmdts;
        cmdt_list->count = count;
}

void
vdp1_cmdt_list_reset(struct vdp1_cmdt_list *cmdt_list)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);
        assert(cmdt_list->count > 0);

        cmdt_list->cmdt = cmdt_list->cmdts;
}

struct vdp1_cmdt *
vdp1_cmdt_base_get(void)
{
        return (struct vdp1_cmdt *)_state_vdp1()->vram.cmdt_base;
}

void
vdp1_cmdt_normal_sprite_draw(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_sprite *sprite)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt->cmd_ctrl = 0x0000;
        cmdt->cmd_link = 0x0000;

        cmdt->cmd_pmod = (sprite->cs_mode.raw & 0x9FFF) ^ 0x00C0;

        cmdt->cmd_xa = sprite->cs_position.x;
        cmdt->cmd_ya = sprite->cs_position.y;

        _cmdt_cmd_colr_set(cmdt, sprite);
        _cmdt_cmd_srca_set(cmdt, sprite);
        _cmdt_cmd_size_set(cmdt, sprite);
        _cmdt_cmd_grda_set(cmdt, sprite->cs_grad);

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_scaled_sprite_draw(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_sprite *sprite)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        uint16_t zp;
        zp = (sprite->cs_zoom_point.enable)
            ? dlog2(sprite->cs_zoom_point.raw & ~0x0010)
            : 0x0000;

        cmdt->cmd_ctrl = (zp << 8) | 0x0001;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = sprite->cs_mode.raw;

        if (sprite->cs_zoom_point.enable) {
                cmdt->cmd_xa = sprite->cs_zoom.point.x;
                cmdt->cmd_ya = sprite->cs_zoom.point.y;
                cmdt->cmd_xb = sprite->cs_zoom.display.x;
                cmdt->cmd_yb = sprite->cs_zoom.display.y;
        } else {
                /* Scale with two vertices, A and C. No zoom point */
                cmdt->cmd_xa = sprite->cs_vertex.a.x;
                cmdt->cmd_ya = sprite->cs_vertex.a.y;
                cmdt->cmd_xc = sprite->cs_vertex.c.x;
                cmdt->cmd_yc = sprite->cs_vertex.c.y;
        }

        _cmdt_cmd_colr_set(cmdt, sprite);
        _cmdt_cmd_srca_set(cmdt, sprite);
        _cmdt_cmd_size_set(cmdt, sprite);
        _cmdt_cmd_grda_set(cmdt, sprite->cs_grad);

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_distorted_sprite_draw(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_sprite *sprite)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

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

        _cmdt_cmd_colr_set(cmdt, sprite);
        _cmdt_cmd_srca_set(cmdt, sprite);
        _cmdt_cmd_size_set(cmdt, sprite);
        _cmdt_cmd_grda_set(cmdt, sprite->cs_grad);

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_polygon_draw(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_polygon *polygon)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt->cmd_ctrl = 0x0004;
        cmdt->cmd_link = 0x0000;
        /* Force bit 6 and 7 to be set */
        cmdt->cmd_pmod = polygon->cp_mode.raw | 0x00C0;
        cmdt->cmd_colr = polygon->cp_color.raw;

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

        _cmdt_cmd_grda_set(cmdt, polygon->cp_grad);

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_polyline_draw(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_polyline *polyline)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt->cmd_ctrl = 0x0005;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = polyline->cl_mode.raw | 0x00C0;
        cmdt->cmd_colr = polyline->cl_color.raw;
        /* CCW starting from vertex D */
        cmdt->cmd_xd = polyline->cl_vertex.d.x;
        cmdt->cmd_yd = polyline->cl_vertex.d.y;
        cmdt->cmd_xa = polyline->cl_vertex.a.x;
        cmdt->cmd_ya = polyline->cl_vertex.a.y;
        cmdt->cmd_xb = polyline->cl_vertex.b.x;
        cmdt->cmd_yb = polyline->cl_vertex.b.y;
        cmdt->cmd_xc = polyline->cl_vertex.c.x;
        cmdt->cmd_yc = polyline->cl_vertex.c.y;

        _cmdt_cmd_grda_set(cmdt, polyline->cl_grad);

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_line_draw(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_line *line)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt->cmd_ctrl = 0x0006;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = line->cl_mode.raw | 0x00C0;
        cmdt->cmd_colr = line->cl_color.raw;
        /* CCW starting from vertex D */
        cmdt->cmd_xa = line->cl_vertex.a.x;
        cmdt->cmd_ya = line->cl_vertex.a.y;
        cmdt->cmd_xb = line->cl_vertex.b.x;
        cmdt->cmd_yb = line->cl_vertex.b.y;

        _cmdt_cmd_grda_set(cmdt, line->cl_grad);

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_user_clip_coord_set(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_user_clip_coord *user_clip)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

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
        cmdt->cmd_link = 0x0000;
        /* Upper-left (x1, y1) */
        cmdt->cmd_xa = x0 & 0x03FF;
        cmdt->cmd_ya = y0 & 0x01FF;
        /* Lower-right (x2, y2) */
        cmdt->cmd_xc = x1 & 0x03FF;
        cmdt->cmd_yc = y1 & 0x01FF;

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_system_clip_coord_set(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_system_clip_coord *system_clip)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt->cmd_ctrl = 0x0009;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_xc = system_clip->scc_coord.x;
        cmdt->cmd_yc = system_clip->scc_coord.y;

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_local_coord_set(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_local_coord *local)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt->cmd_ctrl = 0x000A;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_xa = local->lc_coord.x;
        cmdt->cmd_ya = local->lc_coord.y;

        cmdt_list->cmdt++;
}

void
vdp1_cmdt_end(struct vdp1_cmdt_list *cmdt_list)
{
        _cmdt_list_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt->cmd_ctrl |= 0x8000;
        cmdt->cmd_link = 0x0000;

        cmdt_list->cmdt++;
}

static inline void __always_inline
_cmdt_cmd_grda_set(struct vdp1_cmdt *cmdt, uint32_t grad)
{
        /* Gouraud shading processing is valid when a color calculation
         * mode is specified */
        cmdt->cmd_grda = (grad >> 3) & 0xFFFF;
}

static inline void __always_inline
_cmdt_cmd_colr_set(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_sprite *sprite)
{
        switch (sprite->cs_mode.color_mode) {
        case 0:
                cmdt->cmd_colr = sprite->cs_sprite_type.raw & 0xFFF0;
                break;
        case 1:
                cmdt->cmd_colr = (uint16_t)((sprite->cs_clut >> 3) & 0xFFFF);
                break;
        case 2:
                cmdt->cmd_colr = sprite->cs_sprite_type.raw & 0xFFC0;
                break;
        case 3:
                cmdt->cmd_colr = sprite->cs_sprite_type.raw & 0xFF80;
                break;
        case 4:
                cmdt->cmd_colr = sprite->cs_sprite_type.raw & 0xFF00;
                break;
        case 5:
                break;
        }
}

static inline void __always_inline
_cmdt_cmd_srca_set(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_sprite *sprite)
{
        cmdt->cmd_srca = (sprite->cs_char >> 3) & 0xFFFF;
}

static inline void __always_inline
_cmdt_cmd_size_set(struct vdp1_cmdt *cmdt,
    const struct vdp1_cmdt_sprite *sprite)
{
        uint16_t width;
        width = sprite->cs_width;

        uint16_t height;
        height = sprite->cs_height;

        cmdt->cmd_size = (((width >> 3) << 8) | height) & 0x3FFF;
}

static inline void __always_inline
_cmdt_list_assert(const struct vdp1_cmdt_list *cmdt_list)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);
        assert(cmdt_list->cmdt != NULL);
        assert(cmdt_list->cmdt >= cmdt_list->cmdts);
        assert(cmdt_list->count > 0);
        assert((uint16_t)((cmdt_list->cmdt + 1) - cmdt_list->cmdts) <= cmdt_list->count);
}
