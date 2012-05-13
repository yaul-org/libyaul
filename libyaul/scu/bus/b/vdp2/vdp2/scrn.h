/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _SCRN_H_
#define _SCRN_H_

#include <stdbool.h>
#include <inttypes.h>

enum scrn_type {
        SCRN_NBG0,
        SCRN_NBG1,
        SCRN_NBG2,
        SCRN_NBG3,
        SCRN_RBG0,
        SCRN_RBG1
};

enum scrn_rp_mode_type {
        SCRN_RP_MODE_0,         /* Rotation Parameter A */
        SCRN_RP_MODE_1,         /* Rotation Parameter B */
        SCRN_RP_MODE_2,         /* Swap Coefficient Data Read */
        SCRN_RP_MODE_3          /* Swap via Rotation Parameter Window */
};

struct scrn_bm_format {
        enum scrn_type ch_scrn; /* Normal background */
        uint8_t bm_bs;          /* Bitmap size */
        uint8_t bm_sp;          /* Special priority */
        uint8_t bm_scc;         /* Supplementary character number */
        uint8_t bm_spn;         /* Supplementary palette number */
};

struct scrn_ch_format {
        enum scrn_type ch_scrn; /* Normal background */
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
        uint8_t ch_pri;         /* Priority [0..7] When priority is zero, scroll screen is transparent. */

#define SCRN_CH_CHC_16          0 /* Palette Format */
#define SCRN_CH_CHC_256         1 /* Palette Format */
#define SCRN_CH_CHC_2048        2 /* Palette Format */
#define SCRN_CH_CHC_32768       3 /* RGB Format */
#define SCRN_CH_CHC_16770000    4 /* RGB Format */
        uint8_t ch_chc;         /* Character color */
};

struct scrn_ls_format {
        enum scrn_type ls_scrn; /* Normal background */
        uint32_t ls_lsta;       /* Line scroll table (lead addr.) */
        uint8_t ls_int;         /* Dependent on the interlace setting */

#define SCRN_LS_N0SCX   0x0002
#define SCRN_LS_N1SCX   0x0200
#define SCRN_LS_N0SCY   0x0004
#define SCRN_LS_N1SCY   0x0400
        uint16_t ls_fun;        /* Enable line scroll */
};

struct scrn_vcs_format {
        enum scrn_type vcs_scrn; /* Normal background */
        uint32_t vcs_vcsta;     /* Vertical cell scroll table (lead addr.) */
};

extern void vdp2_scrn_bm_format_set(enum scrn_type, struct scrn_bm_format *);
extern void vdp2_scrn_ch_format_set(struct scrn_ch_format *);
extern void vdp2_scrn_ls_set(struct scrn_ls_format *);

extern void vdp2_scrn_display_set(enum scrn_type, bool);
extern void vdp2_scrn_rp_mode_set(enum scrn_rp_mode_type);

#endif /* !_SCRN_H_ */
