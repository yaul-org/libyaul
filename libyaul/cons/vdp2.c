/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <inttypes.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <vdp2/cram.h>
#include <vdp2/pn.h>
#include <vdp2/priority.h>
#include <vdp2/scrn.h>
#include <vdp2/tvmd.h>
#include <vdp2/vram.h>

#include "cons.h"

typedef struct {
        uint16_t *pnt[4];
        uint32_t *character;
        uint32_t character_no;
} cons_vdp2_t;

static cons_vdp2_t *cons_vdp2_new(void);

static void cons_vdp2_clear(struct cons *, int32_t, int32_t, int32_t, int32_t);
static void cons_vdp2_reset(struct cons *);
static void cons_vdp2_scroll(struct cons *);
static void cons_vdp2_write(struct cons *, int, uint8_t, uint8_t);

void
cons_vdp2_init(struct cons *cons)
{
        struct scrn_ch_format cfg;
        struct vram_ctl *vram_ctl;

        uint32_t y;

        cons_vdp2_t *cons_vdp2;

        cons_vdp2 = cons_vdp2_new();

        cons->driver = cons_vdp2;

        cons->clear = cons_vdp2_clear;
        cons->reset = cons_vdp2_reset;
        cons->scroll = cons_vdp2_scroll;
        cons->write = cons_vdp2_write;

        cons_reset(cons);

        /* We want to be in VBLANK-IN (retrace) */
        vdp2_tvmd_display_clear();

        /* VRAM B1 */
        cons_vdp2->pnt[0] = (uint16_t *)VRAM_ADDR_4MBIT(3, 0x10000);
        /* VRAM B1 */
        cons_vdp2->pnt[1] = (uint16_t *)VRAM_ADDR_4MBIT(3, 0x10000);
        /* VRAM B1 */
        cons_vdp2->pnt[2] = (uint16_t *)VRAM_ADDR_4MBIT(3, 0x18000);
        /* VRAM B1 */
        cons_vdp2->pnt[3] = (uint16_t *)VRAM_ADDR_4MBIT(3, 0x18000);
        /* VRAM B1 */
        cons_vdp2->character = (uint32_t *)VRAM_ADDR_4MBIT(3, 0x00000);

        cfg.ch_scrn = SCRN_NBG2;
        cfg.ch_cs = 1 * 1; /* 1x1 cells */
        cfg.ch_pnds = 1; /* 1 word */
        cfg.ch_cnsm = 1; /* Character number supplement mode: 1 */
        cfg.ch_sp = 0;
        cfg.ch_scc = 0;
        cfg.ch_spn = 0;
        cfg.ch_scn = (uint32_t)cons_vdp2->character;
        cfg.ch_pls = 1 * 1; /* 1x1 plane size */
        cfg.ch_map[0] = (uint32_t)cons_vdp2->pnt[0];
        cfg.ch_map[1] = (uint32_t)cons_vdp2->pnt[1];
        cfg.ch_map[2] = (uint32_t)cons_vdp2->pnt[2];
        cfg.ch_map[3] = (uint32_t)cons_vdp2->pnt[3];

        vdp2_scrn_ccc_set(SCRN_NBG2, SCRN_CCC_CHC_16);
        vdp2_scrn_ch_format_set(&cfg);
        vdp2_priority_spn_set(SCRN_NBG2, 7);

        vram_ctl = vdp2_vram_control_get();
        vram_ctl->vram_cycp.pt[3].t7 = VRAM_CTL_CYCP_CHPNDR_NBG2;
        vram_ctl->vram_cycp.pt[3].t6 = VRAM_CTL_CYCP_PNDR_NBG2;
        vram_ctl->vram_cycp.pt[3].t5 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t4 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t3 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t2 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t1 = VRAM_CTL_CYCP_CPU_RW;
        vram_ctl->vram_cycp.pt[3].t0 = VRAM_CTL_CYCP_CPU_RW;
        vdp2_vram_control_set(vram_ctl);

        /* Clear the first unpacked cell of the font */
        for (y = 0; y < FONT_H; y++)
                cons_vdp2->character[y] = font_unpacked[0];

        memcpy((uint16_t *)CRAM_BANK(0, 0), palette,
            FONT_NCOLORS * sizeof(uint16_t));

        cons->clear(cons, 0, COLS, 0, ROWS);

        /* Hopefully it won't glitch */
        vdp2_scrn_display_set(SCRN_NBG2, /* no_trans = */ false);
        vdp2_tvmd_display_set();
}

static cons_vdp2_t *
cons_vdp2_new(void)
{
        cons_vdp2_t *cons_vdp2;

        if ((cons_vdp2 = (cons_vdp2_t *)malloc(sizeof(cons_vdp2_t))) == NULL)
                return NULL;

        memset(cons_vdp2, 0, sizeof(cons_vdp2_t));
        return cons_vdp2;
}

static void
cons_vdp2_clear(struct cons *cons, int32_t col_start, int32_t col_end,
    int32_t row_start, int32_t row_end)
{
        cons_vdp2_t *cons_vdp2;
        int32_t col;
        int32_t row;
        uint16_t ofs;

        cons_vdp2 = cons->driver;

        /* Clear map */
        ofs = PN_CHARACTER_NO((uint32_t)cons_vdp2->character);
        for (row = row_start; row < row_end; row++) {
                for (col = col_start; col < col_end; col++)
                        cons_vdp2->pnt[0][col + (row << 6)] = ofs;
        }
}

static void
cons_vdp2_reset(struct cons *cons)
{
        cons_vdp2_t *cons_vdp2;

        cons_vdp2 = cons->driver;
        cons_vdp2->character_no = ((cons->cursor.row % ROWS) * COLS) + cons->cursor.col + 1;
}

static void
cons_vdp2_scroll(struct cons *cons __attribute__ ((unused)))
{
        static int32_t scvy = 0;

        scvy += FONT_H;
        vdp2_scrn_scv_y_set(SCRN_NBG2, scvy, 0);
}

static void
cons_vdp2_write(struct cons *cons, int c, uint8_t fg, uint8_t bg)
{
        cons_vdp2_t *cons_vdp2;

        uint32_t cofs;
        uint32_t tofs;
        uint32_t row;
        uint32_t fg_mask;
        uint32_t bg_mask;

        uint32_t ofs;

        int y;

        static const uint32_t color_tbl[] = {
                0x00000000, 0x11111111, 0x22222222, 0x33333333,
                0x44444444, 0x55555555, 0x66666666, 0x77777777,
                0x88888888, 0x99999999, 0xAAAAAAAA, 0xBBBBBBBB,
                0xCCCCCCCC, 0xDDDDDDDD, 0xEEEEEEEE, 0xFFFFFFFF
        };

        cons_vdp2 = cons->driver;

        tofs = c << 3;
        cofs = cons_vdp2->character_no << 3;

        fg_mask = color_tbl[fg];
        bg_mask = color_tbl[bg];

        /* Expand cell */
        for (y = FONT_H - 1; y >= 0; y--) {
                row = font_unpacked[font[y + tofs]];
                cons_vdp2->character[y + cofs] = (row & fg_mask) | ((row & bg_mask) ^ bg_mask);
        }

        ofs = PN_CHARACTER_NO((uint32_t)cons_vdp2->character) | cons_vdp2->character_no;
        cons_vdp2->pnt[0][cons->cursor.col + (cons->cursor.row << 6)] = ofs;

        cons_vdp2->character_no++;
        if ((cons_vdp2->character_no % (ROWS * COLS)) == 0)
                cons_vdp2->character_no = 1;
}
