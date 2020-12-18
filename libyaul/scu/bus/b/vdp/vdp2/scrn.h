/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_SCRN_H_
#define _VDP2_SCRN_H_

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include <vdp2/map.h>
#include <vdp2/vram.h>

__BEGIN_DECLS

typedef int16_t vdp2_color8_t;

typedef enum vdp2_scrn {
        /// Normal background.
        VDP2_SCRN_NBG0   = 0 , 
        /// Rotational background.
        VDP2_SCRN_RBG1   = 5, 
        /// Normal background.
        VDP2_SCRN_NBG1   = 1,
        /// Normal background.
        VDP2_SCRN_NBG2   = 2,
        /// Normal background.
        VDP2_SCRN_NBG3   = 3,
        /// Rotational background.
        VDP2_SCRN_RBG0   = 4,
        /// Back screen.
        VDP2_SCRN_BACK   = 5,
        /// Sprite layer.
        VDP2_SCRN_SPRITE = 6
} vdp2_scrn_t;

typedef enum vdp2_scrn_ccc {
        VDP2_SCRN_CCC_PALETTE_16   = 0,
        VDP2_SCRN_CCC_PALETTE_256  = 1,
        VDP2_SCRN_CCC_PALETTE_2048 = 2,
        VDP2_SCRN_CCC_RGB_32768    = 3,
        VDP2_SCRN_CCC_RGB_16770000 = 4
} vdp2_scrn_ccc_t;

typedef enum vdp2_scrn_ls_enable {
        /// Scrolls horizontally per line units.
        VDP2_SCRN_LS_HORZ      = 0x01,
        /// Scrolls vertically per line units.
        VDP2_SCRN_LS_VERT      = 0x02,
        /// Scales horizontally per line units.
        VDP2_SCRN_LS_ZOOM_HORZ = 0x04
} vdp2_scrn_ls_enable_t;

typedef enum vdp2_scrn_color_offset {
        /// Not yet documented.
        VDP2_SCRN_COLOR_OFFSET_A = 0,
        /// Not yet documented.
        VDP2_SCRN_COLOR_OFFSET_B = 1
} vdp2_scrn_color_offset_t;

typedef enum vdp2_scrn_reduction {
        /// No reduction
        VDP2_SCRN_REDUCTION_NONE    = 0,
        /// 1/2 reduction
        VDP2_SCRN_REDUCTION_HALF    = 1,
        /// 1/4 reduction
        VDP2_SCRN_REDUCTION_QUARTER = 2  
} vdp2_scrn_reduction_t;

typedef enum vdp2_scrn_sf_type {
        /// Not yet documented.
        VDP2_SCRN_SF_TYPE_NONE              = 0,
        /// Not yet documented.
        VDP2_SCRN_SF_TYPE_COLOR_CALCULATION = 1,
        /// Not yet documented.
        VDP2_SCRN_SF_TYPE_PRIORITY          = 2
} vdp2_scrn_sf_type_t;

typedef enum vdp2_scrn_sf_code {
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_A = 0,
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_B = 1
} vdp2_scrn_sf_code_t;

typedef enum vdp2_scrn_sf_code_range {
        VDP2_SCRN_SF_CODE_0x00_0x01 = (1 << 0),
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_0x02_0x03 = (1 << 1),
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_0x04_0x05 = (1 << 2),
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_0x06_0x07 = (1 << 3),
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_0x08_0x09 = (1 << 4),
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_0x0A_0x0B = (1 << 5),
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_0x0C_0x0D = (1 << 6),
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_0x0E_0x0F = (1 << 7)
} vdp2_scrn_sf_code_range_t;

#define VDP2_SCRN_CALCULATE_PAGE_COUNT(format)                                 \
        ((format)->plane_size)

#define VDP2_SCRN_CALCULATE_PAGE_WIDTH(format)                                 \
        (((format)->character_size == (1 * 1)) ? 64 : 32)

#define VDP2_SCRN_CALCULATE_PAGE_HEIGHT(format)                                \
        (((format)->character_size == (1 * 1)) ? 64 : 32)

