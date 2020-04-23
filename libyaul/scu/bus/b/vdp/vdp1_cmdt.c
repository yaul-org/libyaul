/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <cpu/cache.h>
#include <cpu/instructions.h>

#include <vdp1/cmdt.h>
#include <vdp1/vram.h>

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "vdp-internal.h"

struct vdp1_cmdt *
vdp1_cmdt_base_get(void)
{
        return (struct vdp1_cmdt *)_state_vdp1()->vram.cmdt_base;
}

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

        vdp1_cmdt_list_init(cmdt_list, cmdts);

        return cmdt_list;
}

void
vdp1_cmdt_list_free(struct vdp1_cmdt_list *cmdt_list)
{
        assert(cmdt_list != NULL);
        assert(cmdt_list->cmdts != NULL);

        cmdt_list->count = 0;

        free(cmdt_list->cmdts);
        free(cmdt_list);
}

void
vdp1_cmdt_list_init(struct vdp1_cmdt_list *cmdt_list, struct vdp1_cmdt *cmdts)
{
        assert(cmdt_list != NULL);
        assert(cmdts != NULL);

        cmdt_list->cmdts = cmdts;
        cmdt_list->count = 0;
}

struct vdp1_cmdt_orderlist *
vdp1_cmdt_orderlist_alloc(uint16_t count)
{
        assert(count > 0);

        uint32_t size;
        size = count * sizeof(struct vdp1_cmdt);

        uint32_t aligned_boundary;
        aligned_boundary = dlog2(size);

        if ((aligned_boundary & (aligned_boundary - 1)) != 0x00000000) {
                aligned_boundary++;
        }

        struct vdp1_cmdt_orderlist *cmdt_orderlist;
        cmdt_orderlist = memalign(sizeof(struct vdp1_cmdt_orderlist), aligned_boundary);
        assert(cmdt_orderlist != NULL);

        vdp1_cmdt_orderlist_init(cmdt_orderlist, count);

        return cmdt_orderlist;
}

void
vdp1_cmdt_orderlist_free(struct vdp1_cmdt_orderlist *cmdt_orderlist)
{
        assert(cmdt_orderlist != NULL);

        free(cmdt_orderlist);
}

void
vdp1_cmdt_orderlist_init(struct vdp1_cmdt_orderlist *cmdt_orderlist, uint16_t count)
{
        assert(cmdt_orderlist != NULL);
        assert(count > 0);

        struct scu_dma_xfer *xfer_table;
        xfer_table = (struct scu_dma_xfer *)cmdt_orderlist;

        for (uint32_t i = 0; i < count; i++) {
                xfer_table[i].len = sizeof(struct vdp1_cmdt);
                xfer_table[i].dst = 0x00000000;
                xfer_table[i].src = 0x00000000;
        }
}

void
vdp1_cmdt_orderlist_vram_patch(struct vdp1_cmdt_orderlist *cmdt_orderlist, const uint32_t base, const uint16_t count)
{
        assert(cmdt_orderlist != NULL);
        assert(count > 0);

        struct scu_dma_xfer *xfer_table;
        xfer_table = (struct scu_dma_xfer *)cmdt_orderlist;

        for (uint32_t i = 0; i < count; i++) {
                xfer_table[i].dst = base + (i * sizeof(struct vdp1_cmdt));
        }

        xfer_table[count - 1].len |= SCU_DMA_INDIRECT_TBL_END;
}

void
vdp1_cmdt_param_draw_mode_set(struct vdp1_cmdt *cmdt,
    const vdp1_cmdt_draw_mode draw_mode)
{
        /* Values 0x4, 0x5, 0x6 for comm indicate a non-textured command table,
         * and we want to set the bits 7 and 6 without branching */
        uint16_t comm;
        comm = (cmdt->cmd_ctrl & 0x0004);

        uint32_t pmod_bits;
        pmod_bits = (comm << 5) | (comm << 4);

        cmdt->cmd_pmod = pmod_bits | draw_mode.raw;
}

void
vdp1_cmdt_param_zoom_set(struct vdp1_cmdt *cmdt, const uint8_t zoom_point)
{
        cmdt->cmd_ctrl &= 0xF0F0;
        cmdt->cmd_ctrl |= (zoom_point << 8) | 0x0001;
}

void
vdp1_cmdt_param_char_base_set(struct vdp1_cmdt *cmdt, uint32_t base)
{
        cmdt->cmd_srca = (base >> 3) & 0xFFFF;
}

void
vdp1_cmdt_param_color_set(struct vdp1_cmdt *cmdt, color_rgb555_t color)
{
        cmdt->cmd_colr = color.raw;
}

void
vdp1_cmdt_param_color_bank_set(struct vdp1_cmdt *cmdt,
    const vdp1_cmdt_color_bank color_bank)
{
        cmdt->cmd_colr = color_bank.raw;
}

void
vdp1_cmdt_param_color_mode0_set(struct vdp1_cmdt *cmdt,
    const vdp1_cmdt_color_bank color_bank)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_colr = color_bank.raw & 0xFFF0;
}

