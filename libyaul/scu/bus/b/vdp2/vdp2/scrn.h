/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_SCRN_H_
#define _VDP2_SCRN_H_

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SCRN_NBG0               0 /* Normal background (NBG0) */
#define SCRN_RBG1               5 /* Rotational background (RBG1) */
#define SCRN_NBG1               1 /* Normal background (NBG1) */
#define SCRN_NBG2               2 /* Normal background (NBG2) */
#define SCRN_NBG3               3 /* Normal background (NBG3) */
#define SCRN_RBG0               4 /* Rotational background (RBG0) */
#define SCRN_BACK               5
#define SCRN_SPRITE             6

#define SCRN_CCC_PALETTE_16     0
#define SCRN_CCC_PALETTE_256    1
#define SCRN_CCC_PALETTE_2048   2
#define SCRN_CCC_RGB_32768      3
#define SCRN_CCC_RGB_16770000   4

#define SCRN_CALCULATE_PAGE_COUNT(format)                                      \
        ((format)->scf_plane_size)

#define SCRN_CALCULATE_PAGE_WIDTH(format)                                      \
        (((format)->scf_character_size == (1 * 1)) ? 64 : 32)

#define SCRN_CALCULATE_PAGE_HEIGHT(format)                                     \
        (((format)->scf_character_size == (1 * 1)) ? 64 : 32)

#define SCRN_CALCULATE_PAGE_DIMENSION(format)                                  \
        (((format)->scf_character_size == (1 * 1)) ? (64 * 64) : (32 * 32))

/* Possible values for SCRN_CALCULATE_PAGE_SIZE() (in bytes):
 * +----------+-----------+---------------+
 * | PND size | Cell size | Size of page  |
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
 * +------------+----------+-----------+---------------+
 */
#define SCRN_CALCULATE_PLANE_SIZE(format)                                      \
        (((format)->scf_plane_size) * SCRN_CALCULATE_PAGE_SIZE(format))

/*-
 * Possible values (plane count) that can fit within a 4Mbit VRAM bank:
 *
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
 * +-------------+------------+----------+------------+------------+
 */
#define SCRN_CALCULATE_PLANES_2SPLIT_4MBIT_CNT(format)                         \
        (VRAM_2SPLIT_BANK_SIZE_4MBIT / SCRN_CALCULATE_PLANE_SIZE((format)))

#define SCRN_CALCULATE_PLANES_4SPLIT_4MBIT_CNT(format)                         \
        (VRAM_4SPLIT_BANK_SIZE_4MBIT / SCRN_CALCULATE_PLANE_SIZE((format)))

#define SCRN_CALCULATE_PLANES_2SPLIT_8MBIT_CNT(format)                         \
        (VRAM_2SPLIT_BANK_SIZE_8MBIT / SCRN_CALCULATE_PLANE_SIZE((format)))

#define SCRN_CALCULATE_PLANES_4SPLIT_8MBIT_CNT(format)                         \
        (VRAM_4SPLIT_BANK_SIZE_8MBIT / SCRN_CALCULATE_PLANE_SIZE((format)))

/* Configuration table mapping. Depending on how the normal/rotational
 * background is set up, choose a config:
 * +-----+--------+------+-----------+------------+------------+-------------+
 * | CFG | PND    | Cell | Aux. mode | Chr. count | Pal. banks | Color count |
 * +-----+--------+------+-----------+------------+------------+-------------+
 * | 0   | 1-word | 1x1  | 0         | 0x0400     | 128        | 16          |
 * | 1   | 1-word | 1x1  | 1         | 0x1000     | 128        | 16          |
 * | 2   | 1-word | 2x2  | 0         | 0x0400     | 128        | 16          |
 * | 3   | 1-word | 2x2  | 1         | 0x1000     | 128        | 16          |
 * | 4   | 1-word | 1x1  | 0         | 0x0200     | 8          | 16,256,2048 |
 * | 5   | 1-word | 1x1  | 1         | 0x0800     | 8          | 16,256,2048 |
 * | 6   | 1-word | 2x2  | 0         | 0x0200     | 8          | 16,256,2048 |
 * | 7   | 1-word | 2x2  | 1         | 0x0800     | 8          | 16,256,2048 |
 * | 8   | 2-word | X    | X         | 0x8000     | 8/128      | 16,256,2048 |
 * +-----+--------+------+-----------+------------+------------+-------------+
 */

#define CHARACTER_NUMBER(x)     ((x) >> 5)
#define PALETTE_NUMBER(x)       ((x) >> 5)

