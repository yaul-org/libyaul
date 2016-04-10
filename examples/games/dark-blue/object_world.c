/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#include "fs.h"
#include "fs_texture.h"
#include "objects.h"
#include "physics.h"

/* Number of blocks sorrounding the player */
#define BLOCKS 3
/* Number of objects in the pool */
#define OBJECT_BLOCK_COUNT      ((BLOCKS * BLOCKS) + (2 * BLOCKS) + 1)

struct object_block {
        OBJECT_DECLARATIONS

        struct {
        } data;

        struct {
                struct collider m_collider;
                uint16_t m_block;
                int16_vector2_t m_block_pos;
        } private_data;
};

static void on_world_init(struct object *);
static void on_world_update(struct object *);
static void on_world_draw(struct object *);

static void on_block_init(struct object *);
static void on_block_update(struct object *);
static void on_block_draw(struct object *);
static void on_block_collision(struct object *, struct object *,
    const struct collider_info *);

static void m_update_map(struct object *, uint32_t);
static void m_update_colliders(struct object *);
static void m_start(struct object *);

struct object_world object_world = {
        .active = true,
        .on_init = on_world_init,
        .rigid_body = NULL,
        .colliders = {
                NULL
        },
        .on_update = on_world_update,
        .on_draw = on_world_draw,
        .on_destroy = NULL,
        .on_collision = NULL,
        .on_trigger = NULL,
        .private_functions = {
                .m_update_map = m_update_map,
                .m_update_colliders = m_update_colliders
        },
        .functions = {
                .m_start = m_start
        }
};

static struct object_block object_block_pool[OBJECT_BLOCK_COUNT];

