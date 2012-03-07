/*
 * Copyright (c) 2011 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _VDP2_INTERNAL_H_
#define _VDP2_INTERNAL_H_

/* Write and read directly to specified address. */
#define MEM_POKE(x, y)  (*(volatile unsigned short *)(x) = (y))
#define MEM_READ(x)     (*(volatile unsigned short *)(x))

/* Macros specific for processor. */
#define VRAM_BANK(x, y) (0x25e00000 + (((x) & 0x3) << 18) + ((y) << 1))
#define CRAM_BANK(x, y) (0x25f00000 + ((x) << 4) + ((y) << 1))
#define VDP2(x)         (0x25f80000 + ((x) << 1))

enum vdp2_regs_type {
        TVMD,
        EXTEN,
        TVSTAT, /* RO */
        VRSIZE,
        HCNT, /* RO */
        VCNT, /* RO */
        /* RESERVE */
        RAMCTL = 0x007,
        CYCA0L,
        CYCA0U,
        CYCA1L,
        CYCA1U,
        CYCB0L,
        CYCB0U,
        CYCB1L,
        CYCB1U,
        BGON,
        MZCTL,
        SFSEL,
        SFCODE,
        CHCTLA,
        CHCTLB,
        BMPNA,
        BMPNB,
        PNCN0,
        PNCN1,
        PNCN2,
        PNCN3,
        PNCR,
        PLSZ,
        MPOFN,
        MPOFR,
        MPABN0,
        MPCDN0,
        MPABN1,
        MPCDN1,
        MPABN2,
        MPCDN2,
        MPABN3,
        MPCDN3,
        MPABRA,
        MPCDRA,
        MPEFRA,
        MPGHRA,
        MPIJRA,
        MPKLRA,
        MPMNRA,
        MPOPRA,
        MPABRB,
        MPCDRB,
        MPEFRB,
        MPGHRB,
        MPIJRB,
        MPKLRB,
        MPMNRB,
        MPOPRB,
        SCXIN0,
        SCXDN0,
        SCYIN0,
        SCYDN0,
        ZMXN0I,
        ZMXN0D,
        ZMYN0I,
        ZMYN0D,
        SCXIN1,
        SCXDN1,
        SCYIN1,
        SCYDN1,
        ZMXN1I,
        ZMXN1D,
        ZMYN1I,
        ZMYN1D,
        SCXN2,
        SCYN2,
        SCXN3,
        SCYN3,
        ZMCTL,
        SCRCTL,
        VCSTAU,
        VCSTAL,
        LSTA0U,
        LSTA0L,
        LSTA1U,
        LSTA1L,
        LCTAU,
        LCTAL,
        BKTAU,
        BKTAL,
        RPMD,
        RPRCTL,
        KTCTL,
        KTAOF,
        OVPNRA,
        OVPNRB,
        RPTAU,
        RPTAL,
        WPSX0,
        WPSY0,
        WPEX0,
        WPEY0,
        WPSX1,
        WPSY1,
        WPEX1,
        WPEY1,
        WCTLA,
        WCTLB,
        WCTLC,
        WCTLD,
        LWTA0U,
        LWTA0L,
        LWTA1U,
        LWTA1L,
        SPCTL,
        SDCTL,
        CRAOFA,
        CRAOFB,
        LNCLEN,
        SFPRMD,
        CCCTL,
        SFCCMD,
        PRISA,
        PRISB,
        PRISC,
        PRISD,
        PRINA,
        PRINB,
        PRIR,
        /* RESERVE */
        CCRSA = 0x080,
        CCRSB,
        CCRSC,
        CCRSD,
        CCRNA,
        CCRNB,
        CCRR,
        CCRLB,
        CLOFEN,
        CLOFSL,
        COAR,
        COAG,
        COAB,
        COBR,
        COBG,
        COBB
};

struct vdp2_regs {
        unsigned short chctla;  /* Register Offset: 0x180028 */
        unsigned short chctlb;  /* Register Offset: 0x18002a */
        unsigned short plsz;    /* Register Offset: 0x18003a */
        unsigned short rpmd;    /* Register Offset: 0x1800b0 */
} vdp2_regs;

#endif
