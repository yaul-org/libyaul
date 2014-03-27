/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _SCRN_H_
#define _SCRN_H_

#include <stdbool.h>
#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SCRN_NBG0       0       /* Normal background (NBG0) */
#define SCRN_RBG1       5       /* Rotational background (RBG1) */
#define SCRN_NBG1       1       /* Normal background (NBG1) */
#define SCRN_NBG2       2       /* Normal background (NBG2) */
#define SCRN_NBG3       3       /* Normal background (NBG3) */
#define SCRN_RBG0       4       /* Rotational background (RBG0) */

enum scrn_rp_mode_type {
        SCRN_RP_MODE_0,         /* Rotation Parameter A */
        SCRN_RP_MODE_1,         /* Rotation Parameter B */
        SCRN_RP_MODE_2,         /* Swap Coefficient Data Read */
        SCRN_RP_MODE_3          /* Swap via Rotation Parameter Window */
};

struct scrn_bm_format {
        uint8_t bm_scrn;        /* Normal/rotational background */
        uint32_t bm_ccc;        /* Character color count */

#define SCRN_BM_BMSZ_512_256    0 /* Bitmap size 512x256 */
#define SCRN_BM_BMSZ_512_512    1 /* Bitmap size 512x512 */
#define SCRN_BM_BMSZ_1024_256   2 /* Bitmap size 1024x256 */
#define SCRN_BM_BMSZ_1024_512   3 /* Bitmap size 1024x512 */
        uint8_t bm_bs;          /* Bitmap size */
        uint8_t bm_sp;          /* Special priority */
        uint8_t bm_scc;         /* Special color calculation */
        uint8_t bm_spn;         /* Supplementary palette number */
        uint32_t bm_pb;         /* Bitmap pattern boundry (lead addr.) */
};

struct scrn_cell_format {
        uint8_t scf_scrn;       /* Normal/rotational background */
        uint32_t scf_cc_count;  /* Character color count */
        uint8_t scf_character_size; /* Character size: (1 * 1) or (2 * 2) cells */
        uint8_t scf_pnd_size;   /* Pattern name data size: (1)-word or (2)-words */
        uint8_t scf_auxiliary_mode; /* Auxiliary mode #0 (flip function) or
                                     * auxiliary mode #1 (no flip function) */
        uint32_t scf_cp_table;  /* Character pattern table */
        uint8_t scf_plane_size; /* Plane size: (1 * 1) or (2 * 1) or (2 * 2) */

        struct {
                uint32_t plane_a;
                uint32_t plane_b;
                uint32_t plane_c;
                uint32_t plane_d;
        } scf_map;              /* Map lead addresses */
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

extern void vdp2_scrn_bm_format_set(struct scrn_bm_format *);
extern void vdp2_scrn_cell_format_set(struct scrn_cell_format *);
extern void vdp2_scrn_display_set(uint8_t, bool);
extern void vdp2_scrn_display_clear(uint8_t, bool);
extern void vdp2_scrn_ls_set(struct scrn_ls_format *);
extern void vdp2_scrn_scv_x_set(uint8_t, uint16_t, uint8_t);
extern void vdp2_scrn_scv_y_set(uint8_t, uint16_t, uint8_t);
extern void vdp2_scrn_mosaic_set(uint8_t, uint8_t, uint8_t);

extern void vdp2_scrn_rp_mode_set(enum scrn_rp_mode_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_SCRN_H_ */
