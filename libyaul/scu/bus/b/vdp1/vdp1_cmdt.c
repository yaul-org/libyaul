/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdbool.h>

#include <vdp1/cmdt.h>

#include "vdp1-internal.h"

#define VDP1_CMDT_LIST_CNT 31

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

static inline struct vdp1_cmdt * cmdt(void) __attribute__((always_inline));

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

                list->ctl_beg_cmdt = NULL;
                list->ctl_cur_cmdt = NULL;
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

        if (state->cts_last_id < 0) {
                /* Case where this is the first time creating a list */
                list->ctl_beg_cmdt = (struct vdp1_cmdt *)CMD_TABLE(0, 0);
                list->ctl_cur_cmdt = list->ctl_beg_cmdt;
        } else if ((list->ctl_beg_cmdt == NULL) && (list->ctl_cur_cmdt == NULL)) {
                /* Case where this list has never been used */
                struct vdp1_cmdt_list *last_list;
                last_list = &vdp1_cmdt_list_lists[state->cts_last_id];

                /* Undo draw end command from previous list */
                struct vdp1_cmdt *last_cur_cmdt;
                last_cur_cmdt = last_list->ctl_cur_cmdt;
                last_cur_cmdt->cmd_ctrl &= ~0x8000;

                /* Set pointer to next command table */
                list->ctl_beg_cmdt = last_cur_cmdt;
                list->ctl_cur_cmdt = list->ctl_beg_cmdt;
        } else {
                list->ctl_cur_cmdt = list->ctl_beg_cmdt;
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

        /* Stop processing command tables if reference count is 1 */
        if (list->ctl_ref_cnt == 1) {
                struct vdp1_cmdt *cur_cmdt;
                cur_cmdt = list->ctl_cur_cmdt;
                cur_cmdt->cmd_ctrl |= 0x8000;
        }
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
vdp1_cmdt_polygon_draw(struct vdp1_cmdt_polygon *polygon)
{
        struct vdp1_cmdt *cur_cmdt;
        cur_cmdt = cmdt();

        cur_cmdt->cmd_ctrl = 0x0004;
        cur_cmdt->cmd_colr = polygon->cmd_color;
        /* CCW starting from vertex D */
        cur_cmdt->cmd_xd = polygon->cmd_x1;
        cur_cmdt->cmd_yd = polygon->cmd_y1;
        cur_cmdt->cmd_xa = polygon->cmd_x2;
        cur_cmdt->cmd_ya = polygon->cmd_y2;
        cur_cmdt->cmd_xb = polygon->cmd_x3;
        cur_cmdt->cmd_yb = polygon->cmd_y3;
        cur_cmdt->cmd_xc = polygon->cmd_x4;
        cur_cmdt->cmd_yc = polygon->cmd_y4;
}

void vdp1_cmdt_user_clip_coord_set(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
        struct vdp1_cmdt *cur_cmdt;
        cur_cmdt = cmdt();

        assert((x1 <= x2) && (y1 <= y2));

        cur_cmdt->cmd_ctrl = 0x0008;
        /* Upper-left (x1, y1) */
        cur_cmdt->cmd_xa = x1 & 0x00FF;
        cur_cmdt->cmd_ya = y1 & 0x00FF;
        /* Lower-right (x1, y1) */
        cur_cmdt->cmd_xc = x2 & 0x00FF;
        cur_cmdt->cmd_yc = y2 & 0x00FF;
}

void
vdp1_cmdt_sys_clip_coord_set(int16_t x, int16_t y)
{
        struct vdp1_cmdt *cur_cmdt;
        cur_cmdt = cmdt();

        cur_cmdt->cmd_ctrl = 0x0009;
        cur_cmdt->cmd_xc = x;
        cur_cmdt->cmd_yc = y;
}

void
vdp1_cmdt_local_coord_set(int16_t x, int16_t y)
{
        struct vdp1_cmdt *cur_cmdt;
        cur_cmdt = cmdt();

        cur_cmdt->cmd_ctrl = 0x000A;
        cur_cmdt->cmd_xa = x;
        cur_cmdt->cmd_ya = y;
}

void
vdp1_cmdt_normal_sprite_draw(void)
{
}

void
vdp1_cmdt_scaled_sprite_draw(void)
{
}

void
vdp1_cmdt_distorted_sprite_draw(void)
{
}

void
vdp1_cmdt_line_draw(void)
{
}

inline static struct vdp1_cmdt *
cmdt(void)
{
        struct vdp1_cmdt_list_state *state;
        state = &vdp1_cmdt_list_state;

        assert(state->cts_in_list);

        struct vdp1_cmdt_list *list;
        list = &vdp1_cmdt_list_lists[state->cts_cur_id];

        return list->ctl_cur_cmdt++;
}
