/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
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

#define SCRN_CCC_CHC_16         0 /* Palette Format */
#define SCRN_CCC_CHC_256        1 /* Palette Format */
#define SCRN_CCC_CHC_2048       2 /* Palette Format */
#define SCRN_CCC_CHC_32768      3 /* RGB Format */
#define SCRN_CCC_CHC_16770000   4 /* RGB Format */

struct scrn_bm_format {
        uint8_t bm_scrn;        /* Normal/rotational background */

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

struct scrn_ch_format {
        uint8_t ch_scrn;        /* Normal/rotational background */
        uint8_t ch_cs;          /* Character size: (1 * 1) or (2 * 2) cells */
        uint8_t ch_pnds;        /* Pattern name data size: (1)-word or (2)-words */
        uint8_t ch_cnsm;        /* Character number supplementary mode: mode (0) or mode (1) */
        uint8_t ch_sp;          /* Special priority */
        uint8_t ch_scc;         /* Special color calculation */
        uint8_t ch_spn;         /* Supplementary palette number */
        uint32_t ch_scn;        /* Supplementary character number (lead addr.) */
        uint8_t ch_pls;         /* Plane size: (1 * 1) or (2 * 1) or (2 * 2) */
        uint32_t ch_map[4];     /* Map lead addresses */
        uint8_t ch_mapofs;      /* Map offset */
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
extern void vdp2_scrn_ccc_set(uint8_t, uint8_t);
extern void vdp2_scrn_ch_format_set(struct scrn_ch_format *);
extern void vdp2_scrn_display_set(uint8_t, bool);
extern void vdp2_scrn_ls_set(struct scrn_ls_format *);
extern void vdp2_scrn_scv_x_set(uint8_t, uint16_t, uint8_t);
extern void vdp2_scrn_scv_y_set(uint8_t, uint16_t, uint8_t);
extern void vdp2_scrn_mosaic_set(uint8_t, uint8_t, uint8_t);

extern void vdp2_scrn_rp_mode_set(enum scrn_rp_mode_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_SCRN_H_ */
