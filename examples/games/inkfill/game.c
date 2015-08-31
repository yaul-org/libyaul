/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>
#include <tga.h>

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "fs.h"
#include "game.h"
#include "globals.h"
#include "scene.h"

#define TRIES_MAX 18

#define GRID_CELL_COLOR_RED     0
#define GRID_CELL_COLOR_ORANGE  1
#define GRID_CELL_COLOR_GREEN   2
#define GRID_CELL_COLOR_BLUE    3

#define GRID_WIDTH      18
#define GRID_HEIGHT     13

static bool _changed = false;
static uint32_t _tries = 0;

static uint16_t *_nbg1_planes[4] = {
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x01000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x02000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x03000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x04000)
};
/* CRAM */
static uint16_t *_nbg1_color_palette = (uint16_t *)CRAM_MODE_1_OFFSET(1, 1, 0);
/* VRAM B0 */
static uint32_t *_nbg1_character = (uint32_t *)VRAM_ADDR_4MBIT(2, 0x00000);

static uint16_t *_nbg3_planes[4] = {
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x08000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x08000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x08000),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x08000)
};
/* CRAM */
static uint32_t *_nbg3_color_palette = (uint32_t *)CRAM_MODE_1_OFFSET(1, 0, 0);
/* VRAM B0 */
static uint32_t *_nbg3_character = (uint32_t *)VRAM_ADDR_4MBIT(2, 0x06000);

static uint8_t _grid[GRID_WIDTH * GRID_HEIGHT];

static void _grid_init(void);
static void _grid_update(void);
static void _grid_draw(void);
static void _grid_exit(void);
static bool _grid_complete_check(void);
static uint8_t _grid_color_get(uint32_t, uint32_t);
static void _grid_color_set(uint32_t, uint32_t, uint32_t);
static uint32_t _generate_random_xor128(void);
static void _grid_randomize(void);
static void _grid_flood_fill(uint8_t);
static void _grid_flood_fill_helper(int32_t, int32_t, uint8_t, uint8_t);

void
game_init(void)
{
        static uint16_t back_screen_color[] = {
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(84, 164, 255)
        };

        vdp2_scrn_back_screen_set(/* single_color = */ true,
            VRAM_ADDR_4MBIT(3, 0x1FFFE), back_screen_color, 1);

        _grid_init();

        _changed = false;
        _tries = 0;
}

void
game_update(void)
{
        if (_changed) {
                return;
        }

        if ((_tries >= TRIES_MAX) && !(_grid_complete_check())) {
                sleep(1);
                _changed = true;
                scene_load("game-over");
        }

        if (_grid_complete_check()) {
                _tries = 0;
                _grid_randomize();
        }

        if (g_digital.held.button.start) {
                _tries = 0;
                _grid_randomize();
        }

        _grid_update();
}

void
game_draw(void)
{
        _grid_draw();
}

void
game_exit(void)
{
        _grid_exit();
}