static void
on_world_init(struct object *this)
{
        THIS_PRIVATE_DATA(object_world, map_fh) = fs_open("/TEST.MAP");
        assert(THIS_PRIVATE_DATA(object_world, map_fh) != NULL);

        /* World */
        THIS(object_world, transform).pos_int.x = 0;
        THIS(object_world, transform).pos_int.y = 0;

        /* Player */
        OBJECT(&object_player, transform).pos_int.x = 5 * WORLD_BLOCK_WIDTH;
        OBJECT(&object_player, transform).pos_int.y = ((WORLD_ROWS) - 11) * WORLD_BLOCK_HEIGHT;

        THIS_PRIVATE_DATA(object_world, scroll_x) = 0;
        THIS_PRIVATE_DATA(object_world, last_scroll_x) = 0;
        THIS_PRIVATE_DATA(object_world, move) = 0;
        THIS_PRIVATE_DATA(object_world, column) = 0;

        THIS_PRIVATE_DATA(object_world, column_count) =
            fs_size(THIS_PRIVATE_DATA(object_world, map_fh)) / WORLD_MAP_FORMAT_COLUMN_SIZE;
        THIS_PRIVATE_DATA(object_world, row_count) = WORLD_ROWS;

        fs_seek(THIS_PRIVATE_DATA(object_world, map_fh), 0, SEEK_SET);

        uint32_t block_idx;
        for (block_idx = 0; block_idx < OBJECT_BLOCK_COUNT; block_idx++) {
                struct object_block *block;
                block = &object_block_pool[block_idx];

                block->active = true;
                block->id = OBJECT_BLOCK_ID;
                block->on_init = on_block_init;
                block->rigid_body = NULL;
                block->colliders[0] = &block->private_data.m_collider;
                block->on_update = on_block_update;
                block->on_draw = on_block_draw;
                block->on_destroy = NULL;
                block->on_collision = on_block_collision;
                block->on_trigger = NULL;

                OBJECT_CALL_EVENT(block, init);

                physics_object_add((struct object *)block);
        }

        THIS_CALL_PRIVATE_MEMBER(object_world, update_map, 0);
        THIS_CALL_PRIVATE_MEMBER(object_world, update_colliders);

        /* Set up NBG1 */
        struct scrn_cell_format *nbg1_format; {
                nbg1_format = &THIS_PRIVATE_DATA(object_world, nbg1_format);

                nbg1_format->scf_scroll_screen = SCRN_NBG1;
                nbg1_format->scf_cc_count = SCRN_CCC_PALETTE_256;
                nbg1_format->scf_character_size = 2 * 2;
                nbg1_format->scf_pnd_size = 1; /* 1-word */
                nbg1_format->scf_auxiliary_mode = 0;
                nbg1_format->scf_cp_table = VRAM_ADDR_4MBIT(0, 0x04000);
                nbg1_format->scf_color_palette = CRAM_MODE_1_OFFSET(1, 0, 0);
                nbg1_format->scf_plane_size = 1 * 1;
                nbg1_format->scf_map.plane_a = VRAM_ADDR_4MBIT(0, 0x00000);
                nbg1_format->scf_map.plane_b = VRAM_ADDR_4MBIT(0, 0x00800);
                nbg1_format->scf_map.plane_c = VRAM_ADDR_4MBIT(0, 0x01000);
                nbg1_format->scf_map.plane_d = VRAM_ADDR_4MBIT(0, 0x01800);
        }

        /* Set up NBG2 */
        struct scrn_cell_format *nbg2_format; {
                nbg2_format = &THIS_PRIVATE_DATA(object_world, nbg2_format);

                nbg2_format->scf_scroll_screen = SCRN_NBG2;
                nbg2_format->scf_cc_count = SCRN_CCC_PALETTE_256;
                nbg2_format->scf_character_size = 1 * 1;
                nbg2_format->scf_pnd_size = 1; /* 1 word */
                nbg2_format->scf_auxiliary_mode = 1;
                nbg2_format->scf_cp_table = VRAM_ADDR_4MBIT(0, 0x04000);
                nbg2_format->scf_color_palette = CRAM_MODE_1_OFFSET(1, 0, 0);
                nbg2_format->scf_plane_size = 1 * 1;
                nbg2_format->scf_map.plane_a = VRAM_ADDR_4MBIT(0, 0x02000);
                nbg2_format->scf_map.plane_b = VRAM_ADDR_4MBIT(0, 0x02000);
                nbg2_format->scf_map.plane_c = VRAM_ADDR_4MBIT(0, 0x02000);
                nbg2_format->scf_map.plane_d = VRAM_ADDR_4MBIT(0, 0x02000);
        }

        struct vram_ctl *vram_ctl;
        vram_ctl = vdp2_vram_control_get();

        vram_ctl->vram_cycp.pt[0].t7 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[0].t6 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[0].t5 = VRAM_CTL_CYCP_CHPNDR_NBG2;
        vram_ctl->vram_cycp.pt[0].t4 = VRAM_CTL_CYCP_CHPNDR_NBG2;
        vram_ctl->vram_cycp.pt[0].t3 = VRAM_CTL_CYCP_PNDR_NBG1;
        vram_ctl->vram_cycp.pt[0].t2 = VRAM_CTL_CYCP_PNDR_NBG2;
        vram_ctl->vram_cycp.pt[0].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        vram_ctl->vram_cycp.pt[1].t7 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t6 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t5 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t4 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        /* We want to be in VBLANK-IN (retrace) */
        fs_texture_vdp2_load("/WORLD.TGA",
            (void *)nbg1_format->scf_cp_table,
            (uint16_t *)nbg1_format->scf_color_palette);

        vdp2_vram_control_set(vram_ctl);

        vdp2_scrn_cell_format_set(nbg1_format);
        vdp2_priority_spn_set(SCRN_NBG1, 2);
        vdp2_scrn_display_set(SCRN_NBG1, /* transparent = */ true);

        vdp2_scrn_cell_format_set(nbg2_format);
        vdp2_priority_spn_set(SCRN_NBG2, 1);
        vdp2_scrn_display_set(SCRN_NBG2, /* transparent = */ true);
}