void
vdp1_cmdt_param_color_mode1_set(struct vdp1_cmdt *cmdt, uint32_t base)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_pmod |= 0x0008;
        cmdt->cmd_colr = (uint16_t)((base >> 3) & 0xFFFF);
}

void
vdp1_cmdt_param_color_mode2_set(struct vdp1_cmdt *cmdt,
    const vdp1_cmdt_color_bank color_bank)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_pmod |= 0x0010;
        cmdt->cmd_colr = color_bank.raw & 0xFFF0;
}

void
vdp1_cmdt_param_color_mode3_set(struct vdp1_cmdt *cmdt,
    const vdp1_cmdt_color_bank color_bank)
{
        cmdt->cmd_pmod &= 0xFFC7;
       cmdt->cmd_pmod |= 0x0018;
        cmdt->cmd_colr = color_bank.raw & 0xFF80;
}

void
vdp1_cmdt_param_color_mode4_set(struct vdp1_cmdt *cmdt,
    const vdp1_cmdt_color_bank color_bank)
{
        cmdt->cmd_pmod &= 0xFFC7;
        cmdt->cmd_pmod |= 0x0020;
        cmdt->cmd_colr = color_bank.raw & 0xFF00;
}

void
vdp1_cmdt_param_size_set(struct vdp1_cmdt *cmdt, uint16_t width, uint16_t height)
{
        cmdt->cmd_size = (((width >> 3) << 8) | height) & 0x3FFF;
}

void
vdp1_cmdt_param_horizontal_flip_set(struct vdp1_cmdt *cmdt, bool flip)
{
        cmdt->cmd_ctrl &= 0xFFBF;
        cmdt->cmd_ctrl |= ((uint16_t)flip & 0x1)  << 6;
}

void
vdp1_cmdt_param_vertical_flip_set(struct vdp1_cmdt *cmdt, bool flip)
{
        cmdt->cmd_ctrl &= 0xFFDF;
        cmdt->cmd_ctrl |= ((uint16_t)flip & 0x1) << 5;
}

void
vdp1_cmdt_param_vertex_set(struct vdp1_cmdt *cmdt,
    uint16_t vertex_index,
    const int16_vector2_t *p)
{
        int16_vector2_t *vertex;
        vertex = (int16_vector2_t *)(&cmdt->cmd_xa + ((vertex_index & 0x3) << 1));

        vertex->x = p->x;
        vertex->y = p->y;
}

void
vdp1_cmdt_param_vertices_set(struct vdp1_cmdt *cmdt, const int16_vector2_t *p)
{
        (void)memcpy(&cmdt->cmd_xa, p, sizeof(int16_vector2_t) * 4);
}

void
vdp1_cmdt_param_gouraud_base_set(struct vdp1_cmdt *cmdt, uint32_t base)
{
        /* Gouraud shading processing is valid when a color calculation mode is
         * specified */
        cmdt->cmd_grda = (base >> 3) & 0xFFFF;
}

void
vdp1_cmdt_normal_sprite_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x0000;
}

void
vdp1_cmdt_scaled_sprite_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x0001;
}

void
vdp1_cmdt_distorted_sprite_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x0002;
}

void
vdp1_cmdt_polygon_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x0004;
}

void
vdp1_cmdt_polyline_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x0005;
}

void
vdp1_cmdt_line_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x0006;
}

void
vdp1_cmdt_user_clip_coord_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x0008;
}

void
vdp1_cmdt_system_clip_coord_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x0009;
}

void
vdp1_cmdt_local_coord_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x000A;
}

void
vdp1_cmdt_end_set(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl = 0x8000;
}

void
vdp1_cmdt_jump_clear(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
}

void
vdp1_cmdt_jump_assign(struct vdp1_cmdt *cmdt, uint16_t link_index)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x1000;
        cmdt->cmd_link = link_index & 0xFFFC;
}

void
vdp1_cmdt_jump_call(struct vdp1_cmdt *cmdt, uint16_t link_index)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x2000;
        cmdt->cmd_link = link_index & 0xFFFC;
}

void
vdp1_cmdt_jump_skip_assign(struct vdp1_cmdt *cmdt, uint16_t link_index)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x5000;
        cmdt->cmd_link = link_index & 0xFFFC;
}

void
vdp1_cmdt_jump_skip_call(struct vdp1_cmdt *cmdt, uint16_t link_index)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x6000;
        cmdt->cmd_link = link_index & 0xFFFC;
}

void
vdp1_cmdt_jump_next(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
}

void
vdp1_cmdt_jump_return(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x3000;
}

void
vdp1_cmdt_jump_skip_next(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x4000;
}

void
vdp1_cmdt_jump_skip_return(struct vdp1_cmdt *cmdt)
{
        cmdt->cmd_ctrl &= 0x8FFF;
        cmdt->cmd_ctrl |= 0x7000;
}
