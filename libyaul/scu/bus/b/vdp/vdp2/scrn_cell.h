/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP2_SCRN_CELL_H_
#define _YAUL_VDP2_SCRN_CELL_H_

#include <assert.h>
#include <math.h>
#include <stdint.h>

#include <vdp2/scrn_shared.h>
#include <vdp2/scrn_macros.h>

#include <vdp2/map.h>
#include <vdp2/vram.h>

__BEGIN_DECLS

typedef enum vdp2_scrn_char_size {
    VDP2_SCRN_CHAR_SIZE_1X1,
    VDP2_SCRN_CHAR_SIZE_2X2
} __packed vdp2_scrn_char_size_t;

typedef enum vdp2_scrn_plane_size {
    VDP2_SCRN_PLANE_SIZE_1X1 = 1 * 1,
    VDP2_SCRN_PLANE_SIZE_2X1 = 2 * 1,
    VDP2_SCRN_PLANE_SIZE_2X2 = 2 * 2
} __packed vdp2_scrn_plane_size_t;

typedef enum vdp2_scrn_plane {
    VDP2_SCRN_PLANE_A,
    VDP2_SCRN_PLANE_B,
    VDP2_SCRN_PLANE_C,
    VDP2_SCRN_PLANE_D,
    VDP2_SCRN_PLANE_E,
    VDP2_SCRN_PLANE_F,
    VDP2_SCRN_PLANE_G,
    VDP2_SCRN_PLANE_H,
    VDP2_SCRN_PLANE_I,
    VDP2_SCRN_PLANE_J,
    VDP2_SCRN_PLANE_K,
    VDP2_SCRN_PLANE_L,
    VDP2_SCRN_PLANE_M,
    VDP2_SCRN_PLANE_N,
    VDP2_SCRN_PLANE_O,
    VDP2_SCRN_PLANE_P
} vdp2_scrn_plane_t;

typedef enum vdp2_scrn_aux_mode {
    /// Auxiliary mode #0 (flip function).
    VDP2_SCRN_AUX_MODE_0,
    /// Auxiliary mode #1 (no flip function).
    VDP2_SCRN_AUX_MODE_1
} vdp2_scrn_aux_mode_t;

typedef struct vdp2_scrn_cell_format {
    /// Not yet documented.
    vdp2_scrn_t scroll_screen;

    /// Not yet documented.
    vdp2_scrn_ccc_t ccc;
    /// Not yet documented.
    vdp2_scrn_char_size_t char_size;
    /// (1)-word or (2)-words.
    uint8_t pnd_size;
    /// Not yet documented.
    vdp2_scrn_aux_mode_t aux_mode;

    /// Not yet documented.
    vdp2_scrn_plane_size_t plane_size;

    /// Not yet documented.
    vdp2_vram_t cpd_base;
    /// Not yet documented.
    vdp2_cram_t palette_base;
} __packed __aligned(4) vdp2_scrn_cell_format_t;

static_assert(sizeof(vdp2_scrn_cell_format_t) == 20);

typedef union vdp2_scrn_normal_map {
    vdp2_vram_t base_addr[4];

    struct {
        vdp2_vram_t plane_a;
        vdp2_vram_t plane_b;
        vdp2_vram_t plane_c;
        vdp2_vram_t plane_d;
    } __packed;
} __aligned(4) vdp2_scrn_normal_map_t;

typedef struct vdp2_scrn_rotation_map {
    union {
        vdp2_vram_t base_addr[16];

        struct {
            union {
                vdp2_vram_t plane;
                vdp2_vram_t plane_a;
            };

            vdp2_vram_t plane_b;
            vdp2_vram_t plane_c;
            vdp2_vram_t plane_d;
            vdp2_vram_t plane_e;
            vdp2_vram_t plane_f;
            vdp2_vram_t plane_g;
            vdp2_vram_t plane_h;
            vdp2_vram_t plane_i;
            vdp2_vram_t plane_j;
            vdp2_vram_t plane_k;
            vdp2_vram_t plane_l;
            vdp2_vram_t plane_m;
            vdp2_vram_t plane_n;
            vdp2_vram_t plane_o;
            vdp2_vram_t plane_p;
        } __packed;
    } __packed;

    bool single;
} __aligned(4) vdp2_scrn_rotation_map_t;

extern void vdp2_scrn_cell_format_set(const vdp2_scrn_cell_format_t *cell_format,
  const vdp2_scrn_normal_map_t *normal_map);
extern void vdp2_scrn_cell_ccc_set(const vdp2_scrn_cell_format_t *cell_format);
extern void vdp2_scrn_char_size_set(const vdp2_scrn_cell_format_t *cell_format);
extern void vdp2_scrn_plane_size_set(const vdp2_scrn_cell_format_t *cell_format);
extern void vdp2_scrn_map_set(const vdp2_scrn_cell_format_t *cell_format,
  vdp2_scrn_plane_t plane, const vdp2_scrn_normal_map_t *normal_map);
extern void vdp2_scrn_map_plane_set(const vdp2_scrn_cell_format_t *cell_format,
  vdp2_scrn_plane_t plane, vdp2_vram_t plane_base);
extern void vdp2_scrn_pnd_set(const vdp2_scrn_cell_format_t *cell_format);

__END_DECLS

#endif /* !_YAUL_VDP2_SCRN_CELL_H_ */