static void
_grid_init(void)
{
        static uint32_t tiles[] = {
                0x00000000,
                0x11111111,
                0x22222222,
                0x33333333,
                0x44444444,
                0x55555555
        };

        static uint16_t palette[] = {
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00), /* Red */
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0xFF, 0x00, 0x00), /* Red */
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0xFF, 0xA5, 0x00), /* Orange */
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0xFF, 0x00), /* Green */
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0xFF), /* Blue */
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00),
                COLOR_RGB_DATA | COLOR_RGB888_TO_RGB555(0x00, 0x00, 0x00)
        };

        struct scrn_cell_format nbg1_format;
        struct vram_ctl *vram_ctl;

        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear();

        nbg1_format.scf_scroll_screen = SCRN_NBG1;
        nbg1_format.scf_cc_count = SCRN_CCC_PALETTE_16;
        nbg1_format.scf_character_size = 2 * 2;
        nbg1_format.scf_pnd_size = 1; /* 1 word */
        nbg1_format.scf_auxiliary_mode = 1;
        nbg1_format.scf_cp_table = (uint32_t)_nbg1_character;
        nbg1_format.scf_color_palette = (uint32_t)_nbg1_color_palette;
        nbg1_format.scf_plane_size = 2 * 1;
        nbg1_format.scf_map.plane_a = (uint32_t)_nbg1_planes[0];
        nbg1_format.scf_map.plane_b = (uint32_t)_nbg1_planes[1];
        nbg1_format.scf_map.plane_c = (uint32_t)_nbg1_planes[2];
        nbg1_format.scf_map.plane_d = (uint32_t)_nbg1_planes[3];

        vdp2_scrn_cell_format_set(&nbg1_format);
        vdp2_priority_spn_set(SCRN_NBG1, 6);

        vdp2_scrn_scv_x_set(SCRN_NBG1, -24, 0);
        vdp2_scrn_scv_y_set(SCRN_NBG1, -8, 0);

        /* Draw the tiles */
        uint32_t tile_idx;

        for (tile_idx = 0; tile_idx < 6; tile_idx++) {
                uint32_t row;

                row = tiles[tile_idx];

                _nbg1_character[0x00 + (tile_idx << 5)] = row;
                _nbg1_character[0x01 + (tile_idx << 5)] = row;
                _nbg1_character[0x02 + (tile_idx << 5)] = row;
                _nbg1_character[0x03 + (tile_idx << 5)] = row;
                _nbg1_character[0x04 + (tile_idx << 5)] = row;
                _nbg1_character[0x05 + (tile_idx << 5)] = row;
                _nbg1_character[0x06 + (tile_idx << 5)] = row;
                _nbg1_character[0x07 + (tile_idx << 5)] = row;

                _nbg1_character[0x08 + (tile_idx << 5)] = row;
                _nbg1_character[0x09 + (tile_idx << 5)] = row;
                _nbg1_character[0x0A + (tile_idx << 5)] = row;
                _nbg1_character[0x0B + (tile_idx << 5)] = row;
                _nbg1_character[0x0C + (tile_idx << 5)] = row;
                _nbg1_character[0x0D + (tile_idx << 5)] = row;
                _nbg1_character[0x0E + (tile_idx << 5)] = row;
                _nbg1_character[0x0F + (tile_idx << 5)] = row;

                _nbg1_character[0x10 + (tile_idx << 5)] = row;
                _nbg1_character[0x11 + (tile_idx << 5)] = row;
                _nbg1_character[0x12 + (tile_idx << 5)] = row;
                _nbg1_character[0x13 + (tile_idx << 5)] = row;
                _nbg1_character[0x14 + (tile_idx << 5)] = row;
                _nbg1_character[0x15 + (tile_idx << 5)] = row;
                _nbg1_character[0x16 + (tile_idx << 5)] = row;
                _nbg1_character[0x17 + (tile_idx << 5)] = row;

                _nbg1_character[0x18 + (tile_idx << 5)] = row;
                _nbg1_character[0x19 + (tile_idx << 5)] = row;
                _nbg1_character[0x1A + (tile_idx << 5)] = row;
                _nbg1_character[0x1B + (tile_idx << 5)] = row;
                _nbg1_character[0x1C + (tile_idx << 5)] = row;
                _nbg1_character[0x1D + (tile_idx << 5)] = row;
                _nbg1_character[0x1E + (tile_idx << 5)] = row;
                _nbg1_character[0x1F + (tile_idx << 5)] = row;
        }

        /* Clear the entire map */
        uint16_t x;
        uint16_t y;

        for (y = 0; y < 32; y++) {
                for (x = 0; x < 64; x++) {
                        uint16_t character_number;

                        character_number =
                            VDP2_PN_CONFIG_3_CHARACTER_NUMBER((uint32_t)_nbg1_character) |
                            VDP2_PN_CONFIG_0_PALETTE_NUMBER((uint32_t)_nbg1_color_palette);
                        _nbg1_planes[0][x + (y << 6)] = character_number;
                        _nbg1_planes[1][x + (y << 6)] = character_number;
                        _nbg1_planes[2][x + (y << 6)] = character_number;
                        _nbg1_planes[3][x + (y << 6)] = character_number;
                }
        }

        /* Copy palette */
        memcpy(_nbg1_color_palette, palette, 16 * sizeof(uint16_t));

        _grid_randomize();

        struct scrn_cell_format nbg3_format;

        nbg3_format.scf_scroll_screen = SCRN_NBG3;
        nbg3_format.scf_cc_count = SCRN_CCC_PALETTE_16;
        nbg3_format.scf_character_size = 1 * 1;
        nbg3_format.scf_pnd_size = 1; /* 1 word */
        nbg3_format.scf_auxiliary_mode = 1;
        nbg3_format.scf_cp_table = (uint32_t)_nbg3_character;
        nbg3_format.scf_color_palette = (uint32_t)_nbg3_color_palette;
        nbg3_format.scf_plane_size = 1 * 1;
        nbg3_format.scf_map.plane_a = (uint32_t)_nbg3_planes[0];
        nbg3_format.scf_map.plane_b = (uint32_t)_nbg3_planes[1];
        nbg3_format.scf_map.plane_c = (uint32_t)_nbg3_planes[2];
        nbg3_format.scf_map.plane_d = (uint32_t)_nbg3_planes[3];

        vdp2_scrn_cell_format_set(&nbg3_format);
        vdp2_priority_spn_set(SCRN_NBG3, 7);

        for (tile_idx = 0; tile_idx < 6; tile_idx++) {
                uint32_t row;

                row = tiles[tile_idx];

                _nbg3_character[0x00 + (tile_idx << 3)] = row;
                _nbg3_character[0x01 + (tile_idx << 3)] = row;
                _nbg3_character[0x02 + (tile_idx << 3)] = row;
                _nbg3_character[0x03 + (tile_idx << 3)] = row;
                _nbg3_character[0x04 + (tile_idx << 3)] = row;
                _nbg3_character[0x05 + (tile_idx << 3)] = row;
                _nbg3_character[0x06 + (tile_idx << 3)] = row;
                _nbg3_character[0x07 + (tile_idx << 3)] = row;
        }

        /* Clear the entire map */
        for (y = 0; y < 64; y++) {
                for (x = 0; x < 32; x++) {
                        uint16_t character_number;

                        character_number =
                            VDP2_PN_CONFIG_1_CHARACTER_NUMBER((uint32_t)_nbg3_character);
                        _nbg3_planes[0][x + (y << 5)] = character_number;
                        _nbg3_planes[1][x + (y << 5)] = character_number;
                        _nbg3_planes[2][x + (y << 5)] = character_number;
                        _nbg3_planes[3][x + (y << 5)] = character_number;
                }
        }

        /* Copy palette */
        memcpy(_nbg3_color_palette, palette, 16 * sizeof(uint16_t));

        struct scrn_bitmap_format nbg0_format;

        nbg0_format.sbf_scroll_screen = SCRN_NBG0;
        nbg0_format.sbf_cc_count = SCRN_CCC_RGB_32768;
        nbg0_format.sbf_bitmap_size.width = 512;
        nbg0_format.sbf_bitmap_size.height = 256;
        nbg0_format.sbf_bitmap_pattern = VRAM_ADDR_4MBIT(0, 0x00000);

        vdp2_scrn_bitmap_format_set(&nbg0_format);
        vdp2_priority_spn_set(SCRN_NBG0, 5);

        void *fh;

        fh = fs_open("/GAME.TGA");
        fs_load(fh, (uint8_t *)0x00201000);

        tga_t tga;
        tga.tga_options.msb = false;
        tga.tga_options.transparent_pixel = COLOR_RGB888_TO_RGB555(0, 255, 0);

        int ret;

        uint8_t *ptr;
        ptr = (uint8_t *)0x00201000;

        ret = tga_read(&tga, ptr);
        assert(ret == TGA_FILE_OK);
        uint32_t amount;
        amount = tga_image_decode(&tga, (void *)VRAM_ADDR_4MBIT(0, 0x00000));
        assert(amount > 0);

        vram_ctl = vdp2_vram_control_get();
        vram_ctl->vram_cycp.pt[0].t7 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[0].t6 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[0].t5 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[0].t4 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[0].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[0].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        vram_ctl->vram_cycp.pt[1].t7 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t6 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t5 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t4 = VRAM_CTL_CYCP_CHPNDR_NBG0;
        vram_ctl->vram_cycp.pt[1].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[1].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        vram_ctl->vram_cycp.pt[2].t7 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[2].t6 = VRAM_CTL_CYCP_PNDR_NBG1;
        vram_ctl->vram_cycp.pt[2].t5 = VRAM_CTL_CYCP_CHPNDR_NBG3;
        vram_ctl->vram_cycp.pt[2].t4 = VRAM_CTL_CYCP_PNDR_NBG3;
        vram_ctl->vram_cycp.pt[2].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t0 = VRAM_CTL_CYCP_NO_ACCESS;
        vdp2_vram_control_set(vram_ctl);

        vdp2_scrn_display_set(SCRN_NBG0, /* transparent = */ false);
        vdp2_scrn_display_set(SCRN_NBG1, /* transparent = */ true);
        vdp2_scrn_display_set(SCRN_NBG3, /* transparent = */ true);

        vdp2_tvmd_display_set();
}

