/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_SCRN_H_
#define _VDP2_SCRN_H_

#include <stdbool.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SCRN_NBG0               0 /* Normal background (NBG0) */
#define SCRN_RBG1               5 /* Rotational background (RBG1) */
#define SCRN_NBG1               1 /* Normal background (NBG1) */
#define SCRN_NBG2               2 /* Normal background (NBG2) */
#define SCRN_NBG3               3 /* Normal background (NBG3) */
#define SCRN_RBG0               4 /* Rotational background (RBG0) */

#define SCRN_CCC_PALETTE_16     0
#define SCRN_CCC_PALETTE_256    1
#define SCRN_CCC_PALETTE_2048   2
#define SCRN_CCC_RGB_32768      3
#define SCRN_CCC_RGB_16770000   4

#define SCRN_CALCULATE_PAGE_WIDTH(format)                                      \
        (((format)->scf_character_size == (1 * 1)) ? 64 : 32)

#define SCRN_CALCULATE_PAGE_HEIGHT(format)                                     \
        (((format)->scf_character_size == (1 * 1)) ? 64 : 32)

#define SCRN_CALCULATE_PAGE_DIMENSION(format)                                  \
        (((format)->scf_character_size == (1 * 1)) ? (64 * 64) : (32 * 32))

/* Possible values for SCRN_CALCULATE_PAGE_SIZE() (in bytes):
 * +----------+-----------+---------------+
 * | PND size | Cell size | Size of plane |
 * +----------+-----------+---------------+
 * | 1-word   | 1x1       | 0x2000        |
 * | 1-word   | 2x2       | 0x0800        |
 * | 2-word   | 1x1       | 0x4000        |
 * | 2-word   | 2x2       | 0x1000        |
 * +----------+-----------+---------------+
 *
 * Page dimension is 64x64 if cell size is 1x1.
 * Page dimension is 32x32 if cell size is 2x2. */
#define SCRN_CALCULATE_PAGE_SIZE(format)                                       \
        (SCRN_CALCULATE_PAGE_DIMENSION(format) * ((format)->scf_pnd_size * 2))

/* Possible vales for SCRN_CALCULATE_PLANE_SIZE() (in bytes):
 * +------------+----------+-----------+---------------+
 * | Plane size | PND size | Cell size | Size of plane |
 * +------------+----------+-----------+---------------+
 * | 1x1        | 1-word   | 1x1       | 0x2000        |
 * |            | 1-word   | 2x2       | 0x0800        |
 * |            | 2-word   | 1x1       | 0x4000        |
 * |            | 2-word   | 2x2       | 0x1000        |
 * +------------+----------+-----------+---------------+
 * | 2x1        | 1-word   | 1x1       | 0x4000        |
 * |            | 1-word   | 2x2       | 0x1000        |
 * |            | 2-word   | 1x1       | 0x8000        |
 * |            | 2-word   | 2x2       | 0x2000        |
 * +------------+----------+-----------+---------------+
 * | 2x2        | 1-word   | 1x1       | 0x8000        |
 * |            | 1-word   | 2x2       | 0x2000        |
 * |            | 2-word   | 1x1       | 0x10000       |
 * |            | 2-word   | 2x2       | 0x4000        |
 * +------------+----------+-----------+---------------+ */
#define SCRN_CALCULATE_PLANE_SIZE(format)                                      \
        (((format)->scf_plane_size) * SCRN_CALCULATE_PAGE_SIZE(format))

/* Possible values (plane count) for SCRN_CALCULATE_PLANES_CNT():
 * +-------------+------------+----------+------------+------------+
 * | Planes/bank | Char. size | PND size | Plane dim. | Plane size |
 * +-------------+------------+----------+------------+------------+
 * | 16          | 1x1        | 1-word   | 1x1        | 0x002000   |
 * | 64          | 2x2        | 1-word   | 1x1        | 0x000800   |
 * |  8          | 1x1        | 2-word   | 1x1        | 0x004000   |
 * | 32          | 2x2        | 2-word   | 1x1        | 0x001000   |
 * +-------------+------------+----------+------------+------------+
 * |  8          | 1x1        | 1-word   | 2x1        | 0x004000   |
 * | 32          | 2x2        | 1-word   | 2x1        | 0x001000   |
 * |  4          | 1x1        | 2-word   | 2x1        | 0x008000   |
 * | 16          | 2x2        | 2-word   | 2x1        | 0x002000   |
 * +-------------+------------+----------+------------+------------+
 * |  4          | 1x1        | 1-word   | 2x2        | 0x008000   |
 * | 16          | 2x2        | 1-word   | 2x2        | 0x002000   |
 * |  2          | 1x1        | 2-word   | 2x2        | 0x010000   |
 * |  8          | 2x2        | 2-word   | 2x2        | 0x004000   |
 * +-------------+------------+----------+------------+------------+ */
#define SCRN_CALCULATE_PLANES_CNT(format)                                      \
        (VRAM_BANK_SIZE / SCRN_CALCULATE_PLANE_SIZE((format)))