#define VDP2_SCRN_CALCULATE_PAGE_DIMENSION(format)                             \
        (((format)->character_size == (1 * 1)) ? (64 * 64) : (32 * 32))

#define VDP2_SCRN_CALCULATE_PAGE_COUNT_M(plsz)                                 \
        (plsz)

#define VDP2_SCRN_CALCULATE_PAGE_WIDTH_M(chsz)                                 \
        (((chsz) == (1 * 1)) ? 64 : 32)

#define VDP2_SCRN_CALCULATE_PAGE_HEIGHT_M(chsz)                                \
        (((chsz) == (1 * 1)) ? 64 : 32)

#define VDP2_SCRN_CALCULATE_PAGE_DIMENSION_M(chsz)                             \
        (((chsz) == (1 * 1)) ? (64 * 64) : (32 * 32))

/*-
 * Possible values for VDP2_SCRN_CALCULATE_PAGE_SIZE() (in bytes):
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
#define VDP2_SCRN_CALCULATE_PAGE_SIZE(format)                                  \
        (VDP2_SCRN_CALCULATE_PAGE_DIMENSION(format) * ((format)->pnd_size * 2))

#define VDP2_SCRN_CALCULATE_PAGE_SIZE_M(chsz, pndsz)                           \
        (VDP2_SCRN_CALCULATE_PAGE_DIMENSION_M(chsz) * ((pndsz) * 2))

/*-
 * Possible vales for VDP2_SCRN_CALCULATE_PLANE_SIZE() (in bytes):
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

#define VDP2_SCRN_CALCULATE_PLANE_SIZE(format)                                 \
        (((format)->plane_size) * VDP2_SCRN_CALCULATE_PAGE_SIZE(format))

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

#define VDP2_SCRN_CALCULATE_PLANES_2_CNT(format)                               \
        (VDP2_VRAM_BSIZE_2 / VDP2_SCRN_CALCULATE_PLANE_SIZE((format)))

#define VDP2_SCRN_CALCULATE_PLANES_4_CNT(format)                               \
        (VDP2_VRAM_BSIZE_4 / VDP2_SCRN_CALCULATE_PLANE_SIZE((format)))

#define VDP2_SCRN_CALCULATE_PLANES_2_8MBIT_CNT(format)                         \
        (VDP2_VRAM_BSIZE_2_8MBIT / VDP2_SCRN_CALCULATE_PLANE_SIZE((format)))

#define VDP2_SCRN_CALCULATE_PLANES_4_8MBIT_CNT(format)                         \
        (VDP2_VRAM_BSIZE_4_8MBIT / VDP2_SCRN_CALCULATE_PLANE_SIZE((format)))

/*-
 * Configuration table mapping. Depending on how the normal/rotational
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

#define VDP2_SCRN_PND_CP_NUM(x)         (((uint32_t)(x)) >> 5)
#define VDP2_SCRN_PND_MODE_0_PAL_NUM(x) ((((uint32_t)(x)) >> 5) & 0x007F)
#define VDP2_SCRN_PND_MODE_1_PAL_NUM(x) ((((uint32_t)(x)) >> 5) & 0x007F)
#define VDP2_SCRN_PND_MODE_2_PAL_NUM(x) ((((uint32_t)(x)) >> 6) & 0x007F)

#define VDP2_SCRN_PND_PAL_NUM(cram_mode, x)                                    \
        __CONCAT(VDP2_SCRN_PND_MODE_, __CONCAT(cram_mode, _PAL_NUM))(x)

#define VDP2_SCRN_PND_CONFIG_0(cram_mode, cpd_addr, pal_addr, vf, hf)          \
        (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |       \
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFF))

#define VDP2_SCRN_PND_CONFIG_1(cram_mode, cpd_addr, pal_addr)                  \
        (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |       \
         (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x03FF))

#define VDP2_SCRN_PND_CONFIG_2(cram_mode, cpd_addr, pal_addr, vf, hf)          \
        (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |       \
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         ((VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFC) >> 2))

#define VDP2_SCRN_PND_CONFIG_3(cram_mode, cpd_addr, pal_addr)                  \
        (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |       \
         ((VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFC) >> 2))

#define VDP2_SCRN_PND_CONFIG_4(cram_mode, cpd_addr, pal_addr, vf, hf)          \
        ((((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr >> 4)) & 0x0007) << 12) |\
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFF))

#define VDP2_SCRN_PND_CONFIG_5(cram_mode, cpd_addr, pal_addr)                  \
        ((((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr >> 4)) & 0x0007) << 12) |\
         (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x03FF))

#define VDP2_SCRN_PND_CONFIG_6(cram_mode, cpd_addr, pal_addr, vf, hf)          \
        ((((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr >> 4)) & 0x0007) << 12) |\
         (((vf) & 0x01) << 11) |                                               \
         (((hf) & 0x01) << 10) |                                               \
         ((VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFC) >> 2))

#define VDP2_SCRN_PND_CONFIG_7(cram_mode, cpd_addr, pal_addr, vf, hf)          \
        ((((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr >> 4)) & 0x0007) << 12) |\
         ((VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFC) >> 2))

#define VDP2_SCRN_PND_CONFIG_8(cram_mode, cpd_addr, pal_addr, vf, hf, pr, cc)  \
        ((((vf) & 0x01) << 31) |                                               \
         (((hf) & 0x01) << 30) |                                               \
         (((pr) & 0x01) << 29) |                                               \
         (((cc) & 0x01) << 28) |                                               \
         ((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x007F) << 16) |       \
         (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x7FFF))

#define VDP2_VRAM_USAGE_TYPE_NONE       0x00
#define VDP2_VRAM_USAGE_TYPE_COEFF_TBL  0x01
#define VDP2_VRAM_USAGE_TYPE_PND        0x02
#define VDP2_VRAM_USAGE_TYPE_CPD        0x03
#define VDP2_VRAM_USAGE_TYPE_BPD        0x03

typedef struct vdp2_scrn_bitmap_format {
        vdp2_scrn_t scroll_screen; /* Normal/rotational background */
        vdp2_scrn_ccc_t cc_count; /* Character color count */

        struct {
                uint16_t width;
                uint16_t height;
        } bitmap_size; /* Bitmap sizes: 512x256, 512x512, 1024x256,
                        * 1024x512 */
        vdp2_cram_t color_palette; /* Color palette lead address (if
                                    * applicable) */
        vdp2_vram_t bitmap_pattern; /* Bitmap pattern lead address */
        vdp2_scrn_sf_type_t sf_type; /* Special function type priority
                                      * Special function type color calculation */
        vdp2_scrn_sf_code_t sf_code; /* Special function code A
                                      * Special function code B */
        uint32_t sf_mode; /* Mode 0 (per screen)
                           * Mode 1 (per cell)
                           * Mode 2 (per pixel) */
        uint8_t rp_mode; /* RBG0 and RBG1 only
                          * Rotation parameter mode
                          *   Mode 0: Rotation Parameter A
                          *   Mode 1: Rotation Parameter B
                          *   Mode 2: Swap Coefficient Data Read
                          *   Mode 3: Swap via Rotation Parameter Window */
        vdp2_vram_t rotation_table_base; /* RBG0 and RBG1 only */

        struct {
                union {
                        uint8_t a;
                        uint8_t a0;
                };

                uint8_t a1;

                union {
                        uint8_t b;
                        uint8_t b0;
                };

                uint8_t b1;
        } usage_banks; /* RBG0 and RBG1 only */
} vdp2_scrn_bitmap_format_t;

