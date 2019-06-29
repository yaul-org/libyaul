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
static inline void __always_inline _cmdt_cmd_colr_set(struct vdp1_cmdt *, const void *);
static inline void __always_inline _cmdt_cmd_srca_set(struct vdp1_cmdt *, const void *);
static inline void __always_inline _cmdt_cmd_size_set(struct vdp1_cmdt *, const void *);

#ifdef DEBUG
static inline void __always_inline _cmdt_list_assert(const struct vdp1_cmdt_list *);
static inline void __always_inline _cmdt_list_next_assert(const struct vdp1_cmdt_list *);
static inline void __always_inline _cmdt_jump_assert(const struct vdp1_cmdt_list *, uint8_t);
#else
#define _cmdt_list_assert(x)
#define _cmdt_list_next_assert(x)
#define _cmdt_jump_assert(x, y)
#endif /* DEBUG */

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

uint16_t
vdp1_cmdt_normal_sprite_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_normal_sprite *sprite)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        cmdt->cmd_ctrl = sprite->direction.raw & 0x0030;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = sprite->draw_mode.raw & 0x9FFF;

        _cmdt_cmd_colr_set(cmdt, sprite);
        _cmdt_cmd_srca_set(cmdt, sprite);
        _cmdt_cmd_size_set(cmdt, sprite);
        _cmdt_cmd_grda_set(cmdt, sprite->grad_base);

        cmdt->cmd_xa = sprite->position.x;
        cmdt->cmd_ya = sprite->position.y;

        return index;
}

uint16_t
vdp1_cmdt_scaled_sprite_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_scaled_sprite *sprite)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        cmdt->cmd_ctrl = (sprite->direction.raw & 0x0030) | 0x0001;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = sprite->draw_mode.raw & 0x9FFF;

        _cmdt_cmd_colr_set(cmdt, sprite);
        _cmdt_cmd_srca_set(cmdt, sprite);
        _cmdt_cmd_size_set(cmdt, sprite);
        _cmdt_cmd_grda_set(cmdt, sprite->grad_base);

        if (!sprite->zoom_point.enable) {
                /* Scale with two vertices, A and C. No zoom point */
                cmdt->cmd_xa = sprite->vertex.a.x;
                cmdt->cmd_ya = sprite->vertex.a.y;
                cmdt->cmd_xc = sprite->vertex.c.x;
                cmdt->cmd_yc = sprite->vertex.c.y;

                return index;
        }

        uint16_t zp;
        zp = (sprite->zoom_point.enable)
            ? uint32_log2(sprite->zoom_point.raw & ~0x0010)
            : 0x0000;

        cmdt->cmd_ctrl |= zp << 8;

        cmdt->cmd_xa = sprite->zoom.point.x;
        cmdt->cmd_ya = sprite->zoom.point.y;
        cmdt->cmd_xb = sprite->zoom.display.x;
        cmdt->cmd_yb = sprite->zoom.display.y;

        return index;
}

uint16_t
vdp1_cmdt_distorted_sprite_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_distorted_sprite *sprite)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        cmdt->cmd_ctrl = (sprite->direction.raw & 0x0030) | 0x0002;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_pmod = sprite->draw_mode.raw & 0x9FFF;

        _cmdt_cmd_colr_set(cmdt, sprite);
        _cmdt_cmd_srca_set(cmdt, sprite);
        _cmdt_cmd_size_set(cmdt, sprite);
        _cmdt_cmd_grda_set(cmdt, sprite->grad_base);

        /*-
         * CCW starting from vertex D
         * D------------C
         * |            |
         * |            |
         * |            |
         * |            |
         * A------------B
         */

        cmdt->cmd_xa = sprite->vertex.a.x;
        cmdt->cmd_ya = sprite->vertex.a.y;
        cmdt->cmd_xb = sprite->vertex.b.x;
        cmdt->cmd_yb = sprite->vertex.b.y;
        cmdt->cmd_xc = sprite->vertex.c.x;
        cmdt->cmd_yc = sprite->vertex.c.y;
        cmdt->cmd_xd = sprite->vertex.d.x;
        cmdt->cmd_yd = sprite->vertex.d.y;

        return index;
}

