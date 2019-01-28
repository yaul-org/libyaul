/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP_INTERNAL_H_
#define _VDP_INTERNAL_H_

#include <math.h>

#include <scu-internal.h>

#include <vdp1/env.h>

#include <vdp2/map.h>
#include <vdp2/scrn.h>
#include <vdp2/vram.h>

#define COMMIT_XFER_VDP2_REG_TVMD       0
#define COMMIT_XFER_VDP2_REGS           1
#define COMMIT_XFER_BACK_SCREEN_BUFFER  2

struct state_vdp1 {
        struct {
                uint32_t cmdt_base;
                uint32_t texture_base;
                uint32_t gouraud_base;
                uint32_t clut_base;
                uint32_t remaining_base;
        } vram;

        struct vdp1_env env;

        union {
                uint16_t buffer[11];

                struct {
                        uint16_t tvmr;          /* Register offset: 0x100000 */
                        uint16_t fbcr;          /* Register offset: 0x100002 */
                        uint16_t ptmr;          /* Register offset: 0x100004 */
                        uint16_t ewdr;          /* Register offset: 0x100006 */
                        uint16_t ewlr;          /* Register offset: 0x100008 */
                        uint16_t ewrr;          /* Register offset: 0x10000A */
                        uint16_t endr;          /* Register offset: 0x10000C */
                        unsigned : 16;          /* Register offset: 0x100010 */
                        unsigned : 16;          /* Register offset: 0x100012 */
                        unsigned : 16;          /* Register offset: 0x100014 */
                        unsigned : 16;          /* Register offset: 0x100016 */
                };
        } regs;
};

struct state_vdp2 {
        struct {
                int16_vector2_t resolution;
        } tv;

        struct {
                fix16_vector2_t scroll;
        } nbg0;

        struct {
                fix16_vector2_t scroll;
        } nbg1;

        struct {
                int16_vector2_t scroll;
        } nbg2;

        struct {
                int16_vector2_t scroll;
        } nbg3;

        struct {
                struct dma_xfer xfer_table[3] __aligned(4 * 16);
                struct dma_reg_buffer reg_buffer;
        } commit;

        union {
                uint16_t buffer[144];

                struct {
                        uint16_t tvmd;          /* Register offset: 0x180000 */
                        uint16_t exten;         /* Register offset: 0x180002 */
                        uint16_t tvstat;        /* Register offset: 0x180004 */
                        uint16_t vrsize;        /* Register offset: 0x180006 */
                        uint16_t hcnt;          /* Register offset: 0x180008 */
                        uint16_t vcnt;          /* Register offset: 0x18000A */
                        unsigned : 16;          /* Register offset: 0x18000C */
                        uint16_t ramctl;        /* Register offset: 0x18000E */

                        union {
                                struct {
                                        uint16_t cyca0l;        /* Register offset: 0x180010 */
                                        uint16_t cyca0u;        /* Register offset: 0x180012 */
                                        uint16_t cyca1l;        /* Register offset: 0x180014 */
                                        uint16_t cyca1u;        /* Register offset: 0x180016 */
                                        uint16_t cycb0l;        /* Register offset: 0x180018 */
                                        uint16_t cycb0u;        /* Register offset: 0x18001A */
                                        uint16_t cycb1l;        /* Register offset: 0x18001C */
                                        uint16_t cycb1u;        /* Register offset: 0x18001E */
                                } __packed;

                                uint32_t cyc[4];
                        };

