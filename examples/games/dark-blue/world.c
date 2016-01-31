/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#include "fs.h"
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

static void _update_world_map(struct object *, uint32_t);
static void _update_world_colliders(struct object *, uint32_t);

static uint16_t scroll_x = 0;
static uint16_t last_scroll_x = 0;
static uint16_t move = 0;
static uint16_t column = 0;

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
        .on_trigger = NULL
};

struct object_block object_block_pool[OBJECT_BLOCK_COUNT];

static void
on_world_init(struct object *this)
{
        struct object_world *world __unused;
        world = (struct object_world *)this;

        world->private_data.m_map_fh = fs_open("/TEST.MAP");
        assert(world->private_data.m_map_fh != NULL);

        /* World */
        world->transform.position.x = 0;
        world->transform.position.y = 0;

        /* Player */
        object_player.transform.position.x = 5 * WORLD_BLOCK_WIDTH;
        object_player.transform.position.y = ((WORLD_ROWS) - 11) * WORLD_BLOCK_HEIGHT;

        world->private_data.m_column_count =
            fs_size(world->private_data.m_map_fh) / WORLD_MAP_FORMAT_COLUMN_SIZE;
        world->private_data.m_row_count = WORLD_ROWS;

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

        _update_world_map(this, 0);
        _update_world_colliders(this, 0);

        /* VRAM A1 */
        world->private_data.m_nbg1_planes[0] =
            (uint16_t *)VRAM_ADDR_4MBIT(1, 0x01000);
        /* VRAM A1 */
        world->private_data.m_nbg1_planes[1] =
            (uint16_t *)VRAM_ADDR_4MBIT(1, 0x02000);
        /* VRAM A1 */
        world->private_data.m_nbg1_planes[2] =
            (uint16_t *)VRAM_ADDR_4MBIT(1, 0x03000);
        /* VRAM A1 */
        world->private_data.m_nbg1_planes[3] =
            (uint16_t *)VRAM_ADDR_4MBIT(1, 0x04000);
        /* VRAM A1 */
        world->private_data.m_nbg2_planes[0] =
            (uint16_t *)VRAM_ADDR_4MBIT(1, 0x05000);
        /* VRAM A1 */
        world->private_data.m_nbg2_planes[1] =
            (uint16_t *)VRAM_ADDR_4MBIT(1, 0x06000);
        /* VRAM A1 */
        world->private_data.m_nbg2_planes[2] =
            (uint16_t *)VRAM_ADDR_4MBIT(1, 0x07000);
        /* VRAM A1 */
        world->private_data.m_nbg2_planes[3] =
            (uint16_t *)VRAM_ADDR_4MBIT(1, 0x08000);
        /* VRAM A0 */
        world->private_data.m_nbg1_character =
            (uint8_t *)VRAM_ADDR_4MBIT(0, 0x00000);
        /* VRAM A0 */
        world->private_data.m_nbg2_character =
            (uint8_t *)VRAM_ADDR_4MBIT(0, 0x01000);
        /* CRAM */
        world->private_data.m_nbg1_color_palette =
            (uint16_t *)CRAM_MODE_1_OFFSET(1, 0, 0);
        world->private_data.m_nbg2_color_palette =
            (uint16_t *)CRAM_MODE_1_OFFSET(0, 1, 0);

        struct scrn_cell_format *nbg1_format;
        nbg1_format = &world->private_data.m_nbg1_format;
        uint16_t **nbg1_planes;
        nbg1_planes = world->private_data.m_nbg1_planes;
        uint8_t *nbg1_character;
        nbg1_character = world->private_data.m_nbg1_character;
        uint16_t *nbg1_color_palette;
        nbg1_color_palette = world->private_data.m_nbg1_color_palette;

        nbg1_format->scf_scroll_screen = SCRN_NBG1;
        nbg1_format->scf_cc_count = SCRN_CCC_PALETTE_256;
        nbg1_format->scf_character_size = 2 * 2;
        nbg1_format->scf_pnd_size = 1; /* 1-word */
        nbg1_format->scf_auxiliary_mode = 0;
        nbg1_format->scf_cp_table = (uint32_t)nbg1_character;
        nbg1_format->scf_color_palette = (uint32_t)nbg1_color_palette;
        nbg1_format->scf_plane_size = 1 * 1;
        nbg1_format->scf_map.plane_a = (uint32_t)nbg1_planes[0];
        nbg1_format->scf_map.plane_b = (uint32_t)nbg1_planes[1];
        nbg1_format->scf_map.plane_c = (uint32_t)nbg1_planes[2];
        nbg1_format->scf_map.plane_d = (uint32_t)nbg1_planes[3];

        struct scrn_cell_format *nbg2_format;
        nbg2_format = &world->private_data.m_nbg2_format;
        uint16_t **nbg2_planes;
        nbg2_planes = world->private_data.m_nbg2_planes;
        uint8_t *nbg2_character;
        nbg2_character = world->private_data.m_nbg2_character;
        uint16_t *nbg2_color_palette;
        nbg2_color_palette = world->private_data.m_nbg2_color_palette;

        nbg2_format->scf_scroll_screen = SCRN_NBG2;
        nbg2_format->scf_cc_count = SCRN_CCC_PALETTE_16;
        nbg2_format->scf_character_size = 1 * 1;
        nbg2_format->scf_pnd_size = 1; /* 1 word */
        nbg2_format->scf_auxiliary_mode = 1;
        nbg2_format->scf_cp_table = (uint32_t)nbg2_character;
        nbg2_format->scf_color_palette = (uint32_t)nbg2_color_palette;
        nbg2_format->scf_plane_size = 1 * 1;
        nbg2_format->scf_map.plane_a = (uint32_t)nbg2_planes[0];
        nbg2_format->scf_map.plane_b = (uint32_t)nbg2_planes[1];
        nbg2_format->scf_map.plane_c = (uint32_t)nbg2_planes[2];
        nbg2_format->scf_map.plane_d = (uint32_t)nbg2_planes[3];

        struct vram_ctl *vram_ctl;
        vram_ctl = vdp2_vram_control_get();

        vram_ctl->vram_cycp.pt[0].t7 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[0].t6 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[0].t5 = VRAM_CTL_CYCP_CHPNDR_NBG2;
        vram_ctl->vram_cycp.pt[0].t4 = VRAM_CTL_CYCP_CHPNDR_NBG2;
        vram_ctl->vram_cycp.pt[0].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        vram_ctl->vram_cycp.pt[1].t7 = VRAM_CTL_CYCP_PNDR_NBG1;
        vram_ctl->vram_cycp.pt[1].t6 = VRAM_CTL_CYCP_PNDR_NBG1;
        vram_ctl->vram_cycp.pt[1].t5 = VRAM_CTL_CYCP_PNDR_NBG2;
        vram_ctl->vram_cycp.pt[1].t4 = VRAM_CTL_CYCP_PNDR_NBG2;
        vram_ctl->vram_cycp.pt[1].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear(); {
                void *file_handle;
                file_handle = fs_open("/WORLD.TGA");

                size_t file_size;
                file_size = romdisk_total(file_handle);

                uint8_t *ptr;
                ptr = (uint8_t *)0x00201000;

                fs_read(file_handle, ptr, file_size);
                fs_close(file_handle);

                tga_t tga;
                int status;
                status = tga_read(&tga, ptr);
                assert(status == TGA_FILE_OK);
                uint32_t amount;
                amount = tga_image_decode_tiled(&tga, (void *)nbg1_character);
                assert(amount > 0);
                amount = tga_cmap_decode(&tga, nbg1_color_palette);
                assert(amount > 0);

                vdp2_vram_control_set(vram_ctl);

                vdp2_scrn_cell_format_set(nbg1_format);
                vdp2_priority_spn_set(SCRN_NBG1, 2);
                vdp2_scrn_display_set(SCRN_NBG1, /* transparent = */ true);

                vdp2_scrn_cell_format_set(nbg2_format);
                vdp2_priority_spn_set(SCRN_NBG2, 1);
                vdp2_scrn_display_set(SCRN_NBG2, /* transparent = */ true);
        } vdp2_tvmd_display_set();
}