static void
_grid_update(void)
{
        if (g_digital.connected == 1) {
                if (g_digital.held.button.a) {
                        _grid_flood_fill(GRID_CELL_COLOR_GREEN);
                        _tries++;
                }  else if (g_digital.held.button.b) {
                        _grid_flood_fill(GRID_CELL_COLOR_BLUE);
                        _tries++;
                } else if (g_digital.held.button.c) {
                        _grid_flood_fill(GRID_CELL_COLOR_ORANGE);
                        _tries++;
                } else if (g_digital.held.button.r) {
                        _grid_flood_fill(GRID_CELL_COLOR_RED);
                        _tries++;
                }
        }
}

static void
_grid_draw(void)
{
        uint16_t character_number;
        uint16_t palette_number;
        uint32_t x;
        uint32_t y;

        /* Draw actual grid */
        character_number = VDP2_PN_CONFIG_3_CHARACTER_NUMBER((uint32_t)_nbg1_character);
        palette_number = VDP2_PN_CONFIG_0_PALETTE_NUMBER((uint32_t)_nbg1_color_palette);
        for (y = 0; y < GRID_HEIGHT; y++) {
                for (x = 0; x < GRID_WIDTH; x++) {
                        uint8_t color;

                        color = _grid_color_get(x, y) + 1;
                        _nbg1_planes[0][x + (y << 5)] =
                            palette_number | (character_number + color);
                }
        }

        /* Draw number of tries */
        character_number = VDP2_PN_CONFIG_1_CHARACTER_NUMBER((uint32_t)_nbg3_character);
        palette_number = VDP2_PN_CONFIG_0_PALETTE_NUMBER((uint32_t)_nbg3_color_palette);
        for (y = 0; y < TRIES_MAX; y++) {
                _nbg3_planes[0][(1) + ((y + 1) << 6)] =
                    palette_number | (character_number + GRID_CELL_COLOR_BLUE + 1);
        }

        for (y = 0; y < _tries; y++) {
                _nbg3_planes[0][(1) + ((y + 1) << 6)] =
                    palette_number | (character_number + GRID_CELL_COLOR_RED + 1);
        }
}

