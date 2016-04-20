/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_INTERNAL_H_
#define _VDP2_INTERNAL_H_

#include <scu-internal.h>

#include <math.h>

#include <vdp2/map.h>
#include <vdp2/vram.h>

struct vdp2_state {
        uint16_t display_w;
        uint16_t display_h;

        uint16_t interlaced;

        struct {
                fix16_vector2_t nbg0;
                fix16_vector2_t nbg1;
                int16_vector2_t nbg2;
                int16_vector2_t nbg3;
        } scroll;

        struct {
                uint16_t bgon;          /* Register offset: 0x180020 */
                uint16_t mzctl;         /* Register offset: 0x180022 */
                uint16_t chctla;        /* Register offset: 0x180028 */
                uint16_t chctlb;        /* Register offset: 0x18002A */
                uint16_t bmpna;         /* Register offset: 0x18002C */
                uint16_t bmpnb;         /* Register offset: 0x18002E */
                uint16_t plsz;          /* Register offset: 0x18003A */
                uint16_t mpofn;         /* Register offset: 0x18003C */
                uint16_t mpofr;         /* Register offset: 0x18003E */
                uint16_t scxin0;        /* Register offset: 0x180070 */
                uint16_t scxdn0;        /* Register offset: 0x180072 */
                uint16_t scyin0;        /* Register offset: 0x180074 */
                uint16_t scydn0;        /* Register offset: 0x180076 */
                uint16_t scxin1;        /* Register offset: 0x180080 */
                uint16_t scxdn1;        /* Register offset: 0x180082 */
                uint16_t scyin1;        /* Register offset: 0x180084 */
                uint16_t scydn1;        /* Register offset: 0x180086 */
                uint16_t scxn2;         /* Register offset: 0x180090 */
                uint16_t scyn2;         /* Register offset: 0x180092 */
                uint16_t scxn3;         /* Register offset: 0x180094 */
                uint16_t scyn3;         /* Register offset: 0x180096 */
                uint16_t zmctl;         /* Register offset: 0x180098 */
                uint16_t scrctl;        /* Register offset: 0x18009A */
                uint16_t rpmd;          /* Register offset: 0x1800B0 */
                uint16_t spctl;         /* Register offset: 0x1800E0 */
                uint16_t craofa;        /* Register offset: 0x1800E4 */
                uint16_t craofb;        /* Register offset: 0x1800E6 */
                uint16_t prisa;         /* Register offset: 0x1800F0 */
                uint16_t prisb;         /* Register offset: 0x1800F2 */
                uint16_t prisc;         /* Register offset: 0x1800F4 */
                uint16_t prisd;         /* Register offset: 0x1800F6 */
                uint16_t prina;         /* Register offset: 0x1800F8 */
                uint16_t prinb;         /* Register offset: 0x1800FA */
                uint16_t prir;          /* Register offset: 0x1800FC */
                uint16_t clofen;        /* Register offset: 0x180110 */
                uint16_t clofsl;        /* Register offset: 0x180112 */
                uint16_t coar;          /* Register offset: 0x180114 */
                uint16_t coag;          /* Register offset: 0x180116 */
                uint16_t coab;          /* Register offset: 0x180118 */
                uint16_t cobr;          /* Register offset: 0x18011A */
                uint16_t cobg;          /* Register offset: 0x18011C */
                uint16_t cobb;          /* Register offset: 0x18011E */
        } buffered_regs;

        struct vram_ctl vram_ctl;
};

extern struct vdp2_state vdp2_state;

#endif /* !_VDP2_INTERNAL_H_ */