uint16_t
vdp1_cmdt_polygon_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_polygon *polygon)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        cmdt->cmd_ctrl = 0x0004;
        cmdt->cmd_link = 0x0000;
        /* Force bit 6 and 7 to be set */
        cmdt->cmd_pmod = polygon->draw_mode.raw | 0x00C0;
        cmdt->cmd_colr = polygon->color.raw;

        _cmdt_cmd_grda_set(cmdt, polygon->grad_base);

        /*-
         * CCW starting from vertex D
         * D------------C
         * |            |
         * |            |
         * |            |
         * |            |
         * A------------B
         */

        cmdt->cmd_xa = polygon->vertex.a.x;
        cmdt->cmd_ya = polygon->vertex.a.y;
        cmdt->cmd_xb = polygon->vertex.b.x;
        cmdt->cmd_yb = polygon->vertex.b.y;
        cmdt->cmd_xc = polygon->vertex.c.x;
        cmdt->cmd_yc = polygon->vertex.c.y;
        cmdt->cmd_xd = polygon->vertex.d.x;
        cmdt->cmd_yd = polygon->vertex.d.y;

        return index;
}

uint16_t
vdp1_cmdt_polyline_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_polyline *polyline)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        cmdt->cmd_ctrl = 0x0005;
        cmdt->cmd_link = 0x0000;
        /* Force bit 6 and 7 to be set */
        cmdt->cmd_pmod = polyline->draw_mode.raw | 0x00C0;
        cmdt->cmd_colr = polyline->color.raw;

        _cmdt_cmd_grda_set(cmdt, polyline->grad_base);

        /* CCW starting from vertex D */
        cmdt->cmd_xd = polyline->vertex.d.x;
        cmdt->cmd_yd = polyline->vertex.d.y;
        cmdt->cmd_xa = polyline->vertex.a.x;
        cmdt->cmd_ya = polyline->vertex.a.y;
        cmdt->cmd_xb = polyline->vertex.b.x;
        cmdt->cmd_yb = polyline->vertex.b.y;
        cmdt->cmd_xc = polyline->vertex.c.x;
        cmdt->cmd_yc = polyline->vertex.c.y;

        return index;
}

uint16_t
vdp1_cmdt_line_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_line *line)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        cmdt->cmd_ctrl = 0x0006;
        cmdt->cmd_link = 0x0000;
        /* Force bit 6 and 7 to be set */
        cmdt->cmd_pmod = line->draw_mode.raw | 0x00C0;
        cmdt->cmd_colr = line->color.raw;

        _cmdt_cmd_grda_set(cmdt, line->grad_base);

        cmdt->cmd_xa = line->vertex.a.x;
        cmdt->cmd_ya = line->vertex.a.y;
        cmdt->cmd_xb = line->vertex.b.x;
        cmdt->cmd_yb = line->vertex.b.y;

        return index;
}

uint16_t
vdp1_cmdt_user_clip_coord_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_user_clip_coord *user_clip)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        int16_t x0;
        x0 = user_clip->coords[0].x;

        int16_t y0;
        y0 = user_clip->coords[0].y;

        int16_t x1;
        x1 = user_clip->coords[1].x;

        int16_t y1;
        y1 = user_clip->coords[1].y;

        assert(((x0 <= x1) && (y0 <= y1)));

        cmdt->cmd_ctrl = 0x0008;
        cmdt->cmd_link = 0x0000;
        /* Upper-left (x1, y1) */
        cmdt->cmd_xa = x0 & 0x03FF;
        cmdt->cmd_ya = y0 & 0x01FF;
        /* Lower-right (x2, y2) */
        cmdt->cmd_xc = x1 & 0x03FF;
        cmdt->cmd_yc = y1 & 0x01FF;

        return index;
}

uint16_t
vdp1_cmdt_system_clip_coord_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_system_clip_coord *system_clip)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        cmdt->cmd_ctrl = 0x0009;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_xc = system_clip->coord.x;
        cmdt->cmd_yc = system_clip->coord.y;

        return index;
}

uint16_t
vdp1_cmdt_local_coord_add(struct vdp1_cmdt_list *cmdt_list,
    const struct vdp1_cmdt_local_coord *local)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        uint16_t index;
        index = cmdt - &cmdt_list->cmdts[0];

        cmdt->cmd_ctrl = 0x000A;
        cmdt->cmd_link = 0x0000;
        cmdt->cmd_xa = local->coord.x;
        cmdt->cmd_ya = local->coord.y;

        return index;
}

