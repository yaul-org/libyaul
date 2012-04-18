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

enum scrn_ch_color_type {
        SCRN_CHC_16,            /* Palette Format */
        SCRN_CHC_256,           /* Palette Format */
        SCRN_CHC_2048,          /* Palette Format */
        SCRN_CHC_32768,         /* RGB Format */
        SCRN_CHC_16770000       /* RGB Format */
};

enum scrn_rp_mode_type {
        SCRN_RP_MODE_0,         /* Rotation Parameter A */
        SCRN_RP_MODE_1,         /* Rotation Parameter B */
        SCRN_RP_MODE_2,         /* Swap Coefficient Data Read */
        SCRN_RP_MODE_3          /* Swap via Rotation Parameter Window */
};

struct scrn_bm_format {
        enum scrn_type ch_scrn; /* Normal background */
        unsigned char bm_bs;    /* Bitmap Size */
        unsigned char bm_sp;    /* Special Priority */
        unsigned char bm_scc;   /* Supplementary Character Number */
        unsigned char bm_spn;   /* Supplementary Palette Number */
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
        uint8_t ch_pls;         /* Plane size */
        uint32_t ch_map[4];     /* Map lead addresses */
};

extern void vdp2_scrn_bm_format_set(enum scrn_type, struct scrn_bm_format *);
extern void vdp2_scrn_ch_color_set(enum scrn_type, enum scrn_ch_color_type);
extern void vdp2_scrn_ch_format_set(struct scrn_ch_format *);
extern void vdp2_scrn_display_set(enum scrn_type);
extern void vdp2_scrn_priority_set(enum scrn_type, unsigned char);
extern void vdp2_scrn_rp_mode_set(enum scrn_rp_mode_type);

#endif /* !_SCRN_H_ */
