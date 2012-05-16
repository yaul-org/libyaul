/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _VDP2_INTERNAL_H_
#define _VDP2_INTERNAL_H_

#include <inttypes.h>

/* Write and read directly to specified address */
#define MEM_POKE(x, y)  (*(volatile uint16_t *)(x) = (y))
#define MEM_READ(x)     (*(volatile uint16_t *)(x))

/* Macros specific for processor */
#define VDP2(x)         (0x25F80000 + ((x) << 1))

/* Helpers specific to this processor */
#define VRAM_BANK_4MBIT(x)      (((x) >> 1) & 0xFFFF)
#define VRAM_OFFSET_4MBIT(x)    (((x) >> 17) & 0x0007)
#define VRAM_BANK_8MBIT(x)      (((x) >> 1) & 0xFFFF)
#define VRAM_OFFSET_8MBIT(x)    (((x) >> 18) & 0x0007)

enum {
        TVMD,
        EXTEN,
        TVSTAT, /* RO */
        VRSIZE,
        HCNT, /* RO */
        VCNT, /* RO */
        /* RESERVE */
        RAMCTL = 0x0007,
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
        CCRSA = 0x0080,
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
        uint16_t bgon; /* Register offset: 0x180020 */
        uint16_t mzctl; /* Register offset: 0x180022 */
        uint16_t chctla; /* Register offset: 0x180028 */
        uint16_t chctlb; /* Register offset: 0x18002A */
        uint16_t bmpna; /* Register offset: 0x18002C */
        uint16_t bmpnb; /* Register offset: 0x18002E */
        uint16_t plsz; /* Register offset: 0x18003A */
        uint16_t mpofn; /* Register offset: 0x18003C */
        uint16_t scrctl; /* Register offset: 0x18009A */
        uint16_t rpmd; /* Register offset: 0x1800B0 */
        uint16_t prina; /* Register offset: 0x1800F8 */
        uint16_t prinb; /* Register offset: 0x1800FA */
        uint16_t prir; /* Register offset: 0x1800FC */
};

extern struct vdp2_regs vdp2_regs;
extern struct vram_ctl vram_ctl;

#endif /* !_VDP2_INTERNAL_H_ */
