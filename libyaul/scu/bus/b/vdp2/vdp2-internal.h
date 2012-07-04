/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP2_INTERNAL_H_
#define _VDP2_INTERNAL_H_

#include <scu-internal.h>

/* Specific macros */
#define VDP2(x)         (0x25F80000 + (x))

/* Helpers */
#define VRAM_BANK_4MBIT(x)      (((x) >> 1) & 0xFFFF)
#define VRAM_OFFSET_4MBIT(x)    (((x) >> 17) & 0x0007)
#define VRAM_BANK_8MBIT(x)      (((x) >> 1) & 0xFFFF)
#define VRAM_OFFSET_8MBIT(x)    (((x) >> 18) & 0x0007)

/* VDP2 */
#define TVMD            0x0000
#define EXTEN           0x0002
#define TVSTAT          0x0004
#define VRSIZE          0x0006
#define HCNT            0x0008
#define VCNT            0x000A
#define RAMCTL          0x000E
#define CYCA0L          0x0010
#define CYCA0U          0x0012
#define CYCA1L          0x0014
#define CYCA1U          0x0016
#define CYCB0L          0x0018
#define CYCB0U          0x001A
#define CYCB1L          0x001C
#define CYCB1U          0x001E
#define BGON            0x0020
#define MZCTL           0x0022
#define SFSEL           0x0024
#define SFCODE          0x0026
#define CHCTLA          0x0028
#define CHCTLB          0x002A
#define BMPNA           0x002C
#define BMPNB           0x002E
#define PNCN0           0x0030
#define PNCN1           0x0032
#define PNCN2           0x0034
#define PNCN3           0x0036
#define PNCR            0x0038
#define PLSZ            0x003A
#define MPOFN           0x003C
#define MPOFR           0x003E
#define MPABN0          0x0040
#define MPCDN0          0x0042
#define MPABN1          0x0044
#define MPCDN1          0x0046
#define MPABN2          0x0048
#define MPCDN2          0x004A
#define MPABN3          0x004C
#define MPCDN3          0x004E
#define MPABRA          0x0050
#define MPCDRA          0x0052
#define MPEFRA          0x0054
#define MPGHRA          0x0056
#define MPIJRA          0x0058
#define MPKLRA          0x005A
#define MPMNRA          0x005C
#define MPOPRA          0x005E
#define MPABRB          0x0060
#define MPCDRB          0x0062
#define MPEFRB          0x0064
#define MPGHRB          0x0066
#define MPIJRB          0x0068
#define MPKLRB          0x006A
#define MPMNRB          0x006C
#define MPOPRB          0x006E
#define SCXIN0          0x0070
#define SCXDN0          0x0072
#define SCYIN0          0x0074
#define SCYDN0          0x0076
#define ZMXN0I          0x0078
#define ZMXN0D          0x007A
#define ZMYN0I          0x007C
#define ZMYN0D          0x007E
#define SCXIN1          0x0080
#define SCXDN1          0x0082
#define SCYIN1          0x0084
#define SCYDN1          0x0086
#define ZMXN1I          0x0088
#define ZMXN1D          0x008A
#define ZMYN1I          0x008C
#define ZMYN1D          0x008E
#define SCXN2           0x0090
#define SCYN2           0x0092
#define SCXN3           0x0094
#define SCYN3           0x0096
#define ZMCTL           0x0098
#define SCRCTL          0x009A
#define VCSTAU          0x009C
#define VCSTAL          0x009E
#define LSTA0U          0x00A0
#define LSTA0L          0x00A2
#define LSTA1U          0x00A4
#define LSTA1L          0x00A6
#define LCTAU           0x00A8
#define LCTAL           0x00AA
#define BKTAU           0x00AC
#define BKTAL           0x00AE
#define RPMD            0x00B0
#define RPRCTL          0x00B2
#define KTCTL           0x00B4
#define KTAOF           0x00B6
#define OVPNRA          0x00B8
#define OVPNRB          0x00BA
#define RPTAU           0x00BC
#define RPTAL           0x00BE
#define WPSX0           0x00C0
#define WPSY0           0x00C2
#define WPEX0           0x00C4
#define WPEY0           0x00C6
#define WPSX1           0x00C8
#define WPSY1           0x00CA
#define WPEX1           0x00CC
#define WPEY1           0x00CE
#define WCTLA           0x00D0
#define WCTLB           0x00D2
#define WCTLC           0x00D4
#define WCTLD           0x00D6
#define LWTA0U          0x00D8
#define LWTA0L          0x00DA
#define LWTA1U          0x00DC
#define LWTA1L          0x00DE
#define SPCTL           0x00E0
#define SDCTL           0x00E2
#define CRAOFA          0x00E4
#define CRAOFB          0x00E6
#define LNCLEN          0x00E8
#define SFPRMD          0x00EA
#define CCCTL           0x00EC
#define SFCCMD          0x00EE
#define PRISA           0x00F0
#define PRISB           0x00F2
#define PRISC           0x00F4
#define PRISD           0x00F6
#define PRINA           0x00F8
#define PRINB           0x00FA
#define PRIR            0x00FC
#define CCRSA           0x0100
#define CCRSB           0x0102
#define CCRSC           0x0104
#define CCRSD           0x0106
#define CCRNA           0x0108
#define CCRNB           0x010A
#define CCRR            0x010C
#define CCRLB           0x010E
#define CLOFEN          0x0110
#define CLOFSL          0x0112
#define COAR            0x0114
#define COAG            0x0116
#define COAB            0x0118
#define COBR            0x011A
#define COBG            0x011C
#define COBB            0x011E

struct vdp2_regs {
        uint16_t bgon;          /* Register offset: 0x180020 */
        uint16_t mzctl;         /* Register offset: 0x180022 */
        uint16_t chctla;        /* Register offset: 0x180028 */
        uint16_t chctlb;        /* Register offset: 0x18002A */
        uint16_t bmpna;         /* Register offset: 0x18002C */
        uint16_t bmpnb;         /* Register offset: 0x18002E */
        uint16_t plsz;          /* Register offset: 0x18003A */
        uint16_t mpofn;         /* Register offset: 0x18003C */
        uint16_t scrctl;        /* Register offset: 0x18009A */
        uint16_t rpmd;          /* Register offset: 0x1800B0 */
        uint16_t prina;         /* Register offset: 0x1800F8 */
        uint16_t prinb;         /* Register offset: 0x1800FA */
        uint16_t prir;          /* Register offset: 0x1800FC */
};

extern struct vdp2_regs vdp2_regs;
extern struct vram_ctl vram_ctl;

#endif /* !_VDP2_INTERNAL_H_ */
