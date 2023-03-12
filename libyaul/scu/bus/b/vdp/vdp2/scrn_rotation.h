/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP2_SCRN_ROTATION_H_
#define _YAUL_VDP2_SCRN_ROTATION_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <math.h>
#include <stdint.h>

#include <vdp2/scrn_shared.h>
#include <vdp2/scrn_bitmap.h>
#include <vdp2/scrn_cell.h>

#include <vdp2/vram.h>

__BEGIN_DECLS

typedef enum vdp2_scrn_rp_mode {
    /// Rotation Parameter A.
    VDP2_SCRN_RP_MODE_0,
    /// Rotation Parameter B.
    VDP2_SCRN_RP_MODE_1,
    /// Swap coefficient data read.
    VDP2_SCRN_RP_MODE_2,
    /// Swap via rotation parameter window.
    VDP2_SCRN_RP_MODE_3
} __packed vdp2_scrn_rp_mode_t;

typedef enum vdp2_scrn_rsop_type {
    /// Not yet documented.
    VDP2_SCRN_RSOP_TYPE_REPEAT,
    /// Not yet documented.
    VDP2_SCRN_RSOP_TYPE_CELL_REPEAT,
    /// Not yet documented.
    VDP2_SCRN_RSOP_TYPE_DISPLAY_TRANSPARENT,
    /// Not yet documented.
    VDP2_SCRN_RSOP_TYPE_TRANSPARENT
} __packed vdp2_scrn_rsop_type_t;

typedef enum vdp2_scrn_coeff_usage {
    VDP2_SCRN_COEFF_USAGE_KX_KY         = 0x00,
    VDP2_SCRN_COEFF_USAGE_KX            = 0x04,
    VDP2_SCRN_COEFF_USAGE_KY            = 0x08,
    VDP2_SCRN_COEFF_USAGE_XP_CONVERSION = 0x0C
} __packed vdp2_scrn_coeff_usage_t;

typedef enum vdp2_scrn_coeff_word_size {
    VDP2_SCRN_COEFF_WORD_SIZE_1 = 0x02,
    VDP2_SCRN_COEFF_WORD_SIZE_2 = 0x00
} __packed vdp2_scrn_coeff_word_size_t;

typedef struct vdp2_scrn_coeff_params {
    vdp2_scrn_coeff_usage_t usage;
    vdp2_scrn_coeff_word_size_t word_size;

    unsigned int :3;
    unsigned int xst_read_enable:1;
    unsigned int yst_read_enable:1;
    unsigned int kast_read_enable:1;
    /// Line color screen data is only available when @p
    /// vdp2_scrn_coeff_params_t::word size is 2.
    unsigned int lncl_enable:1;
    /// Eanble coefficient table.
    unsigned int enable:1;
} __packed __aligned(4) vdp2_scrn_coeff_params_t;

static_assert(sizeof(vdp2_scrn_coeff_params_t) == 4);

/// Not yet documented.
typedef struct vdp2_scrn_rp_table {
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
    vdp2_vram_t kast;    /* Coefficient table start address */
    uint32_t delta_kast; /* Addr. increment coeff. table (per line) */
    uint32_t delta_kax;  /* Addr. increment coeff. table (per dot) */
} __packed vdp2_scrn_rp_table_t;

/// Not yet documented.
typedef struct vdp2_scrn_rotation_params {
    /// Rotation parameter mode.
    vdp2_scrn_rp_mode_t rp_mode;

    /// Screen-over process type.
    vdp2_scrn_rsop_type_t rsop_type;
    // XXX: Needs the screen-over process pattern name?

    /// Coefficient table parameters.
    vdp2_scrn_coeff_params_t coeff_params;

    /// Pointer to rotation parameter.
    const vdp2_scrn_rp_table_t *rp_table;

    /// Rotation parameter table base address.
    vdp2_vram_t rp_table_base;
} __aligned(4) vdp2_scrn_rotation_params_t;

/// Not yet documented.
extern void vdp2_scrn_rotation_cell_format_set(const vdp2_scrn_cell_format_t *cell_format,
  const vdp2_scrn_rotation_map_t *rotation_map);

/// Not yet documented.
extern void vdp2_scrn_rotation_sop_set(const vdp2_scrn_rotation_params_t *rotation_params);

extern void vdp2_scrn_rotation_rp_mode_set(const vdp2_scrn_rotation_params_t *rotation_params);
/// Not yet documented.
extern void vdp2_scrn_rotation_rp_table_set(const vdp2_scrn_rotation_params_t *rotation_params);
/// Not yet documented.
extern void vdp2_scrn_rotation_rp_read_set(const vdp2_scrn_rotation_params_t *rotation_params);

extern void vdp2_scrn_rotation_coeff_params_set(const vdp2_scrn_rotation_params_t *rotation_params);
/// Not yet documented.
extern void vdp2_scrn_rotation_coeff_table_set(const vdp2_scrn_rotation_params_t *rotation_params);

__END_DECLS

#endif /* !_YAUL_VDP2_SCRN_ROTATION_H_ */