typedef struct vdp2_scrn_cell_format {
        vdp2_scrn_t scroll_screen; /* Normal/rotational background */
        vdp2_scrn_ccc_t cc_count; /* Character color count */
        uint32_t character_size; /* Character size: (1 * 1) or (2 * 2) cells */
        uint32_t pnd_size; /* Pattern name data size: (1)-word or (2)-words */
        uint32_t auxiliary_mode; /* Auxiliary mode #0 (flip function) or
                                      * auxiliary mode #1 (no flip function) */
        vdp2_vram_t cp_table; /* Character pattern table lead address*/
        vdp2_cram_t color_palette; /* Color palette lead address */
        uint32_t plane_size; /* Plane size: (1 * 1) or (2 * 1) or (2 * 2) */
        vdp2_scrn_sf_type_t sf_type; /* Special function type priority
                                      * Special function type color calculation */
        vdp2_scrn_sf_code_t sf_code; /* Special function code A
                                      * Special function code B */
        uint32_t sf_mode; /* Mode 0 (per screen)
                           * Mode 1 (per cell)
                           * Mode 2 (per pixel) */

        union {
                vdp2_vram_t planes[16];

                struct {
                        vdp2_vram_t plane_a;
                        vdp2_vram_t plane_b;
                        vdp2_vram_t plane_c;
                        vdp2_vram_t plane_d;
                        vdp2_vram_t plane_e; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_f; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_g; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_h; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_i; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_j; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_k; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_l; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_m; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_n; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_o; /* For RBG0 and RBG1 use only */
                        vdp2_vram_t plane_p; /* For RBG0 and RBG1 use only */
                } __packed;
        } map_bases; /* Map lead addresses */

        uint32_t rp_mode; /* RBG0 and RBG1 only
                               * Rotation parameter mode
                               *   Mode 0: Rotation Parameter A
                               *   Mode 1: Rotation Parameter B
                               *   Mode 2: Swap Coefficient Data Read
                               *   Mode 3: Swap via Rotation Parameter Window */
        vdp2_vram_t rotation_table; /* RBG0 and RBG1 only */

        struct {
                union {
                        uint8_t a;
                        uint8_t a0;
                };

                uint8_t a1;

                union {
                        uint8_t b;
                        uint8_t b0;
                };

                uint8_t b1;
        } usage_banks; /* RBG0 and RBG1 only */
} vdp2_scrn_cell_format_t;

