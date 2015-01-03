/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>

#include <vdp1/cmdt.h>
#include <vdp1/vram.h>

#include "vdp1-internal.h"

#define VDP1_CMDT_LIST_CNT 16

static struct vdp1_cmdt_list_state {
        /* Has vdp1_cmdt_list_init() been called? */
        bool cts_initialized;
        /* Are we currently inside a BEGIN/END block? */
        bool cts_in_list;
        /* Current list ID (currently inside BEGIN/END block) */
        int32_t cts_cur_id;
        /* Last list ID */
        int32_t cts_last_id;
} vdp1_cmdt_list_state;

static struct vdp1_cmdt_list {
        struct vdp1_cmdt *ctl_beg_cmdt;
        struct vdp1_cmdt *ctl_cur_cmdt;
        uint32_t ctl_ref_cnt;
} vdp1_cmdt_list_lists[VDP1_CMDT_LIST_CNT];

static inline struct vdp1_cmdt *fetch_cmdt(void) __attribute__ ((always_inline));

void
vdp1_cmdt_list_init(void)
{
        struct vdp1_cmdt_list_state *state;
        state = &vdp1_cmdt_list_state;

        state->cts_initialized = true;
        state->cts_in_list = false;
        state->cts_cur_id = -1;
        state->cts_last_id = -1;

        uint32_t id;
        for (id = 0; id < VDP1_CMDT_LIST_CNT; id++) {
                struct vdp1_cmdt_list *list;
                list = &vdp1_cmdt_list_lists[id];

                list->ctl_beg_cmdt = (struct vdp1_cmdt *)CMD_TABLE(0, 0);
                list->ctl_cur_cmdt = list->ctl_beg_cmdt;
                list->ctl_ref_cnt = 0;
        }
}

void
vdp1_cmdt_list_begin(uint32_t id)
{
        assert((id >= 0) && (id < VDP1_CMDT_LIST_CNT));

        struct vdp1_cmdt_list_state *state;
        state = &vdp1_cmdt_list_state;

        struct vdp1_cmdt_list *list;
        list = &vdp1_cmdt_list_lists[id];

        assert(state->cts_initialized);

        /* Check if we're not already in a list */
        assert(!state->cts_in_list);
        state->cts_in_list = !state->cts_in_list;

        state->cts_cur_id = id;

        if (state->cts_last_id >= 0) {
                if (list->ctl_ref_cnt == 0) {
                        /* Case where this list has never been used */
                        struct vdp1_cmdt_list *last_list;
                        last_list = &vdp1_cmdt_list_lists[state->cts_last_id];

                        /* Undo draw end command from previous list */
                        struct vdp1_cmdt *last_cur_cmdt;
                        last_cur_cmdt = last_list->ctl_cur_cmdt;
                        last_cur_cmdt--;

                        bool draw_end;
                        draw_end = (last_cur_cmdt->cmd_ctrl & 0x8000) != 0;
                        if (!draw_end) {
                                last_cur_cmdt++;
                        }

                        /* Set pointer to next command table */
                        list->ctl_beg_cmdt = last_cur_cmdt;
                        list->ctl_cur_cmdt = list->ctl_beg_cmdt;
                } else {
                        list->ctl_cur_cmdt = list->ctl_beg_cmdt;
                }
        }

        list->ctl_ref_cnt++;
}

void
vdp1_cmdt_list_end(uint32_t id)
{
        assert((id >= 0) && (id < VDP1_CMDT_LIST_CNT));

        struct vdp1_cmdt_list_state *state;
        state = &vdp1_cmdt_list_state;

        struct vdp1_cmdt_list *list;
        list = &vdp1_cmdt_list_lists[id];

        /* Check if we're in a list */
        assert(state->cts_in_list);
        state->cts_in_list = !state->cts_in_list;

        assert(state->cts_cur_id == id);
        state->cts_last_id = state->cts_cur_id;
        state->cts_cur_id = -1;
}

void
vdp1_cmdt_list_clear(uint32_t id)
{
        assert((id >= 0) && (id < VDP1_CMDT_LIST_CNT));

        struct vdp1_cmdt_list_state *state;
        state = &vdp1_cmdt_list_state;

        struct vdp1_cmdt_list *list;
        list = &vdp1_cmdt_list_lists[id];

        assert(state->cts_initialized);
        assert(!state->cts_in_list);

        list->ctl_ref_cnt = 0;
}

void
vdp1_cmdt_list_clear_all(void)
{
        uint32_t id;
        for (id = 0; id < VDP1_CMDT_LIST_CNT; id++) {
                vdp1_cmdt_list_clear(id);
        }
}