void
vdp1_cmdt_end(struct vdp1_cmdt_list *cmdt_list)
{
        _cmdt_list_assert(cmdt_list);
        _cmdt_list_next_assert(cmdt_list);

        struct vdp1_cmdt *cmdt;
        cmdt = cmdt_list->cmdt;

        cmdt_list->cmdt++;

        cmdt->cmd_ctrl |= 0x8000;
        cmdt->cmd_link = 0x0000;
}

void
vdp1_cmdt_jump_clear(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
}

void
vdp1_cmdt_jump_assign(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index,
    uint8_t link_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x1000;
        cmdt->cmd_link = link_index & 0xFFFC;
}

void
vdp1_cmdt_jump_call(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index,
    uint8_t link_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x2000;
        cmdt->cmd_link = link_index & 0xFFFC;
}

void
vdp1_cmdt_jump_skip_assign(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index,
    uint8_t link_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x5000;
        cmdt->cmd_link = link_index & 0xFFFC;
}

void
vdp1_cmdt_jump_skip_call(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index,
    uint8_t link_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x6000;
        cmdt->cmd_link = link_index & 0xFFFC;
}

void
vdp1_cmdt_jump_next(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
}

void
vdp1_cmdt_jump_return(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x3000;
}

void
vdp1_cmdt_jump_skip_next(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x4000;
}

void
vdp1_cmdt_jump_skip_return(struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index)
{
        _cmdt_jump_assert(cmdt_list, cmdt_index);

        struct vdp1_cmdt *cmdt;
        cmdt = &cmdt_list->cmdts[cmdt_index];

        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x7000;
}

static inline void __always_inline
_cmdt_cmd_grda_set(struct vdp1_cmdt *cmdt, uint32_t grad)
{
        /* Gouraud shading processing is valid when a color calculation
         * mode is specified */
        cmdt->cmd_grda = (grad >> 3) & 0xFFFF;
}

static inline void __always_inline
_cmdt_cmd_colr_set(struct vdp1_cmdt *cmdt, const void *base_sprite)
{
        const struct vdp1_cmdt_normal_sprite *sprite;
        sprite = base_sprite;

        switch (sprite->draw_mode.color_mode) {
        case 0:
                cmdt->cmd_colr = sprite->sprite_type.raw & 0xFFF0;
                break;
        case 1:
                cmdt->cmd_colr = (uint16_t)((sprite->clut >> 3) & 0xFFFF);
                break;
        case 2:
                cmdt->cmd_colr = sprite->sprite_type.raw & 0xFFC0;
                break;
        case 3:
                cmdt->cmd_colr = sprite->sprite_type.raw & 0xFF80;
                break;
        case 4:
                cmdt->cmd_colr = sprite->sprite_type.raw & 0xFF00;
                break;
        case 5:
                break;
        }
}

static inline void __always_inline
_cmdt_cmd_srca_set(struct vdp1_cmdt *cmdt, const void *base_sprite)
{
        const struct vdp1_cmdt_normal_sprite *sprite;
        sprite = base_sprite;

        cmdt->cmd_srca = (sprite->char_base >> 3) & 0xFFFF;
}

static inline void __always_inline
_cmdt_cmd_size_set(struct vdp1_cmdt *cmdt, const void *base_sprite)
{
        const struct vdp1_cmdt_normal_sprite *sprite;
        sprite = base_sprite;

        uint16_t width;
        width = sprite->width;

        uint16_t height;
        height = sprite->height;

        cmdt->cmd_size = (((width >> 3) << 8) | height) & 0x3FFF;
}

#ifdef DEBUG
static inline void __always_inline
_cmdt_list_assert(const struct vdp1_cmdt_list *cmdt_list)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);
        assert(cmdt_list->cmdt != NULL);
        assert(cmdt_list->cmdt >= cmdt_list->cmdts);
        assert(cmdt_list->count > 0);
}

static inline void __always_inline
_cmdt_list_next_assert(const struct vdp1_cmdt_list *cmdt_list)
{
        assert((uint16_t)((cmdt_list->cmdt + 1) - cmdt_list->cmdts) <= cmdt_list->count);
}

static inline void __always_inline
_cmdt_jump_assert(const struct vdp1_cmdt_list *cmdt_list, uint8_t cmdt_index)
{
        _cmdt_list_assert(cmdt_list);

        assert(cmdt_index < cmdt_list->count);
}
#endif /* DEBUG */