static void
on_world_update(struct object *this)
{
        THIS_PRIVATE_DATA(object_world, last_scroll_x) =
            THIS_PRIVATE_DATA(object_world, scroll_x);
        THIS_PRIVATE_DATA(object_world, scroll_x) =
            OBJECT(&object_camera, transform).pos_int.x;

        uint16_t delta;
        delta = THIS_PRIVATE_DATA(object_world, scroll_x) -
            THIS_PRIVATE_DATA(object_world, last_scroll_x);

        THIS_PRIVATE_DATA(object_world, move) += delta;
        if (THIS_PRIVATE_DATA(object_world, move) >= WORLD_BLOCK_WIDTH) {
                THIS_PRIVATE_DATA(object_world, move) = 0;
                THIS_PRIVATE_DATA(object_world, column)++;
        }

        THIS_CALL_PRIVATE_MEMBER(object_world, update_map,
            THIS_PRIVATE_DATA(object_world, column));
        THIS_CALL_PRIVATE_MEMBER(object_world, update_colliders);

        uint32_t block_idx;
        for (block_idx = 0; block_idx < OBJECT_BLOCK_COUNT; block_idx++) {
                struct object_block *block;
                block = &object_block_pool[block_idx];

                if (block->active) {
                        OBJECT_CALL_EVENT(block, update);
                }
        }
}

static void
on_world_draw(struct object *this)
{
        struct scrn_cell_format *nbg1_format;
        nbg1_format = &THIS_PRIVATE_DATA(object_world, nbg1_format);

        uint32_t nbg1_page_width;
        nbg1_page_width = SCRN_CALCULATE_PAGE_WIDTH(nbg1_format);
        uint32_t nbg1_page_size;
        nbg1_page_size = SCRN_CALCULATE_PAGE_SIZE(nbg1_format);
        uint16_t *nbg1_plane_pages[4];
        nbg1_plane_pages[0] = (uint16_t *)nbg1_format->scf_map.plane_a;
        nbg1_plane_pages[1] = (uint16_t *)(nbg1_format->scf_map.plane_b + nbg1_page_size);
        nbg1_plane_pages[2] = (uint16_t *)(nbg1_format->scf_map.plane_c + (2 * nbg1_page_size));
        nbg1_plane_pages[3] = (uint16_t *)(nbg1_format->scf_map.plane_d + (3 * nbg1_page_size));
        uint8_t *nbg1_cp_table;
        nbg1_cp_table = (uint8_t *)THIS_PRIVATE_DATA(object_world, nbg1_format).scf_cp_table;
        uint16_t *nbg1_color_palette;
        nbg1_color_palette = (uint16_t *)THIS_PRIVATE_DATA(object_world, nbg1_format).scf_color_palette;

        vdp2_scrn_scv_x_set(SCRN_NBG1, THIS_PRIVATE_DATA(object_world, move),
            0);

        uint32_t column_idx;
        for (column_idx = 0; column_idx < (WORLD_COLUMNS + 1); column_idx++) {
                struct world_column *world_column;
                world_column = &THIS_PRIVATE_DATA(object_world, columns)[column_idx];

                uint32_t row_idx;
                for (row_idx = 0; row_idx < WORLD_ROWS; row_idx++) {
                        uint16_t pnd;
                        pnd = (VDP2_PN_CONFIG_2_CHARACTER_NUMBER((uint32_t)nbg1_cp_table) |
                            VDP2_PN_CONFIG_1_PALETTE_NUMBER((uint32_t)nbg1_color_palette)) |
                            (world_column->row[row_idx].block * 2);

                        nbg1_plane_pages[0][column_idx + (row_idx * nbg1_page_width)] = pnd;
                }
        }

        struct scrn_cell_format *nbg2_format;
        nbg2_format = &THIS_PRIVATE_DATA(object_world, nbg2_format);

        uint32_t nbg2_page_width;
        nbg2_page_width = SCRN_CALCULATE_PAGE_WIDTH(nbg2_format);
        uint32_t nbg2_page_size;
        nbg2_page_size = SCRN_CALCULATE_PAGE_SIZE(nbg2_format);
        uint16_t *nbg2_plane_pages[4];
        nbg2_plane_pages[0] = (uint16_t *)nbg2_format->scf_map.plane_a;
        nbg2_plane_pages[1] = (uint16_t *)(nbg2_format->scf_map.plane_b + nbg2_page_size);
        nbg2_plane_pages[2] = (uint16_t *)(nbg2_format->scf_map.plane_c + (2 * nbg2_page_size));
        nbg2_plane_pages[3] = (uint16_t *)(nbg2_format->scf_map.plane_d + (3 * nbg2_page_size));
        uint8_t *nbg2_cp_table;
        nbg2_cp_table = (uint8_t *)THIS_PRIVATE_DATA(object_world, nbg2_format).scf_cp_table;
        uint16_t *nbg2_color_palette;
        nbg2_color_palette = (uint16_t *)THIS_PRIVATE_DATA(object_world, nbg2_format).scf_color_palette;

        uint32_t x;
        for (x = 0; x < (SCREEN_WIDTH / 8); x++) {
                uint32_t y;
                for (y = 0; y < (SCREEN_HEIGHT / 8); y++) {
                        uint16_t pnd;
                        pnd = (VDP2_PN_CONFIG_1_CHARACTER_NUMBER((uint32_t)nbg2_cp_table) |
                            VDP2_PN_CONFIG_1_PALETTE_NUMBER((uint32_t)nbg2_color_palette));
                        nbg2_plane_pages[0][x + (y * nbg2_page_width)] = pnd | ((3 * 4) * 2);
                }
        }
}