#define SCRN_PND_CONFIG_0(character_addr, palette_addr, vf, hf)                \
        (((PALETTE_NUMBER(palette_addr) & 0x000F) << 12) |                     \
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         (CHARACTER_NUMBER(character_addr) & 0x0FFF))

#define SCRN_PND_CONFIG_1(character_addr, palette_addr)                        \
        (((PALETTE_NUMBER(palette_addr) & 0x000F) << 12) |                     \
         (CHARACTER_NUMBER(character_addr) & 0x03FF))

#define SCRN_PND_CONFIG_2(character_addr, palette_addr, vf, hf)                \
        (((PALETTE_NUMBER(palette_addr) & 0x000F) << 12) |                     \
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         ((CHARACTER_NUMBER(character_addr) & 0x0FFC) >> 2))

#define SCRN_PND_CONFIG_3(character_addr, palette_addr)                        \
        (((PALETTE_NUMBER(palette_addr) & 0x000F) << 12) |                     \
         ((CHARACTER_NUMBER(character_addr) & 0x0FFC) >> 2))

#define SCRN_PND_CONFIG_4(character_addr, palette_addr, vf, hf)                \
        ((((PALETTE_NUMBER(palette_addr) >> 4) & 0x0007) << 12) |              \
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         (CHARACTER_NUMBER(character_addr) & 0x0FFF))

#define SCRN_PND_CONFIG_5(character_addr, palette_addr)                        \
        ((((PALETTE_NUMBER(palette_addr) >> 4) & 0x0007) << 12) |              \
         (CHARACTER_NUMBER(character_addr) & 0x03FF))

#define SCRN_PND_CONFIG_6(character_addr, palette_addr, vf, hf)                \
        ((((PALETTE_NUMBER(palette_addr) >> 4) & 0x0007) << 12) |              \
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         ((CHARACTER_NUMBER(character_addr) & 0x0FFC) >> 2))

#define SCRN_PND_CONFIG_7(character_addr, palette_addr, vf, hf)                \
        ((((PALETTE_NUMBER(palette_addr) >> 4) & 0x0007) << 12) |              \
         ((CHARACTER_NUMBER(character_addr) & 0x0FFC) >> 2))

#define SCRN_PND_CONFIG_8(character_addr, palette_addr, vf, hf, pr, cc)        \
        ((((vf) & 0x01) << 31) |                                               \
         (((hf) & 0x01) << 30) |                                               \
         (((pr) & 0x01) << 29) |                                               \
         (((cc) & 0x01) << 28) |                                               \
         ((PALETTE_NUMBER(palette_addr) & 0x007F) << 16) |                     \
         (CHARACTER_NUMBER(character_addr) & 0x7FFF))

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
        uint32_t sbf_sf_type; /* Special function type priority
                               * Special function type color calculation */
        uint32_t sbf_sf_code; /* Special function code A
                               * Special function code B */
        uint32_t sbf_sf_mode; /* Mode 0 (per screen)
                               * Mode 1 (per cell)
                               * Mode 2 (per pixel) */
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
        uint32_t scf_sf_type; /* Special function type priority
                               * Special function type color calculation */
        uint32_t scf_sf_code; /* Special function code A
                               * Special function code B */
        uint32_t scf_sf_mode; /* Mode 0 (per screen)
                               * Mode 1 (per cell)
                               * Mode 2 (per pixel) */

        union {
                uint32_t planes[16];

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
                } __packed;
        } scf_map;                      /* Map lead addresses */

        uint32_t scf_rp_mode; /* RBG0 and RBG1 only
                               * Rotation parameter mode
                               *   Mode 0: Rotation Parameter A
                               *   Mode 1: Rotation Parameter B
                               *   Mode 2: Swap Coefficient Data Read
                               *   Mode 3: Swap via Rotation Parameter Window */
};

#define SCRN_SF_TYPE_NONE               0
#define SCRN_SF_TYPE_COLOR_CALCULATION  1
#define SCRN_SF_TYPE_PRIORITY           2

#define SCRN_SF_CODE_A          0
#define SCRN_SF_CODE_B          1

#define SCRN_SF_CODE_0x00_0x01  (1 << 0)
#define SCRN_SF_CODE_0x02_0x03  (1 << 1)
#define SCRN_SF_CODE_0x04_0x05  (1 << 2)
#define SCRN_SF_CODE_0x06_0x07  (1 << 3)
#define SCRN_SF_CODE_0x08_0x09  (1 << 4)
#define SCRN_SF_CODE_0x0A_0x0B  (1 << 5)
#define SCRN_SF_CODE_0x0C_0x0D  (1 << 6)
#define SCRN_SF_CODE_0x0E_0x0F  (1 << 7)

