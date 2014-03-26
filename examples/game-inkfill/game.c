/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <vdp2.h>
#include <vdp2/pn.h>

#include <tga.h>

#include <assert.h>
#include <inttypes.h>
#include <string.h>

#include "fs.h"
#include "game.h"
#include "globals.h"
#include "scene.h"

#define RGB888_TO_RGB555(r, g, b) ((((b) >> 3) << 10) | (((g) >> 3) << 5) | ((r) >> 3))

#define TRIES_MAX 26

#define GRID_CELL_COLOR_RED     0
#define GRID_CELL_COLOR_ORANGE  1
#define GRID_CELL_COLOR_YELLOW  2
#define GRID_CELL_COLOR_BLUE    3

#define GRID_WIDTH      18
#define GRID_HEIGHT     12

static bool _changed = false;
static uint32_t _tries = 0;

static uint16_t *_pnt[4] = {
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x0800),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x0800),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x0800),
        /* VRAM B0 */
        (uint16_t *)VRAM_ADDR_4MBIT(2, 0x0800)
};

/* VRAM B0 */
static uint32_t *_character = (uint32_t *)VRAM_ADDR_4MBIT(2, 0x00000);

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
        static uint16_t blcs_color[] = {
                0x8000
        };

        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

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

        if (_tries >= TRIES_MAX) {
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
                0x0000,
                0x8000 | RGB888_TO_RGB555(0xFF, 0x00, 0x00), /* Red */
                0x8000 | RGB888_TO_RGB555(0xFF, 0xA5, 0x00), /* Orange */
                0x8000 | RGB888_TO_RGB555(0xFF, 0xFF, 0x00), /* Yellow */
                0x8000 | RGB888_TO_RGB555(0x00, 0x00, 0xFF), /* Blue */
                0x0000,
                0x0000,
                0x0000,
                0x0000,
                0x0000,
                0x0000,
                0x0000,
                0x0000,
                0x0000,
                0x0000,
                0x0000
        };

        struct scrn_ch_format nbg1_cfg;
        struct vram_ctl *vram_ctl;

        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear();

        nbg1_cfg.ch_scrn = SCRN_NBG1;
        nbg1_cfg.ch_cs = 2 * 2; /* 2x2 cells */
        nbg1_cfg.ch_pnds = 1; /* 1 word */
        nbg1_cfg.ch_cnsm = 1; /* Character number supplement mode: 1 */
        nbg1_cfg.ch_sp = 0;
        nbg1_cfg.ch_scc = 0;
        nbg1_cfg.ch_spn = 0;
        nbg1_cfg.ch_scn = (uint32_t)_character;
        nbg1_cfg.ch_pls = 1 * 1; /* 1x1 plane size */
        nbg1_cfg.ch_map[0] = (uint32_t)_pnt[0];
        nbg1_cfg.ch_map[1] = (uint32_t)_pnt[1];
        nbg1_cfg.ch_map[2] = (uint32_t)_pnt[2];
        nbg1_cfg.ch_map[3] = (uint32_t)_pnt[3];

        vdp2_scrn_ccc_set(SCRN_NBG1, SCRN_CCC_CHC_16);
        vdp2_scrn_ch_format_set(&nbg1_cfg);
        vdp2_priority_spn_set(SCRN_NBG1, 7);

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
        vram_ctl->vram_cycp.pt[2].t5 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t4 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t3 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t2 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t1 = VRAM_CTL_CYCP_NO_ACCESS;
        vram_ctl->vram_cycp.pt[2].t0 = VRAM_CTL_CYCP_NO_ACCESS;

        /* Draw the tiles */
        uint32_t tile_idx;

        for (tile_idx = 0; tile_idx < 6; tile_idx++) {
                uint32_t row;

                row = tiles[tile_idx];

                _character[0x00 + (tile_idx << 5)] = row;
                _character[0x01 + (tile_idx << 5)] = row;
                _character[0x02 + (tile_idx << 5)] = row;
                _character[0x03 + (tile_idx << 5)] = row;
                _character[0x04 + (tile_idx << 5)] = row;
                _character[0x05 + (tile_idx << 5)] = row;
                _character[0x06 + (tile_idx << 5)] = row;
                _character[0x07 + (tile_idx << 5)] = row;

                _character[0x08 + (tile_idx << 5)] = row;
                _character[0x09 + (tile_idx << 5)] = row;
                _character[0x0A + (tile_idx << 5)] = row;
                _character[0x0B + (tile_idx << 5)] = row;
                _character[0x0C + (tile_idx << 5)] = row;
                _character[0x0D + (tile_idx << 5)] = row;
                _character[0x0E + (tile_idx << 5)] = row;
                _character[0x0F + (tile_idx << 5)] = row;

                _character[0x10 + (tile_idx << 5)] = row;
                _character[0x11 + (tile_idx << 5)] = row;
                _character[0x12 + (tile_idx << 5)] = row;
                _character[0x13 + (tile_idx << 5)] = row;
                _character[0x14 + (tile_idx << 5)] = row;
                _character[0x15 + (tile_idx << 5)] = row;
                _character[0x16 + (tile_idx << 5)] = row;
                _character[0x17 + (tile_idx << 5)] = row;

                _character[0x18 + (tile_idx << 5)] = row;
                _character[0x19 + (tile_idx << 5)] = row;
                _character[0x1A + (tile_idx << 5)] = row;
                _character[0x1B + (tile_idx << 5)] = row;
                _character[0x1C + (tile_idx << 5)] = row;
                _character[0x1D + (tile_idx << 5)] = row;
                _character[0x1E + (tile_idx << 5)] = row;
                _character[0x1F + (tile_idx << 5)] = row;
        }

        /* Clear the entire map */
        uint16_t x;
        uint16_t y;

        for (y = 0; y < 32; y++) {
                for (x = 0; x < 32; x++) {
                        _pnt[0][x + (y << 5)] =
                            VDP2_PN_CONFIG_3_CHARACTER_NUMBER((uint32_t)_character);
                }
        }

        /* Copy palette */
        memcpy((uint16_t *)CRAM_BANK(0, 0), palette, 16 * sizeof(uint16_t));

        _grid_randomize();

        struct scrn_bm_format nbg0_cfg;

        nbg0_cfg.bm_scrn = SCRN_NBG0;
        nbg0_cfg.bm_bs = SCRN_BM_BMSZ_512_256;
        nbg0_cfg.bm_pb = VRAM_ADDR_4MBIT(0, 0x00000);
        nbg0_cfg.bm_sp = 0;
        nbg0_cfg.bm_spn = 0;
        nbg0_cfg.bm_scc = 0;

        vdp2_scrn_ccc_set(SCRN_NBG0, SCRN_CCC_CHC_32768);
        vdp2_scrn_bm_format_set(&nbg0_cfg);
        vdp2_priority_spn_set(SCRN_NBG0, 6);

        void *fh;

        fh = fs_open("/GAME.TGA");
        fs_load(fh, (uint8_t *)0x00201000);

        tga_t tga;
        int ret;

        ret = tga_read(&tga, (uint8_t *)0x00201000,
            (uint16_t *)VRAM_ADDR_4MBIT(0, 0x00000), NULL);
        assert(ret == TGA_FILE_OK);

        vdp2_vram_control_set(vram_ctl);
        vdp2_scrn_display_set(SCRN_NBG0, /* no_trans = */ false);
        vdp2_scrn_display_set(SCRN_NBG1, /* no_trans = */ false);
        vdp2_tvmd_display_set();
}

static void
_grid_update(void)
{
        if (g_digital.connected == 1) {
                if (g_digital.held.button.a) {
                        _grid_flood_fill(GRID_CELL_COLOR_YELLOW);
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
        uint32_t x;
        uint32_t y;

        /* Draw actual grid */
        for (y = 0; y < GRID_HEIGHT; y++) {
                for (x = 0; x < GRID_WIDTH; x++) {
                        uint8_t color;

                        color = _grid_color_get(x, y) + 1;
                        _pnt[0][x + (y << 5)] =
                            VDP2_PN_CONFIG_3_CHARACTER_NUMBER((uint32_t)_character) + color;
                }
        }
}

static void
_grid_exit(void)
{
        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear();

        vdp2_scrn_display_clear(SCRN_NBG0, /* no_trans = */ false);
        vdp2_scrn_display_clear(SCRN_NBG1, /* no_trans = */ false);
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
