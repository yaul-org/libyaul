/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP2_SCRN_MACROS_H_
#define _YAUL_VDP2_SCRN_MACROS_H_

#include <stdbool.h>
#include <stdint.h>

#include <vdp2/vram.h>

__BEGIN_DECLS

#define VDP2_SCRN_PAGE_COUNT_CALCULATE(format)                                 \
    ((format)->plane_size)

#define VDP2_SCRN_PAGE_WIDTH_CALCULATE(format)                                 \
    (((format)->char_size == VDP2_SCRN_CHAR_SIZE_1X1) ? 64 : 32)

#define VDP2_SCRN_PAGE_HEIGHT_CALCULATE(format)                                \
    (((format)->char_size == VDP2_SCRN_CHAR_SIZE_1X1) ? 64 : 32)

#define VDP2_SCRN_PAGE_DIMENSION_CALCULATE(format)                             \
    (((format)->char_size == VDP2_SCRN_CHAR_SIZE_1X1)                          \
        ? (64 * 64)                                                            \
        : (32 * 32))

#define VDP2_SCRN_PAGE_COUNT_M_CALCULATE(plane_size)                           \
    (plane_size)

#define VDP2_SCRN_PAGE_WIDTH_M_CALCULATE(char_size)                            \
    (((char_size) == VDP2_SCRN_CHAR_SIZE_1X1) ? 64 : 32)

#define VDP2_SCRN_PAGE_HEIGHT_M_CALCULATE(char_size)                           \
    (((char_size) == VDP2_SCRN_CHAR_SIZE_1X1) ? 64 : 32)

#define VDP2_SCRN_PAGE_DIMENSION_M_CALCULATE(char_size)                        \
    (((char_size) == VDP2_SCRN_CHAR_SIZE_1X1) ? (64 * 64) : (32 * 32))

/*-
 * Possible values for VDP2_SCRN_PAGE_SIZE_CALCULATE() (in bytes):
 * +----------+-----------+---------------+
 * | PND size | Cell size | Size of page  |
 * +----------+-----------+---------------+
 * | 1-word   | 2x2       | 0x0800        |
 * | 2-word   | 2x2       | 0x1000        |
 * | 1-word   | 1x1       | 0x2000        |
 * | 2-word   | 1x1       | 0x4000        |
 * +----------+-----------+---------------+
 *
 * Page dimension is 64x64 if cell size is 1x1.
 * Page dimension is 32x32 if cell size is 2x2 */
#define VDP2_SCRN_PAGE_SIZE_CALCULATE(format)                                  \
    (VDP2_SCRN_PAGE_DIMENSION_CALCULATE(format) * ((format)->pnd_size * 2))

#define VDP2_SCRN_PAGE_SIZE_M_CALCULATE(char_size, pnd_size)                   \
    (VDP2_SCRN_PAGE_DIMENSION_M_CALCULATE(char_size) * ((pnd_size) * 2))