static void
_grid_exit(void)
{
        vdp2_scrn_display_clear();
}

static void
_grid_randomize(void)
{
        uint32_t x;
        uint32_t y;

        for (y = 0; y < GRID_HEIGHT; y++) {
                for (x = 0; x < GRID_WIDTH; x++) {
                        _grid_color_set(x, y, _generate_random_xor128() % 4);
                }
        }
}

static void
_grid_flood_fill(uint8_t color)
{
        _grid_flood_fill_helper(0, 0, _grid_color_get(0, 0), color);
}

static void
_grid_flood_fill_helper(int32_t x, int32_t y, uint8_t old_color, uint8_t new_color)
{
        if ((old_color == new_color) || (_grid_color_get(x, y) != old_color)) {
                return;
        }

        _grid_color_set(x, y, new_color);

        if (x > 0) {
                _grid_flood_fill_helper(x - 1, y, old_color, new_color);
        }

        if (x < (GRID_WIDTH - 1)) {
                _grid_flood_fill_helper(x + 1, y, old_color, new_color);
        }

        if (y > 0) {
                _grid_flood_fill_helper(x, y - 1, old_color, new_color);
        }

        if (y < (GRID_HEIGHT - 1)) {
                _grid_flood_fill_helper(x, y + 1, old_color, new_color);
        }
}

static uint8_t
_grid_color_get(uint32_t x, uint32_t y)
{
        return (_grid[x + (y * GRID_WIDTH)] - 1);
}

static void
_grid_color_set(uint32_t x, uint32_t y, uint32_t color)
{
        _grid[x + (y * GRID_WIDTH)] = color + 1;
}

static bool
_grid_complete_check(void)
{
        int32_t y;
        int32_t x;
        for (y = 0; y < GRID_HEIGHT; y++) {
                for (x = 0; x < GRID_WIDTH; x++) {
                        if (_grid_color_get(0, 0) != _grid_color_get(x, y)) {
                                return false;
                        }
                }
        }

        return true;
}

/**
 * <http://en.wikipedia.org/wiki/Xorshift>
 */
static uint32_t
_generate_random_xor128(void)
{
        static uint32_t x = 123456789;
        static uint32_t y = 362436069;
        static uint32_t z = 521288629;
        static uint32_t w = 88675123;

        uint32_t t;

        t = x ^ (x << 11);
        x = y;
        y = z;
        z = w;
        w = w ^ (w >> 19) ^ t ^ (t >> 8);

        return w;
}