struct scrn_bitmap_format {
        uint8_t sbf_scroll_screen; /* Normal/rotational background */
        uint32_t sbf_cc_count; /* Character color count */

        struct {
                uint16_t width;
                uint16_t height;
        } sbf_bitmap_size; /* Bitmap sizes: 512x256, 512x512, 1024x256,
                            * 1024x512 */
        uint32_t sbf_color_palette; /* Color palette lead address (if
                                     * applicable) */
        uint32_t sbf_bitmap_pattern; /* Bitmap pattern lead address */
        uint8_t sbf_rp_mode; /* RBG0 and RBG1 only
                              * Rotation parameter mode
                              *   Mode 0: Rotation Parameter A
                              *   Mode 1: Rotation Parameter B
                              *   Mode 2: Swap Coefficient Data Read
                              *   Mode 3: Swap via Rotation Parameter Window */
};

struct scrn_cell_format {
        uint32_t scf_scroll_screen; /* Normal/rotational background */
        uint32_t scf_cc_count; /* Character color count */
        uint32_t scf_character_size; /* Character size: (1 * 1) or (2 * 2) cells */
        uint32_t scf_pnd_size; /* Pattern name data size: (1)-word or (2)-words */
        uint32_t scf_auxiliary_mode; /* Auxiliary mode #0 (flip function) or
                                     * auxiliary mode #1 (no flip function) */
        uint32_t scf_cp_table; /* Character pattern table lead address*/
        uint32_t scf_color_palette; /* Color palette lead address */
        uint32_t scf_plane_size; /* Plane size: (1 * 1) or (2 * 1) or (2 * 2) */

        struct {
                uint32_t plane_a;
                uint32_t plane_b;
                uint32_t plane_c;
                uint32_t plane_d;
                uint32_t plane_e; /* For RBG0 and RBG1 use only */
                uint32_t plane_f; /* For RBG0 and RBG1 use only */
                uint32_t plane_g; /* For RBG0 and RBG1 use only */
                uint32_t plane_h; /* For RBG0 and RBG1 use only */
                uint32_t plane_i; /* For RBG0 and RBG1 use only */
                uint32_t plane_j; /* For RBG0 and RBG1 use only */
                uint32_t plane_k; /* For RBG0 and RBG1 use only */
                uint32_t plane_l; /* For RBG0 and RBG1 use only */
                uint32_t plane_m; /* For RBG0 and RBG1 use only */
                uint32_t plane_n; /* For RBG0 and RBG1 use only */
                uint32_t plane_o; /* For RBG0 and RBG1 use only */
                uint32_t plane_p; /* For RBG0 and RBG1 use only */
        } scf_map; /* Map lead addresses */

        uint32_t scf_rp_mode; /* RBG0 and RBG1 only
                               * Rotation parameter mode
                               *   Mode 0: Rotation Parameter A
                               *   Mode 1: Rotation Parameter B
                               *   Mode 2: Swap Coefficient Data Read
                               *   Mode 3: Swap via Rotation Parameter Window */
};

struct scrn_ls_format {
        uint8_t ls_scrn;        /* Normal background */
        uint32_t ls_lsta;       /* Line scroll table (lead addr.) */
        uint8_t ls_int;         /* Dependent on the interlace setting */

#define SCRN_LS_N0SCX   0x0002
#define SCRN_LS_N1SCX   0x0200
#define SCRN_LS_N0SCY   0x0004
#define SCRN_LS_N1SCY   0x0400
        uint16_t ls_fun;        /* Enable line scroll */
};

struct scrn_vcs_format {
        uint8_t vcs_scrn;       /* Normal background */
        uint32_t vcs_vcsta;     /* Vertical cell scroll table (lead addr.) */
};

extern void vdp2_scrn_back_screen_addr_set(bool, uint32_t);
extern void vdp2_scrn_back_screen_color_set(uint32_t, uint16_t);
extern void vdp2_scrn_bitmap_format_set(struct scrn_bitmap_format *);
extern void vdp2_scrn_cell_format_set(const struct scrn_cell_format *);
extern void vdp2_scrn_display_clear(void);
extern void vdp2_scrn_display_set(uint8_t, bool);
extern void vdp2_scrn_display_unset(uint8_t);
extern void vdp2_scrn_mosaic_clear(void);
extern void vdp2_scrn_mosaic_horizontal_set(uint32_t);
extern void vdp2_scrn_mosaic_set(uint8_t);
extern void vdp2_scrn_mosaic_unset(uint8_t);
extern void vdp2_scrn_mosaic_vertical_set(uint32_t);

extern void vdp2_scrn_ls_set(struct scrn_ls_format *);
extern void vdp2_scrn_scv_x_set(uint8_t, uint16_t, uint8_t);
extern void vdp2_scrn_scv_y_set(uint8_t, uint16_t, uint8_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_VDP2_SCRN_H_ */