/* Hardware defined */
typedef struct vdp2_scrn_rotation_table {
        /* Screen start coordinates */
        uint32_t xst;
        uint32_t yst;
        uint32_t zst;

        /* Screen vertical coordinate increments (per each line) */
        uint32_t delta_xst;
        uint32_t delta_yst;

        /* Screen horizontal coordinate increments (per each dot) */
        uint32_t delta_x;
        uint32_t delta_y;

        /* Rotation matrix
         *
         * A B C
         * D E F
         * G H I
         */
        union {
                uint32_t raw[2][3]; /* XXX: Needs to be tested */

                struct {
                        uint32_t a;
                        uint32_t b;
                        uint32_t c;
                        uint32_t d;
                        uint32_t e;
                        uint32_t f;
                } param __packed;

                uint32_t params[6]; /* Parameters A, B, C, D, E, F */
        } matrix;

        /* View point coordinates */
        uint16_t px;
        uint16_t py;
        uint16_t pz;

        unsigned int :16;

        /* Center coordinates */
        uint16_t cx;
        uint16_t cy;
        uint16_t cz;

        unsigned int :16;

        /* Amount of horizontal shifting */
        uint32_t mx;
        uint32_t my;

        /* Scaling coefficients */
        uint32_t kx; /* Expansion/reduction coefficient X */
        uint32_t ky; /* Expansion/reduction coefficient Y */

        /* Coefficient table address */
        uint32_t kast;       /* Coefficient table start address */
        uint32_t delta_kast; /* Addr. increment coeff. table (per line) */
        uint32_t delta_kax;  /* Addr. increment coeff. table (per dot) */
} __packed vdp2_scrn_rotation_table_t;

/*-
 * Limitations for NBG0/NBG1 reduction:
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
 * The coord. increment should be a value smaller then 1 to zoom in and larger
 * than 1 to zoom out. No zoom means equal to 1.
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

#define VDP2_SCRN_REDUCTION_STEP        Q0_3_8(1.0f / 256.0f)
#define VDP2_SCRN_REDUCTION_MIN         VDP2_SCRN_REDUCTION_STEP
#define VDP2_SCRN_REDUCTION_MAX         Q0_3_8(7.0f)

typedef struct vdp2_scrn_ls_format {
        vdp2_scrn_t scroll_screen; /* Normal background */
        vdp2_vram_t line_scroll_table; /* Line scroll table (lead addr.) */
        uint8_t interval; /* Dependent on the interlace setting */
        vdp2_scrn_ls_enable_t enable; /* Enable line scroll */
} vdp2_scrn_ls_format_t;