static void
on_world_update(struct object *this)
{
        struct object_world *world __unused;
        world = (struct object_world *)this;

        last_scroll_x = scroll_x;
        scroll_x = object_camera.transform.position.x;

        move += scroll_x - last_scroll_x;
        if (move >= WORLD_BLOCK_WIDTH) {
                move = 0;
                column++;
        }

        _update_world_map(this, column);
        _update_world_colliders(this, column);

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
        struct object_world *world __unused;
        world = (struct object_world *)this;

        uint16_t **nbg1_planes;
        nbg1_planes = world->private_data.m_nbg1_planes;
        uint8_t *nbg1_character;
        nbg1_character = world->private_data.m_nbg1_character;
        uint16_t *nbg1_color_palette;
        nbg1_color_palette = world->private_data.m_nbg1_color_palette;

        vdp2_scrn_scv_x_set(SCRN_NBG1, move, 0);

        uint32_t column_idx;
        for (column_idx = 0; column_idx < (WORLD_COLUMNS + 1); column_idx++) {
                struct world_column *world_column;
                world_column = &world->private_data.m_columns[column_idx];
                uint32_t row_idx;
                for (row_idx = 0; row_idx < WORLD_ROWS; row_idx++) {
                        uint16_t character_number;
                        character_number =
                            (VDP2_PN_CONFIG_2_CHARACTER_NUMBER((uint32_t)nbg1_character) |
                                VDP2_PN_CONFIG_1_PALETTE_NUMBER((uint32_t)nbg1_color_palette)) |
                            (world_column->row[row_idx].block * 2);

                        uint32_t map_idx;
                        map_idx = column_idx + (row_idx << 5);

                        nbg1_planes[0][map_idx] = character_number;
                        nbg1_planes[1][map_idx] = character_number;
                        nbg1_planes[2][map_idx] = character_number;
                        nbg1_planes[3][map_idx] = character_number;
                }
        }
}