                        uint16_t bgon;          /* Register offset: 0x180020 */
                        uint16_t mzctl;         /* Register offset: 0x180022 */
                        uint16_t sfsel;         /* Register offset: 0x180024 */
                        uint16_t sfcode;        /* Register offset: 0x180026 */
                        uint16_t chctla;        /* Register offset: 0x180028 */
                        uint16_t chctlb;        /* Register offset: 0x18002A */
                        uint16_t bmpna;         /* Register offset: 0x18002C */
                        uint16_t bmpnb;         /* Register offset: 0x18002E */
                        uint16_t pncn0;         /* Register offset: 0x180030 */
                        uint16_t pncn1;         /* Register offset: 0x180032 */
                        uint16_t pncn2;         /* Register offset: 0x180034 */
                        uint16_t pncn3;         /* Register offset: 0x180036 */
                        uint16_t pncr;          /* Register offset: 0x180038 */
                        uint16_t plsz;          /* Register offset: 0x18003A */
                        uint16_t mpofn;         /* Register offset: 0x18003C */
                        uint16_t mpofr;         /* Register offset: 0x18003E */
                        uint16_t mpabn0;        /* Register offset: 0x180040 */
                        uint16_t mpcdn0;        /* Register offset: 0x180042 */
                        uint16_t mpabn1;        /* Register offset: 0x180044 */
                        uint16_t mpcdn1;        /* Register offset: 0x180046 */
                        uint16_t mpabn2;        /* Register offset: 0x180048 */
                        uint16_t mpcdn2;        /* Register offset: 0x18004A */
                        uint16_t mpabn3;        /* Register offset: 0x18004C */
                        uint16_t mpcdn3;        /* Register offset: 0x18004E */
                        uint16_t mpabra;        /* Register offset: 0x180050 */
                        uint16_t mpcdra;        /* Register offset: 0x180052 */
                        uint16_t mpefra;        /* Register offset: 0x180054 */
                        uint16_t mpghra;        /* Register offset: 0x180056 */
                        uint16_t mpijra;        /* Register offset: 0x180058 */
                        uint16_t mpklra;        /* Register offset: 0x18005A */
                        uint16_t mpmnra;        /* Register offset: 0x18005C */
                        uint16_t mpopra;        /* Register offset: 0x18005E */
                        uint16_t mpabrb;        /* Register offset: 0x180060 */
                        uint16_t mpcdrb;        /* Register offset: 0x180062 */
                        uint16_t mpefrb;        /* Register offset: 0x180064 */
                        uint16_t mpghrb;        /* Register offset: 0x180066 */
                        uint16_t mpijrb;        /* Register offset: 0x180068 */
                        uint16_t mpklrb;        /* Register offset: 0x18006A */
                        uint16_t mpmnrb;        /* Register offset: 0x18006C */
                        uint16_t mpoprb;        /* Register offset: 0x18006E */
                        uint16_t scxin0;        /* Register offset: 0x180070 */
                        uint16_t scxdn0;        /* Register offset: 0x180072 */
                        uint16_t scyin0;        /* Register offset: 0x180074 */
                        uint16_t scydn0;        /* Register offset: 0x180076 */
                        uint16_t zmxin0;        /* Register offset: 0x180078 */
                        uint16_t zmxdn0;        /* Register offset: 0x18007A */
                        uint16_t zmyin0;        /* Register offset: 0x18007C */
                        uint16_t zmydn0;        /* Register offset: 0x18007E */
                        uint16_t scxin1;        /* Register offset: 0x180080 */
                        uint16_t scxdn1;        /* Register offset: 0x180082 */
                        uint16_t scyin1;        /* Register offset: 0x180084 */
                        uint16_t scydn1;        /* Register offset: 0x180086 */
                        uint16_t zmxin1;        /* Register offset: 0x180088 */
                        uint16_t zmxdn1;        /* Register offset: 0x18008A */
                        uint16_t zmyin1;        /* Register offset: 0x18008C */
                        uint16_t zmydn1;        /* Register offset: 0x18008E */
                        uint16_t scxn2;         /* Register offset: 0x180090 */
                        uint16_t scyn2;         /* Register offset: 0x180092 */
                        uint16_t scxn3;         /* Register offset: 0x180094 */
                        uint16_t scyn3;         /* Register offset: 0x180096 */
                        uint16_t zmctl;         /* Register offset: 0x180098 */
                        uint16_t scrctl;        /* Register offset: 0x18009A */
                        uint16_t vcstau;        /* Register offset: 0x18009C */
                        uint16_t vcstal;        /* Register offset: 0x18009E */
                        uint16_t lsta0u;        /* Register offset: 0x1800A0 */
                        uint16_t lsta0l;        /* Register offset: 0x1800A2 */
                        uint16_t lsta1u;        /* Register offset: 0x1800A4 */
                        uint16_t lsta1l;        /* Register offset: 0x1800A6 */
                        uint16_t lctau;         /* Register offset: 0x1800A8 */
                        uint16_t lctal;         /* Register offset: 0x1800AA */
                        uint16_t bktau;         /* Register offset: 0x1800AC */
                        uint16_t bktal;         /* Register offset: 0x1800AE */
                        uint16_t rpmd;          /* Register offset: 0x1800B0 */
                        uint16_t rprctl;        /* Register offset: 0x1800B2 */
                        uint16_t ktctl;         /* Register offset: 0x1800B4 */
                        uint16_t ktaof;         /* Register offset: 0x1800B6 */
                        uint16_t ovpnra;        /* Register offset: 0x1800B8 */
                        uint16_t ovpnrb;        /* Register offset: 0x1800BA */
                        uint16_t rptau;         /* Register offset: 0x1800BC */
                        uint16_t rptal;         /* Register offset: 0x1800BE */
                        uint16_t wpsx0;         /* Register offset: 0x1800C0 */
                        uint16_t wpsy0;         /* Register offset: 0x1800C2 */
                        uint16_t wpex0;         /* Register offset: 0x1800C4 */
                        uint16_t wpey0;         /* Register offset: 0x1800C6 */
                        uint16_t wpsx1;         /* Register offset: 0x1800C8 */
                        uint16_t wpsy1;         /* Register offset: 0x1800CA */
                        uint16_t wpex1;         /* Register offset: 0x1800CC */
                        uint16_t wpey1;         /* Register offset: 0x1800CE */
                        uint16_t wctla;         /* Register offset: 0x1800D0 */
                        uint16_t wctlb;         /* Register offset: 0x1800D2 */
                        uint16_t wctlc;         /* Register offset: 0x1800D4 */
                        uint16_t wctld;         /* Register offset: 0x1800D6 */
                        uint16_t lwta0u;        /* Register offset: 0x1800D8 */
                        uint16_t lwta0l;        /* Register offset: 0x1800DA */
                        uint16_t lwta1u;        /* Register offset: 0x1800DC */
                        uint16_t lwta1l;        /* Register offset: 0x1800DE */
                        uint16_t spctl;         /* Register offset: 0x1800E0 */
                        uint16_t sdctl;         /* Register offset: 0x1800E2 */
                        uint16_t craofa;        /* Register offset: 0x1800E4 */
                        uint16_t craofb;        /* Register offset: 0x1800E6 */
                        uint16_t lnclen;        /* Register offset: 0x1800E8 */
                        uint16_t sfprmd;        /* Register offset: 0x1800EA */
                        uint16_t ccctl;         /* Register offset: 0x1800EC */
                        uint16_t sfccmd;        /* Register offset: 0x1800EE */
                        uint16_t prisa;         /* Register offset: 0x1800F0 */
                        uint16_t prisb;         /* Register offset: 0x1800F2 */
                        uint16_t prisc;         /* Register offset: 0x1800F4 */
                        uint16_t prisd;         /* Register offset: 0x1800F6 */
                        uint16_t prina;         /* Register offset: 0x1800F8 */
                        uint16_t prinb;         /* Register offset: 0x1800FA */
                        uint16_t prir;          /* Register offset: 0x1800FC */
                        unsigned : 16;          /* Register offset: 0x18000C */
                        uint16_t ccrsa;         /* Register offset: 0x180100 */
                        uint16_t ccrsb;         /* Register offset: 0x180102 */
                        uint16_t ccrsc;         /* Register offset: 0x180104 */
                        uint16_t ccrsd;         /* Register offset: 0x180106 */
                        uint16_t ccrna;         /* Register offset: 0x180108 */
                        uint16_t ccrnb;         /* Register offset: 0x18010A */
                        uint16_t ccrr;          /* Register offset: 0x18010C */
                        uint16_t ccrlb;         /* Register offset: 0x18010E */
                        uint16_t clofen;        /* Register offset: 0x180110 */
                        uint16_t clofsl;        /* Register offset: 0x180112 */
                        uint16_t coar;          /* Register offset: 0x180114 */
                        uint16_t coag;          /* Register offset: 0x180116 */
                        uint16_t coab;          /* Register offset: 0x180118 */
                        uint16_t cobr;          /* Register offset: 0x18011A */
                        uint16_t cobg;          /* Register offset: 0x18011C */
                        uint16_t cobb;          /* Register offset: 0x18011E */
                };
        } regs;
};

static inline struct state_vdp1 * __always_inline
_state_vdp1(void)
{
        extern struct state_vdp1 _internal_state_vdp1;

        return &_internal_state_vdp1;
}

static inline struct state_vdp2 * __always_inline
_state_vdp2(void)
{
        extern struct state_vdp2 _internal_state_vdp2;

        return &_internal_state_vdp2;
}

#endif /* !_VDP_INTERNAL_H_ */
