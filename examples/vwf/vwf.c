/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <vdp2.h>
#include <vdp2/pn.h>

#include <cons/vdp2.h>

#include <fs/romdisk/romdisk.h>

#define WSWAP_16(x) ((((x) >> 8) & 0x00FF) | (((x) & 0x00FF) << 8))

typedef struct {
        uint16_t x;
        uint16_t y;
        uint16_t width;
        uint16_t height;
        int16_t x_offs;
        int16_t y_offs;
        int16_t x_advance;
} __attribute__ ((packed)) glyph;

typedef struct {
        char first;
        char second;
        int8_t amount;
} __attribute__ ((packed)) kerning_pair;

static uint32_t *map[4] = {
        (uint32_t *)VRAM_ADDR_4MBIT(2, 0x00000),
        (uint32_t *)VRAM_ADDR_4MBIT(2, 0x00000),
        (uint32_t *)VRAM_ADDR_4MBIT(2, 0x00000),
        (uint32_t *)VRAM_ADDR_4MBIT(2, 0x00000)
};

static uint16_t *pal = (uint16_t *)CRAM_BANK(0x00, 0);
static uint8_t *chr = (uint8_t *)VRAM_ADDR_4MBIT(3, 0x00400);

static void nbg1_init(void);
static uint8_t get_pixel(uint8_t *, uint32_t, uint32_t);
static void set_pixel(uint8_t *, uint32_t, uint32_t, uint8_t);

static void write_vwf(const char *, glyph *, kerning_pair *, size_t, uint8_t *);

static struct cons cons;

int
main(void)
{
        extern uint8_t root_romdisk[];

        void *romdisk;
        void *fh; /* File handle */

        uint8_t *a_font16;
        ssize_t a_font16_len;

        uint8_t *font;
        ssize_t font_len;

        uint16_t *palette;
        ssize_t palette_len;

        uint8_t *glyphikp;
        ssize_t glyphikp_len;

        ssize_t nbg1_pnd_len;

        glyph *glyphs;
        uint16_t glyphscnt;

        kerning_pair *kerning_pairs;
        uint16_t kpcnt;

        uint16_t blcs_color[] = {
                0x8000
        };

        vdp2_init();
        vdp2_tvmd_blcs_set(/* lcclmd = */ false, VRAM_ADDR_4MBIT(3, 0x1FFFE),
            blcs_color, 0);

        cons_vdp2_init(&cons);

        nbg1_init();

        romdisk_init();
        romdisk = romdisk_mount("/", root_romdisk);

        if ((fh = romdisk_open(romdisk, "A_FONT16.BIN", O_RDONLY)) == NULL) {
                cons_write(&cons, "[1;31mCould not load A_FONT16.BIN[0m\n");
                abort();
        }

        a_font16 = (uint8_t *)0x00200000;
        a_font16_len = romdisk_total(fh);

        memset(a_font16, 0x00, a_font16_len);

        if ((romdisk_read(fh, a_font16, a_font16_len)) != a_font16_len) {
                cons_write(&cons, "[1;31mCould read A_FONT16.BIN[0m\n");
                abort();
        }
        romdisk_close(fh);


        if ((fh = romdisk_open(romdisk, "A_FONTVW.BIN", O_RDONLY)) == NULL) {
                cons_write(&cons, "[1;31mCould not load A_FONTVW.BIN[0m\n");
                abort();
        }

        font = (uint8_t *)0x00210000;
        font_len = romdisk_total(fh);

        memset(font, 0x00, (font_len * 2));

        if ((romdisk_read(fh, font, font_len)) != font_len) {
                cons_write(&cons, "[1;31mCould read A_FONTVW.BIN[0m\n");
                abort();
        }
        romdisk_close(fh);


        if ((fh = romdisk_open(romdisk, "A_FONTVW.PAL", O_RDONLY)) == NULL) {
                cons_write(&cons, "[1;31mCould not load A_FONTVW.PAL[0m\n");
                abort();
        }

        palette = pal;
        palette_len = romdisk_total(fh);

        memset(palette, 0x00, palette_len);

        if ((romdisk_read(fh, palette, palette_len)) != palette_len) {
                cons_write(&cons, "[1;31mCould read A_FONTVW.PAL[0m\n");
                abort();
        }
        romdisk_close(fh);

        uint32_t palidx;

        for (palidx = 0; palidx < (palette_len / sizeof(uint16_t)); palidx++) {
                palette[palidx] = 0x8000 | WSWAP_16(palette[palidx]);
        }

        if ((fh = romdisk_open(romdisk, "GLYPHIKP.BIN", O_RDONLY)) == NULL) {
                cons_write(&cons, "[1;31mCould not load GLYPHIKP.BIN[0m\n");
                abort();
        }

        glyphikp = (uint8_t *)0x00220000;
        glyphikp_len = romdisk_total(fh);

        memset(glyphikp, 0x00, glyphikp_len);

        if ((romdisk_read(fh, glyphikp, glyphikp_len)) != glyphikp_len) {
                cons_write(&cons, "[1;31mCould read GLYPHIKP.BIN[0m\n");
                abort();
        }
        romdisk_close(fh);

        glyphscnt = 256;
        glyphs = (glyph *)(glyphikp + 2);

        kpcnt = *(uint16_t *)(glyphikp + 2 + (glyphscnt * sizeof(glyph)));
        kerning_pairs =
            (kerning_pair *)((glyphikp + 2 + (glyphscnt * sizeof(glyph))) + 2);

        if ((fh = romdisk_open(romdisk, "NBG1_PND.BIN", O_RDONLY)) == NULL) {
                cons_write(&cons, "[1;31mCould not load NBG1_PND.BIN[0m\n");
                abort();
        }

        nbg1_pnd_len = romdisk_total(fh);
        if ((romdisk_read(fh, map[0], nbg1_pnd_len)) != nbg1_pnd_len) {
                cons_write(&cons, "[1;31mCould read NBG1_PND.BIN[0m\n");
                abort();
        }
        romdisk_close(fh);

        write_vwf("Kerning: Ay.y,w.w,v.v,r.r,Yv11A ",
            glyphs,
            kerning_pairs,
            kpcnt,
            font);

        while (true) {
                vdp2_tvmd_vblank_in_wait();
                vdp2_tvmd_vblank_out_wait();
        }

        return 0;
}