typedef struct vdp2_scrn_ls_h {
        fix16_t horz;
} __packed vdp2_scrn_ls_h_t;

typedef struct vdp2_scrn_ls_hv {
        fix16_t horz;
        fix16_t vert;
} __packed vdp2_scrn_ls_hv_t;

typedef struct vdp2_scrn_ls_hvz {
        fix16_t horz;
        fix16_t vert;
        fix16_t horz_incr;
} __packed vdp2_scrn_ls_hvz_t;

typedef struct vdp2_scrn_vcs_format {
        vdp2_scrn_t scroll_screen; /* Normal background */
        vdp2_vram_t vcs_table; /* Vertical cell scroll table (lead addr.) */
} vdp2_scrn_vcs_format_t;

extern void vdp2_scrn_back_screen_color_set(vdp2_vram_t, const color_rgb1555_t);
extern void vdp2_scrn_back_screen_buffer_set(vdp2_vram_t, const color_rgb1555_t *,
    const uint16_t);

extern void vdp2_scrn_bitmap_format_set(const vdp2_scrn_bitmap_format_t *);

extern void vdp2_scrn_cell_format_set(const vdp2_scrn_cell_format_t *);

extern void vdp2_scrn_color_offset_clear(void);
extern void vdp2_scrn_color_offset_rgb_set(vdp2_scrn_color_offset_t, vdp2_color8_t, vdp2_color8_t, vdp2_color8_t);
extern void vdp2_scrn_color_offset_set(vdp2_scrn_t, vdp2_scrn_color_offset_t);
extern void vdp2_scrn_color_offset_unset(vdp2_scrn_t);

extern void vdp2_scrn_display_set(vdp2_scrn_t, bool);
extern void vdp2_scrn_display_unset(vdp2_scrn_t);
extern void vdp2_scrn_display_clear(void);

extern void vdp2_scrn_ls_set(const vdp2_scrn_ls_format_t *);

extern void vdp2_scrn_vcs_set(const vdp2_scrn_vcs_format_t *);
extern void vdp2_scrn_vcs_unset(vdp2_scrn_t);
extern void vdp2_scrn_vcs_clear(void);

extern void vdp2_scrn_mosaic_set(vdp2_scrn_t);
extern void vdp2_scrn_mosaic_unset(vdp2_scrn_t);
extern void vdp2_scrn_mosaic_clear(void);
extern void vdp2_scrn_mosaic_horizontal_set(uint8_t);
extern void vdp2_scrn_mosaic_vertical_set(uint8_t);

extern void vdp2_scrn_priority_set(vdp2_scrn_t, uint8_t);
extern uint8_t vdp2_scrn_priority_get(vdp2_scrn_t);

extern void vdp2_scrn_reduction_set(vdp2_scrn_t, vdp2_scrn_reduction_t);
extern void vdp2_scrn_reduction_x_set(vdp2_scrn_t, q0_3_8_t);
extern void vdp2_scrn_reduction_y_set(vdp2_scrn_t, q0_3_8_t);

extern void vdp2_scrn_scroll_x_set(vdp2_scrn_t, fix16_t);
extern void vdp2_scrn_scroll_x_update(vdp2_scrn_t, fix16_t);
extern void vdp2_scrn_scroll_y_set(vdp2_scrn_t, fix16_t);
extern void vdp2_scrn_scroll_y_update(vdp2_scrn_t, fix16_t);

extern void vdp2_scrn_sf_codes_set(vdp2_scrn_sf_code_t,
    vdp2_scrn_sf_code_range_t);

__END_DECLS

#endif /* !_VDP2_SCRN_H_ */