/* Limitations for NBG0/NBG1 reduction:
 * +--------+-----+-----------+------------+
 * | Screen | CCC | Reduction | No display |
 * +--------+-----+-----------+------------+
 * | NBG0   | 16  | 1/2       | None       |
 * |        |     +-----------+------------+
 * |        |     | 1/4       | NBG2       |
 * |        +-----+-----------+------------+
 * |        | 256 | 1/2       | NBG2       |
 * +--------+-----+-----------+------------+
 * | NBG1   | 16  | 1/2       | None       |
 * |        |     +-----------+------------+
 * |        |     | 1/4       | NBG3       |
 * |        +-----+-----------+------------+
 * |        | 256 | 1/2       | NBG3       |
 * +--------+-----+-----------+------------+
 *
 * The coord. increment should be a value smaller then 1 to zoom in and
 * larger than 1 to zoom out. No zoom means equal to 1.
 *
 * Range limitations:
 * +----------------+-------------------+-----+
 * | Range (X or Y) | Reduction results | CCC |
 * +----------------+-------------------+-----+
 * | [0,1)          | Reduc. in         | 16  |
 * |                |                   | 256 |
 * +----------------+-------------------+-----+
 * | 1              | No reduction      | 16  |
 * +----------------+-------------------+-----+
 * | (1,2]          | Reduc. out (1/2)  | 16  |
 * |                |                   | 256 |
 * +----------------+-------------------+-----+
 * | (1,7]          | Reduc. out (1/7)  | 16  |
 * +----------------+-------------------+-----+ */

#define SCRN_REDUCTION_NONE     0 /* No reduction */
#define SCRN_REDUCTION_HALF     1 /* 1/2 reduction */
#define SCRN_REDUCTION_QUARTER  2 /* 1/4 reduction */

#define SCRN_REDUCTION_STEP     Q0_3_8(1.0f / 256.0f)
#define SCRN_REDUCTION_MIN      SCRN_REDUCTION_STEP
#define SCRN_REDUCTION_MAX      Q0_3_8(7.0f)

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

#define SCRN_COLOR_OFFSET_A     0
#define SCRN_COLOR_OFFSET_B     1

extern void vdp2_scrn_back_screen_addr_set(uint32_t, bool);
extern void vdp2_scrn_back_screen_color_set(uint32_t, color_rgb555_t);
extern void vdp2_scrn_bitmap_format_set(struct scrn_bitmap_format *);
extern void vdp2_scrn_cell_format_set(const struct scrn_cell_format *);
extern const struct scrn_cell_format *vdp2_scrn_cell_format_get(uint8_t);
extern void vdp2_scrn_color_offset_clear(void);
extern void vdp2_scrn_color_offset_rgb_set(uint8_t, int16_t,  int16_t, int16_t);
extern void vdp2_scrn_color_offset_set(uint8_t, uint8_t);
extern void vdp2_scrn_color_offset_unset(uint8_t);
extern void vdp2_scrn_display_clear(void);
extern void vdp2_scrn_display_set(uint8_t, bool);
extern void vdp2_scrn_display_unset(uint8_t);
extern void vdp2_scrn_ls_set(struct scrn_ls_format *);
extern void vdp2_scrn_mosaic_clear(void);
extern void vdp2_scrn_mosaic_horizontal_set(uint32_t);
extern void vdp2_scrn_mosaic_set(uint8_t);
extern void vdp2_scrn_mosaic_unset(uint8_t);
extern void vdp2_scrn_mosaic_vertical_set(uint32_t);
extern uint8_t vdp2_scrn_priority_get(uint8_t);
extern void vdp2_scrn_priority_set(uint8_t, uint8_t);
extern void vdp2_scrn_reduction_set(uint8_t, uint16_t);
extern void vdp2_scrn_reduction_x_set(uint8_t scrn, q0_3_8_t);
extern void vdp2_scrn_reduction_y_set(uint8_t scrn, q0_3_8_t);
extern void vdp2_scrn_scroll_x_set(uint8_t, fix16_t);
extern void vdp2_scrn_scroll_x_update(uint8_t, fix16_t);
extern void vdp2_scrn_scroll_y_set(uint8_t, fix16_t);
extern void vdp2_scrn_scroll_y_update(uint8_t, fix16_t);
extern void vdp2_scrn_sf_codes_set(uint8_t, uint8_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_VDP2_SCRN_H_ */