static void
bitmap_offset(uint32_t x, uint32_t *x_offset, uint32_t y, uint32_t *y_offset)
{
        const uint32_t TILES = 38;
        const uint32_t TILE_SIZE = 64;
        const uint32_t TILE_WIDTH = 8;

        uint32_t tile_num = x >> 3;
        uint32_t line_num = y >> 3;

        *x_offset =
            (x - (tile_num * TILE_WIDTH)) + (tile_num * TILE_SIZE);
        *y_offset =
            ((y - (line_num * TILE_WIDTH)) * TILE_WIDTH) +
            (line_num * TILES * TILE_SIZE);
}

static uint8_t __attribute__ ((unused))
get_pixel(uint8_t *dst, uint32_t x, uint32_t y)
{
        uint32_t x_offset;
        uint32_t y_offset;

        bitmap_offset(x, &x_offset, y, &y_offset);

        return dst[x_offset + y_offset];
}

static void
set_pixel(uint8_t *dst, uint32_t x, uint32_t y, uint8_t color)
{
        uint32_t x_offset;
        uint32_t y_offset;

        bitmap_offset(x, &x_offset, y, &y_offset);

        dst[x_offset + y_offset] = color;
}

static void
draw_glyph(char c, glyph *glyphs, kerning_pair *kp, size_t kpcnt, uint8_t *font)
{
        static char prev_c = '\0';

        static uint32_t x_offs = 0;
        static uint32_t y_offs = 0;

        uint32_t kpidx;
        int16_t amount;

        amount = 0;

        if (prev_c != '\0') {
                for (kpidx = 0; kpidx < kpcnt; kpidx++) {
                        kerning_pair *kpp;

                        kpp = &kp[kpidx];
                        if ((kpp->first == prev_c) && (kpp->second == c)) {
                                amount = kp->amount;

                                break;
                        }
                }
        }

        uint32_t x;
        uint32_t y;

        glyph *glyph;

        glyph = &glyphs[(int)c];

        for (y = 0; y < glyph->height; y++) {
                for (x = 0; x < glyph->width; x++) {
                        uint8_t pixel;

                        uint32_t px;
                        uint32_t py;
                        uint32_t fx;
                        uint32_t fy;

                        fx = glyph->x + x;
                        fy = (glyph->y + y) << 8;

                        pixel = font[fx + fy];

                        px = x + glyph->x_offs + amount + x_offs;
                        py = y + glyph->y_offs + y_offs;

                        if (pixel != 0x00) {
                                set_pixel(chr, px, py, pixel);
                        }
                }
        }

        x_offs += glyph->x_advance;

        prev_c = c;
}

static void
write_vwf(const char *msg, glyph *glyphs, kerning_pair *kp, size_t kpcnt,
    uint8_t *font)
{
        for (; *msg != '\0'; msg++) {
                draw_glyph(*msg, glyphs, kp, kpcnt, font);
        }
}

static void
nbg1_init(void)
{
        struct scrn_ch_format cfg;
        struct vram_ctl *vram_ctl;

        cfg.ch_scrn = SCRN_NBG1;
        cfg.ch_cs = 1 * 1;
        cfg.ch_pnds = 2;
        cfg.ch_cnsm = 0;
        cfg.ch_sp = 0;
        cfg.ch_spn = 0;
        cfg.ch_scn = (uint32_t)chr;
        cfg.ch_pls = 1 * 1;
        /* VRAM B0 */
        cfg.ch_map[0] = (uint32_t)map[0];
        /* VRAM B0 */
        cfg.ch_map[1] = (uint32_t)map[1];
        /* VRAM B0 */
        cfg.ch_map[2] = (uint32_t)map[2];
        /* VRAM B0 */
        cfg.ch_map[3] = (uint32_t)map[3];
        cfg.ch_mapofs = 0;

        vdp2_scrn_ccc_set(SCRN_NBG1, SCRN_CCC_CHC_256);
        vdp2_scrn_ch_format_set(&cfg);
        vdp2_priority_spn_set(SCRN_NBG1, 7);

        vram_ctl = vdp2_vram_control_get();
        vram_ctl->vram_cycp.pt[3].t2 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[3].t1 = VRAM_CTL_CYCP_CHPNDR_NBG1;
        vram_ctl->vram_cycp.pt[2].t0 = VRAM_CTL_CYCP_PNDR_NBG1;
        vdp2_vram_control_set(vram_ctl);

        vdp2_scrn_display_set(SCRN_NBG1, /* no_trans = */ false);
}