static void
m_update_colliders(struct object *this)
{
        struct bounding_box bb;
        bb.point.a.x = (-(BLOCKS / 2) * WORLD_BLOCK_WIDTH) + 1;
        bb.point.a.y = ((BLOCKS - 1) * WORLD_BLOCK_HEIGHT) - 1;

        bb.point.b.x = ((BLOCKS - 1) * WORLD_BLOCK_WIDTH) - 1;
        bb.point.b.y = ((BLOCKS - 1) * WORLD_BLOCK_HEIGHT) - 1;

        bb.point.c.x = ((BLOCKS - 1) * WORLD_BLOCK_WIDTH) - 1;
        bb.point.c.y = (-(BLOCKS / 2) * WORLD_BLOCK_WIDTH) + 1;

        bb.point.d.x = (-(BLOCKS / 2) * WORLD_BLOCK_WIDTH) + 1;
        bb.point.d.y = (-(BLOCKS / 2) * WORLD_BLOCK_WIDTH) + 1;

        int16_vector2_t block_points[4];

        uint32_t point_idx;
        for (point_idx = 0; point_idx < 4; point_idx++) {
                int16_t bb_x;
                bb_x = bb.points[point_idx].x;
                int16_t bb_y;
                bb_y = bb.points[point_idx].y;

                int16_t column;
                column = ((OBJECT(&object_player, transform).pos_int.x + bb_x) /
                    WORLD_BLOCK_WIDTH);
                int16_t row;
                row = (WORLD_ROWS - 1) -
                    ((OBJECT(&object_player, transform).pos_int.y + bb_y) /
                        WORLD_BLOCK_HEIGHT);

                block_points[point_idx].x = clamp(column, 0, WORLD_COLUMNS - 1);
                block_points[point_idx].y = clamp(row, 0, WORLD_ROWS - 1);
        }

        uint32_t columns;
        columns = (block_points[1].x - block_points[0].x) + 1;
        uint32_t rows;
        rows = (block_points[3].y - block_points[0].y) + 1;

        /* Make sure we don't cause a buffer overflow */
        assert(OBJECT_BLOCK_COUNT >= (columns * rows));

        int16_t column_initial;
        column_initial = block_points[3].x;
        int16_t row_initial;
        row_initial = block_points[0].y;

        uint32_t block_idx;
        block_idx = 0;

        uint32_t column_idx;
        for (column_idx = 0; column_idx < columns; column_idx++) {
                uint32_t column_offset;
                column_offset = column_initial + column_idx;

                struct world_column *world_column;
                world_column = &THIS_PRIVATE_DATA(object_world, columns)[column_offset];

                uint32_t row_idx;
                for (row_idx = 0; row_idx < rows; row_idx++) {
                        uint32_t row_offset;
                        row_offset = row_initial + row_idx;

                        struct object_block *block;
                        block = &object_block_pool[block_idx];

                        switch (world_column->row[row_offset].block) {
                        case WORLD_COLUMN_BLOCK_NONE:
                                continue;
                        }

                        block->active = true;

                        block->private_data.m_block_pos.x = column_offset;
                        block->private_data.m_block_pos.y = row_offset;

                        block->private_data.m_block =
                            world_column->row[row_offset].block;

                        block_idx++;
                }
        }

        for (; block_idx < (2 * BLOCKS * BLOCKS); block_idx++) {
                struct object_block *block;
                block = &object_block_pool[block_idx];

                block->active = false;
        }
}