/*-
 * Possible vales for VDP2_SCRN_PLANE_SIZE_CALCULATE() (in bytes):
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

#define VDP2_SCRN_PLANE_SIZE_CALCULATE(format)                                 \
    (((format)->plane_size) * VDP2_SCRN_PAGE_SIZE_CALCULATE(format))

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

#define VDP2_SCRN_PLANES_2_CNT_CALCULATE(format)                               \
    (VDP2_VRAM_BSIZE_2 / VDP2_SCRN_PLANE_SIZE_CALCULATE((format)))

#define VDP2_SCRN_PLANES_4_CNT_CALCULATE(format)                               \
    (VDP2_VRAM_BSIZE_4 / VDP2_SCRN_PLANE_SIZE_CALCULATE((format)))

/*-
 * Configuration table mapping. Depending on how the normal/rotational
 * background is set up, choose a config:
 * +-----+--------+------+-----------+------------+------------+-------------+
 * | CFG | PND    | Cell | Aux. mode | Page size  | Pal. banks | Color count |
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

#define VDP2_SCRN_PND_CP_NUM(addr)              (((uint32_t)(addr)) >> 5)
#define VDP2_SCRN_PND_MODE_0_PAL_NUM(cram_addr) ((((uint32_t)(cram_addr)) >> 5) & 0x007F)
#define VDP2_SCRN_PND_MODE_1_PAL_NUM(cram_addr) ((((uint32_t)(cram_addr)) >> 5) & 0x007F)
#define VDP2_SCRN_PND_MODE_2_PAL_NUM(cram_addr) ((((uint32_t)(cram_addr)) >> 6) & 0x007F)

#define VDP2_SCRN_PND_PAL_NUM(cram_mode, cram_addr)                            \
    __CONCAT(VDP2_SCRN_PND_MODE_, __CONCAT(cram_mode, _PAL_NUM))(cram_addr)

#define VDP2_SCRN_PND_CONFIG_0(cram_mode, cpd_addr, pal_addr, vf, hf)          \
    (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |           \
     (((vf) & 0x01) << 11) |                                                   \
     (((hf) & 0x01) << 10) |                                                   \
     (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x03FF))

#define VDP2_SCRN_PND_CONFIG_1(cram_mode, cpd_addr, pal_addr)                  \
    (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |           \
     (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFF))

#define VDP2_SCRN_PND_CONFIG_2(cram_mode, cpd_addr, pal_addr, vf, hf)          \
    (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |           \
     (((vf) & 0x01) << 11) |                                                   \
     (((hf) & 0x01) << 10) |                                                   \
     ((VDP2_SCRN_PND_CP_NUM(cpd_addr) >> 2) & 0x03FF))

#define VDP2_SCRN_PND_CONFIG_3(cram_mode, cpd_addr, pal_addr)                  \
    (((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x000F) << 12) |           \
     ((VDP2_SCRN_PND_CP_NUM(cpd_addr) >> 2) & 0x0FFF))

#define VDP2_SCRN_PND_CONFIG_4(cram_mode, cpd_addr, pal_addr, vf, hf)          \
    ((((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr >> 4)) & 0x0007) << 12) |    \
     (((vf) & 0x01) << 11) |                                                   \
     (((hf) & 0x01) << 10) |                                                   \
     (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x03FF))

#define VDP2_SCRN_PND_CONFIG_5(cram_mode, cpd_addr, pal_addr)                  \
    ((((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr >> 4)) & 0x0007) << 12) |    \
     (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x0FFF))

#define VDP2_SCRN_PND_CONFIG_6(cram_mode, cpd_addr, pal_addr, vf, hf)          \
    ((((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr >> 4)) & 0x0007) << 12) |    \
     (((vf) & 0x01) << 11) |                                                   \
     (((hf) & 0x01) << 10) |                                                   \
     ((VDP2_SCRN_PND_CP_NUM(cpd_addr) >> 2) & 0x03FF))

#define VDP2_SCRN_PND_CONFIG_7(cram_mode, cpd_addr, pal_addr, vf, hf)          \
    ((((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr >> 4)) & 0x0007) << 12) |    \
     ((VDP2_SCRN_PND_CP_NUM(cpd_addr) >> 2) & 0x0FFF))

#define VDP2_SCRN_PND_CONFIG_8(cram_mode, cpd_addr, pal_addr, vf, hf, pr, cc)  \
    ((((vf) & 0x01) << 31) |                                                   \
     (((hf) & 0x01) << 30) |                                                   \
     (((pr) & 0x01) << 29) |                                                   \
     (((cc) & 0x01) << 28) |                                                   \
     ((VDP2_SCRN_PND_PAL_NUM(cram_mode, pal_addr) & 0x007F) << 16) |           \
     (VDP2_SCRN_PND_CP_NUM(cpd_addr) & 0x7FFF))

__END_DECLS

#endif /* !_YAUL_VDP2_SCRN_MACROS_H_ */
