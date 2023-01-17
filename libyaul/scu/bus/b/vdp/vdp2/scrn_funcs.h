/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP2_SCRN_FUNCS_H_
#define _YAUL_VDP2_SCRN_FUNCS_H_

#include <assert.h>
#include <math.h>
#include <stdint.h>

#include <vdp2/scrn_shared.h>

#include <vdp2/vram.h>

__BEGIN_DECLS

typedef enum vdp2_scrn_ls_type {
        /// Scrolls horizontally per line units.
        VDP2_SCRN_LS_TYPE_HORZ      = 0x01,
        /// Scrolls vertically per line units.
        VDP2_SCRN_LS_TYPE_VERT      = 0x02,
        /// Scales horizontally per line units.
        VDP2_SCRN_LS_TYPE_ZOOM_HORZ = 0x04
} vdp2_scrn_ls_type_t;

typedef enum vdp2_scrn_coff {
        /// Not yet documented.
        VDP2_SCRN_COFF_A = 0,
        /// Not yet documented.
        VDP2_SCRN_COFF_B = 1
} vdp2_scrn_coff_t;

typedef enum vdp2_scrn_reduction {
        /// No reduction
        VDP2_SCRN_REDUCTION_NONE    = 0,
        /// 1/2 reduction
        VDP2_SCRN_REDUCTION_HALF    = 1,
        /// 1/4 reduction
        VDP2_SCRN_REDUCTION_QUARTER = 2
} vdp2_scrn_reduction_t;

typedef enum vdp2_scrn_sf_mode {
        /// Not yet documented.
        VDP2_SCRN_SF_MODE_0 = 0,
        /// Not yet documented.
        VDP2_SCRN_SF_MODE_1 = 1,
        /// Not yet documented.
        VDP2_SCRN_SF_MODE_2 = 2,
} __packed vdp2_scrn_sf_mode_t;

typedef enum vdp2_scrn_sf_type {
        /// Not yet documented.
        VDP2_SCRN_SF_TYPE_NONE            = 0,
        /// Not yet documented.
        VDP2_SCRN_SF_TYPE_CELL_CC         = 0x10,
        /// Not yet documented.
        VDP2_SCRN_SF_TYPE_CELL_PRIORITY   = 0x20,
        /// Not yet documented.
        VDP2_SCRN_SF_TYPE_BITMAP_CC       = 0x18,
        /// Not yet documented.
        VDP2_SCRN_SF_TYPE_BITMAP_PRIORITY = 0x28
} __packed vdp2_scrn_sf_type_t;

typedef enum vdp2_scrn_sf_code {
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_A = 0,
        /// Not yet documented.
        VDP2_SCRN_SF_CODE_B = 1
} __packed vdp2_scrn_sf_code_t;

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
} __packed vdp2_scrn_sf_code_range_t;

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

#define VDP2_SCRN_REDUCTION_STEP FIX16(1.0f / 256.0f)
#define VDP2_SCRN_REDUCTION_MIN  VDP2_SCRN_REDUCTION_STEP
#define VDP2_SCRN_REDUCTION_MAX  FIX16(7.0f)

typedef struct vdp2_scrn_ls_format {
        /// Scroll screen.
        vdp2_scrn_t scroll_screen;
        /// LIne scroll table base address.
        vdp2_vram_t table_base;
        /// Not yet documented. Dependent on the interlace setting.
        uint8_t interval;
        /// Not yet documented.
        vdp2_scrn_ls_type_t type;
} vdp2_scrn_ls_format_t;

typedef struct vdp2_scrn_ls_h {
        fix16_t horz;
} __packed vdp2_scrn_ls_h_t;

typedef struct vdp2_scrn_ls_v {
        fix16_t vert;
} __packed vdp2_scrn_ls_v_t;

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
        /// Scroll screen.
        vdp2_scrn_t scroll_screen;
        /// Vertical cell scroll table base address.
        vdp2_vram_t table_base;
} vdp2_scrn_vcs_format_t;

typedef struct vdp2_scrn_coff_rgb {
        int16_t r;
        int16_t g;
        int16_t b;
} vdp2_scrn_coff_rgb_t;

extern void vdp2_scrn_lncl_set(vdp2_scrn_t scrn_mask);
extern void vdp2_scrn_lncl_color_set(vdp2_vram_t vram,
    uint16_t cram_index);
extern void vdp2_scrn_lncl_buffer_set(vdp2_vram_t vram,
    const uint16_t *buffer, uint32_t count);
extern void vdp2_scrn_lncl_sync(void);

extern void vdp2_scrn_coff_clear(void);
extern void vdp2_scrn_coff_rgb_set(vdp2_scrn_coff_t select,
    const vdp2_scrn_coff_rgb_t *rgb);
extern void vdp2_scrn_coff_set(vdp2_scrn_t scroll_screen,
    vdp2_scrn_coff_t select);
extern void vdp2_scrn_coff_unset(vdp2_scrn_t scroll_screen);

extern void vdp2_scrn_ls_set(const vdp2_scrn_ls_format_t *ls_format);

extern void vdp2_scrn_vcs_set(const vdp2_scrn_vcs_format_t *vcs_format);
extern void vdp2_scrn_vcs_unset(vdp2_scrn_t scroll_screen);
extern void vdp2_scrn_vcs_clear(void);

extern void vdp2_scrn_mosaic_set(vdp2_scrn_t scrn_mask);
extern void vdp2_scrn_mosaic_horizontal_set(uint8_t horizontal);
extern void vdp2_scrn_mosaic_vertical_set(uint8_t vertical);

extern void vdp2_scrn_priority_set(vdp2_scrn_t scroll_screen, uint8_t priority);
extern uint8_t vdp2_scrn_priority_get(vdp2_scrn_t scroll_screen);

extern void vdp2_scrn_reduction_set(vdp2_scrn_t scroll_screen,
    vdp2_scrn_reduction_t reduction);
extern void vdp2_scrn_reduction_x_set(vdp2_scrn_t scroll_screen, fix16_t scale);
extern void vdp2_scrn_reduction_y_set(vdp2_scrn_t scroll_screen, fix16_t scale);
extern void vdp2_scrn_reduction_xy_set(vdp2_scrn_t scroll_screen,
    const fix16_vec2_t *scale);

extern void vdp2_scrn_sf_set(vdp2_scrn_t scroll_screen, uint32_t sf_mode,
    vdp2_scrn_sf_type_t sf_type, vdp2_scrn_sf_code_t sf_code);
extern void vdp2_scrn_sf_codes_set(vdp2_scrn_sf_code_t code,
    vdp2_scrn_sf_code_range_t code_range);

__END_DECLS

#endif /* !_YAUL_VDP2_SCRN_FUNCS_H_ */