void
vdp1_cmdt_sprite_draw(struct vdp1_cmdt_sprite *sprite)
{
        struct vdp1_cmdt *cmdt;
        cmdt = fetch_cmdt();

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
                    ? common_log2_down(sprite->cs_zoom_point.raw & ~0x0010)
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
                cmdt->cmd_colr = sprite->cs_color_bank << 4;
                break;
        case 1:
                cmdt->cmd_colr =
                    (uint16_t)((sprite->cs_clut >> 3) & 0xFFFF);
                break;
        case 2:
                cmdt->cmd_colr = sprite->cs_color_bank << 6;
                break;
        case 3:
                cmdt->cmd_colr = sprite->cs_color_bank << 7;
                break;
        case 4:
                cmdt->cmd_colr = sprite->cs_color_bank << 8;
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
vdp1_cmdt_polygon_draw(struct vdp1_cmdt_polygon *polygon)
{
        struct vdp1_cmdt *cmdt;
        cmdt = fetch_cmdt();

        cmdt->cmd_ctrl = 0x0004;
        cmdt->cmd_pmod = polygon->cp_mode.raw;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_colr = polygon->cp_color;
        /* CCW starting from vertex D */
        cmdt->cmd_xd = polygon->cp_vertex.d.x;
        cmdt->cmd_yd = polygon->cp_vertex.d.y;
        cmdt->cmd_xa = polygon->cp_vertex.a.x;
        cmdt->cmd_ya = polygon->cp_vertex.a.y;
        cmdt->cmd_xb = polygon->cp_vertex.b.x;
        cmdt->cmd_yb = polygon->cp_vertex.b.y;
        cmdt->cmd_xc = polygon->cp_vertex.c.x;
        cmdt->cmd_yc = polygon->cp_vertex.c.y;
        /* Gouraud shading processing is valid when a color calculation
         * mode is specified */
        cmdt->cmd_grda = (polygon->cp_grad >> 3) & 0xFFFF;
}

void
vdp1_cmdt_polyline_draw(struct vdp1_cmdt_polyline *polyline)
{
        struct vdp1_cmdt *cmdt;
        cmdt = fetch_cmdt();

        cmdt->cmd_ctrl = 0x0005;
        cmdt->cmd_pmod = polyline->cp_mode.raw | 0x00C0;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_colr = polyline->cp_color;
        /* CCW starting from vertex D */
        cmdt->cmd_xd = polyline->cp_vertex.d.x;
        cmdt->cmd_yd = polyline->cp_vertex.d.y;
        cmdt->cmd_xa = polyline->cp_vertex.a.x;
        cmdt->cmd_ya = polyline->cp_vertex.a.y;
        cmdt->cmd_xb = polyline->cp_vertex.b.x;
        cmdt->cmd_yb = polyline->cp_vertex.b.y;
        cmdt->cmd_xc = polyline->cp_vertex.c.x;
        cmdt->cmd_yc = polyline->cp_vertex.c.y;
        /* Gouraud shading processing is valid when a color calculation
         * mode is specified */
        cmdt->cmd_grda = (polyline->cp_grad >> 3) & 0xFFFF;
}

void
vdp1_cmdt_line_draw(struct vdp1_cmdt_line *line)
{
        struct vdp1_cmdt *cmdt;
        cmdt = fetch_cmdt();

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
vdp1_cmdt_user_clip_coord_set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
        struct vdp1_cmdt *cmdt;
        cmdt = fetch_cmdt();

        assert((x1 <= x2) && (y1 <= y2));

        cmdt->cmd_ctrl = 0x0008;
        /* Upper-left (x1, y1) */
        cmdt->cmd_xa = x1 & 0x01FF;
        cmdt->cmd_ya = y1 & 0x00FF;
        /* Lower-right (x1, y1) */
        cmdt->cmd_xc = x2 & 0x01FF;
        cmdt->cmd_yc = y2 & 0x00FF;
}

void
vdp1_cmdt_system_clip_coord_set(int16_t x, int16_t y)
{
        struct vdp1_cmdt *cmdt;
        cmdt = fetch_cmdt();

        cmdt->cmd_ctrl = 0x0009;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_xc = x;
        cmdt->cmd_yc = y;
}

void
vdp1_cmdt_local_coord_set(int16_t x, int16_t y)
{
        struct vdp1_cmdt *cmdt;
        cmdt = fetch_cmdt();

        cmdt->cmd_ctrl = 0x000A;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_xa = x;
        cmdt->cmd_ya = y;
}

void
vdp1_cmdt_end(void)
{
        struct vdp1_cmdt *cmdt;
        cmdt = fetch_cmdt();

        cmdt->cmd_ctrl = 0x8000;
}

static inline struct vdp1_cmdt *
fetch_cmdt(void)
{
        struct vdp1_cmdt_list_state *state;
        state = &vdp1_cmdt_list_state;

        assert(state->cts_in_list);

        struct vdp1_cmdt_list *list;
        list = &vdp1_cmdt_list_lists[state->cts_cur_id];

        return list->ctl_cur_cmdt++;
}