static void
_update_world_colliders(struct object *this, uint32_t column __unused)
{
        struct object_world *world __unused;
        world = (struct object_world *)this;

        /* Take the position of the player */
        struct object_player *player;
        player = (struct object_player *)&object_player;

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
                column = ((player->transform.position.x + bb_x) /
                    WORLD_BLOCK_WIDTH);
                int16_t row;
                row = (WORLD_ROWS - 1) -
                    ((player->transform.position.y + bb_y) /
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

        int16_t column_initial __unused;
        column_initial = block_points[3].x;
        int16_t row_initial __unused;
        row_initial = block_points[0].y;

        uint32_t block_idx;
        block_idx = 0;

        uint32_t column_idx;
        for (column_idx = 0; column_idx < columns; column_idx++) {
                uint32_t column_offset;
                column_offset = column_initial + column_idx;

                struct world_column *world_column;
                world_column = &world->private_data.m_columns[column_offset];

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
_update_world_map(struct object *this, uint32_t column)
{
        struct object_world *world __unused;
        world = (struct object_world *)this;

        fs_seek(world->private_data.m_map_fh,
            column * WORLD_MAP_FORMAT_COLUMN_SIZE, SEEK_SET);

        uint32_t column_idx;
        for (column_idx = 0; column_idx < (WORLD_COLUMNS + 1); column_idx++) {
                struct world_column *world_column;
                world_column = &world->private_data.m_columns[column_idx];

                uint8_t row[WORLD_MAP_FORMAT_COLUMN_SIZE];
                fs_read(world->private_data.m_map_fh, &row[0],
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
        struct object_block *block;
        block = (struct object_block *)this;

        block->private_data.m_block = WORLD_COLUMN_BLOCK_NONE;
        int16_vector2_zero(&block->private_data.m_block_pos);

        block->transform.object = (struct object *)block;
        block->transform.position.x = 0;
        block->transform.position.y = 0;

        collider_init(block->colliders[0], /* id = */ -1,
            WORLD_BLOCK_WIDTH, WORLD_BLOCK_HEIGHT,
            /* trigger = */ false, /* fixed = */ true);
}

static void
on_block_update(struct object *this)
{
        struct object_block *block __unused;
        block = (struct object_block *)this;

        struct object_world *world;
        world = (struct object_world *)&object_world;

        uint32_t column;
        column = block->private_data.m_block_pos.x;
        uint32_t row;
        row = block->private_data.m_block_pos.y;

        struct world_column *world_column;
        world_column = &world->private_data.m_columns[column];

        switch (block->private_data.m_block) {
        case WORLD_COLUMN_BLOCK_SAFE:
        case WORLD_COLUMN_BLOCK_UNSAFE:
                world_column->row[row].block = WORLD_COLUMN_BLOCK_COLLIDER;

                block->colliders[0]->id = 0;
                break;
        }

        /* XXX: This is not truly in world space */
        block->transform.position.x = column * WORLD_BLOCK_WIDTH;
        block->transform.position.y = (WORLD_ROWS - row - 1) * WORLD_BLOCK_HEIGHT;
}

static void
on_block_draw(struct object *this __unused)
{
}

static void
on_block_collision(struct object *this, struct object *other,
    const struct collider_info *info __unused)
{
        struct object_block *block __unused;
        block = (struct object_block *)this;

        if (other->id == OBJECT_BLOCK_ID) {
                return;
        }

        (void)sprintf(text, "block, other->id=%i\n", (int)other->id);
        cons_buffer(text);
}