static void
m_update_map(struct object *this, uint32_t cur_column)
{
        fs_seek(THIS_PRIVATE_DATA(object_world, map_fh),
            cur_column * WORLD_MAP_FORMAT_COLUMN_SIZE, SEEK_SET);

        uint32_t column_idx;
        for (column_idx = 0; column_idx < (WORLD_COLUMNS + 1); column_idx++) {
                struct world_column *world_column;
                world_column = &THIS_PRIVATE_DATA(object_world, columns)[column_idx];

                uint8_t row[WORLD_MAP_FORMAT_COLUMN_SIZE];
                fs_read(THIS_PRIVATE_DATA(object_world, map_fh), &row[0],
                    WORLD_MAP_FORMAT_COLUMN_SIZE);

                uint32_t row_idx;
                for (row_idx = 0; row_idx < WORLD_ROWS; row_idx++) {
                        uint16_t tile_number;

                        uint8_t block;
                        block = row[row_idx];

                        switch (block) {
                        case WORLD_COLUMN_BLOCK_NONE:
                                tile_number = 0;
                                break;
                        case WORLD_COLUMN_BLOCK_SAFE:
                        case WORLD_COLUMN_BLOCK_UNSAFE:
                                tile_number = block;
                                break;
                        default:
                                assert(false && "Invalid block");
                        }

                        world_column->row[row_idx].block = tile_number;
                }
        }
}

static void
on_block_init(struct object *this)
{
        THIS_PRIVATE_DATA(object_block, block) = WORLD_COLUMN_BLOCK_NONE;
        int16_vector2_zero(&THIS_PRIVATE_DATA(object_block, block_pos));

        THIS(object_block, transform).object = this;
        THIS(object_block, transform).pos_int.x = 0;
        THIS(object_block, transform).pos_int.y = 0;

        collider_init(THIS(object_block, colliders)[0], /* id = */ -1,
            WORLD_BLOCK_WIDTH, WORLD_BLOCK_HEIGHT,
            /* trigger = */ false, /* fixed = */ true);
}

static void
on_block_update(struct object *this)
{
        uint32_t column;
        column = THIS_PRIVATE_DATA(object_block, block_pos).x;
        uint32_t row;
        row = THIS_PRIVATE_DATA(object_block, block_pos).y;

        struct world_column *world_column;
        world_column = &OBJECT_PRIVATE_DATA(&object_world, columns)[column];

        switch (THIS_PRIVATE_DATA(object_block, block)) {
        case WORLD_COLUMN_BLOCK_SAFE:
        case WORLD_COLUMN_BLOCK_UNSAFE:
                world_column->row[row].block = WORLD_COLUMN_BLOCK_COLLIDER;

                THIS(object_block, colliders)[0]->id = 0;
                break;
        }

        /* XXX: This is not truly in world space */
        THIS(object_block, transform).pos_int.x = column * WORLD_BLOCK_WIDTH;
        THIS(object_block, transform).pos_int.y = (WORLD_ROWS - row - 1) * WORLD_BLOCK_HEIGHT;
}

static void
on_block_draw(struct object *this __unused)
{
}

static void
on_block_collision(struct object *this __unused, struct object *other,
    const struct collider_info *info __unused)
{
        if (other->id == OBJECT_BLOCK_ID) {
                return;
        }

        (void)sprintf(text, "block, other->id=%i\n", (int)other->id);
        cons_buffer(text);
}

static void
m_start(struct object *this)
{
        /* World */
        THIS(object_world, transform).pos_int.x = 0;
        THIS(object_world, transform).pos_int.y = 0;

        /* Player */
        OBJECT(&object_player, transform).pos_int.x = 5 * WORLD_BLOCK_WIDTH;
        OBJECT(&object_player, transform).pos_int.y = ((WORLD_ROWS) - 11) * WORLD_BLOCK_HEIGHT;

        fs_seek(THIS_PRIVATE_DATA(object_world, map_fh), 0, SEEK_SET);

        THIS_PRIVATE_DATA(object_world, scroll_x) = 0;
        THIS_PRIVATE_DATA(object_world, last_scroll_x) = 0;
        THIS_PRIVATE_DATA(object_world, move) = 0;
        THIS_PRIVATE_DATA(object_world, column) = 0;
}
