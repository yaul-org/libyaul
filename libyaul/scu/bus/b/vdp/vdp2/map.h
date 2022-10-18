/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP2_MAP_H_
#define _YAUL_VDP2_MAP_H_

#include <sys/cdefs.h>

#include <stdint.h>

#define TVMD   0x0000UL
#define EXTEN  0x0002UL
#define TVSTAT 0x0004UL
#define VRSIZE 0x0006UL
#define HCNT   0x0008UL
#define VCNT   0x000AUL
#define RAMCTL 0x000EUL
#define CYCA0L 0x0010UL
#define CYCA0U 0x0012UL
#define CYCA1L 0x0014UL
#define CYCA1U 0x0016UL
#define CYCB0L 0x0018UL
#define CYCB0U 0x001AUL
#define CYCB1L 0x001CUL
#define CYCB1U 0x001EUL
#define BGON   0x0020UL
#define MZCTL  0x0022UL
#define SFSEL  0x0024UL
#define SFCODE 0x0026UL
#define CHCTLA 0x0028UL
#define CHCTLB 0x002AUL
#define BMPNA  0x002CUL
#define BMPNB  0x002EUL
#define PNCN0  0x0030UL
#define PNCN1  0x0032UL
#define PNCN2  0x0034UL
#define PNCN3  0x0036UL
#define PNCR   0x0038UL
#define PLSZ   0x003AUL
#define MPOFN  0x003CUL
#define MPOFR  0x003EUL
#define MPABN0 0x0040UL
#define MPCDN0 0x0042UL
#define MPABN1 0x0044UL
#define MPCDN1 0x0046UL
#define MPABN2 0x0048UL
#define MPCDN2 0x004AUL
#define MPABN3 0x004CUL
#define MPCDN3 0x004EUL
#define MPABRA 0x0050UL
#define MPCDRA 0x0052UL
#define MPEFRA 0x0054UL
#define MPGHRA 0x0056UL
#define MPIJRA 0x0058UL
#define MPKLRA 0x005AUL
#define MPMNRA 0x005CUL
#define MPOPRA 0x005EUL
#define MPABRB 0x0060UL
#define MPCDRB 0x0062UL
#define MPEFRB 0x0064UL
#define MPGHRB 0x0066UL
#define MPIJRB 0x0068UL
#define MPKLRB 0x006AUL
#define MPMNRB 0x006CUL
#define MPOPRB 0x006EUL
#define SCXIN0 0x0070UL
#define SCXDN0 0x0072UL
#define SCYIN0 0x0074UL
#define SCYDN0 0x0076UL
#define ZMXIN0 0x0078UL
#define ZMXDN0 0x007AUL
#define ZMYIN0 0x007CUL
#define ZMYDN0 0x007EUL
#define SCXIN1 0x0080UL
#define SCXDN1 0x0082UL
#define SCYIN1 0x0084UL
#define SCYDN1 0x0086UL
#define ZMXIN1 0x0088UL
#define ZMXDN1 0x008AUL
#define ZMYIN1 0x008CUL
#define ZMYDN1 0x008EUL
#define SCXN2  0x0090UL
#define SCYN2  0x0092UL
#define SCXN3  0x0094UL
#define SCYN3  0x0096UL
#define ZMCTL  0x0098UL
#define SCRCTL 0x009AUL
#define VCSTAU 0x009CUL
#define VCSTAL 0x009EUL
#define LSTA0U 0x00A0UL
#define LSTA0L 0x00A2UL
#define LSTA1U 0x00A4UL
#define LSTA1L 0x00A6UL
#define LCTAU  0x00A8UL
#define LCTAL  0x00AAUL
#define BKTAU  0x00ACUL
#define BKTAL  0x00AEUL
#define RPMD   0x00B0UL
#define RPRCTL 0x00B2UL
#define KTCTL  0x00B4UL
#define KTAOF  0x00B6UL
#define OVPNRA 0x00B8UL
#define OVPNRB 0x00BAUL
#define RPTAU  0x00BCUL
#define RPTAL  0x00BEUL
#define WPSX0  0x00C0UL
#define WPSY0  0x00C2UL
#define WPEX0  0x00C4UL
#define WPEY0  0x00C6UL
#define WPSX1  0x00C8UL
#define WPSY1  0x00CAUL
#define WPEX1  0x00CCUL
#define WPEY1  0x00CEUL
#define WCTLA  0x00D0UL
#define WCTLB  0x00D2UL
#define WCTLC  0x00D4UL
#define WCTLD  0x00D6UL
#define LWTA0U 0x00D8UL
#define LWTA0L 0x00DAUL
#define LWTA1U 0x00DCUL
#define LWTA1L 0x00DEUL
#define SPCTL  0x00E0UL
#define SDCTL  0x00E2UL
#define CRAOFA 0x00E4UL
#define CRAOFB 0x00E6UL
#define LNCLEN 0x00E8UL
#define SFPRMD 0x00EAUL
#define CCCTL  0x00ECUL
#define SFCCMD 0x00EEUL
#define PRISA  0x00F0UL
#define PRISB  0x00F2UL
#define PRISC  0x00F4UL
#define PRISD  0x00F6UL
#define PRINA  0x00F8UL
#define PRINB  0x00FAUL
#define PRIR   0x00FCUL
#define CCRSA  0x0100UL
#define CCRSB  0x0102UL
#define CCRSC  0x0104UL
#define CCRSD  0x0106UL
#define CCRNA  0x0108UL
#define CCRNB  0x010AUL
#define CCRR   0x010CUL
#define CCRLB  0x010EUL
#define CLOFEN 0x0110UL
#define CLOFSL 0x0112UL
#define COAR   0x0114UL
#define COAG   0x0116UL
#define COAB   0x0118UL
#define COBR   0x011AUL
#define COBG   0x011CUL
#define COBB   0x011EUL

typedef uint32_t vdp2_cram_t;
typedef uint32_t vdp2_vram_t;

typedef union vdp2_registers {
        uint16_t buffer[144];

        struct {
                uint16_t tvmd;
                uint16_t exten;
                uint16_t tvstat;
                uint16_t vrsize;
                uint16_t hcnt;
                uint16_t vcnt;
                unsigned int :16;
                uint16_t ramctl;

                union {
                        struct {
                                uint16_t cyca0l;
                                uint16_t cyca0u;
                                uint16_t cyca1l;
                                uint16_t cyca1u;
                                uint16_t cycb0l;
                                uint16_t cycb0u;
                                uint16_t cycb1l;
                                uint16_t cycb1u;
                        } __packed;

                        uint32_t cyc[4];
                };

                uint16_t bgon;
                uint16_t mzctl;
                uint16_t sfsel;
                uint16_t sfcode;
                uint16_t chctla;
                uint16_t chctlb;
                uint16_t bmpna;
                uint16_t bmpnb;
                uint16_t pncn0;
                uint16_t pncn1;
                uint16_t pncn2;
                uint16_t pncn3;
                uint16_t pncr;
                uint16_t plsz;
                uint16_t mpofn;
                uint16_t mpofr;
                uint16_t mpabn0;
                uint16_t mpcdn0;
                uint16_t mpabn1;
                uint16_t mpcdn1;
                uint16_t mpabn2;
                uint16_t mpcdn2;
                uint16_t mpabn3;
                uint16_t mpcdn3;
                uint16_t mpabra;
                uint16_t mpcdra;
                uint16_t mpefra;
                uint16_t mpghra;
                uint16_t mpijra;
                uint16_t mpklra;
                uint16_t mpmnra;
                uint16_t mpopra;
                uint16_t mpabrb;
                uint16_t mpcdrb;
                uint16_t mpefrb;
                uint16_t mpghrb;
                uint16_t mpijrb;
                uint16_t mpklrb;
                uint16_t mpmnrb;
                uint16_t mpoprb;
                uint16_t scxin0;
                uint16_t scxdn0;
                uint16_t scyin0;
                uint16_t scydn0;
                uint16_t zmxin0;
                uint16_t zmxdn0;
                uint16_t zmyin0;
                uint16_t zmydn0;
                uint16_t scxin1;
                uint16_t scxdn1;
                uint16_t scyin1;
                uint16_t scydn1;
                uint16_t zmxin1;
                uint16_t zmxdn1;
                uint16_t zmyin1;
                uint16_t zmydn1;
                uint16_t scxn2;
                uint16_t scyn2;
                uint16_t scxn3;
                uint16_t scyn3;
                uint16_t zmctl;
                uint16_t scrctl;
                uint16_t vcstau;
                uint16_t vcstal;
                uint16_t lsta0u;
                uint16_t lsta0l;
                uint16_t lsta1u;
                uint16_t lsta1l;
                uint16_t lctau;
                uint16_t lctal;
                uint16_t bktau;
                uint16_t bktal;
                uint16_t rpmd;
                uint16_t rprctl;
                uint16_t ktctl;
                uint16_t ktaof;
                uint16_t ovpnra;
                uint16_t ovpnrb;
                uint16_t rptau;
                uint16_t rptal;
                uint16_t wpsx0;
                uint16_t wpsy0;
                uint16_t wpex0;
                uint16_t wpey0;
                uint16_t wpsx1;
                uint16_t wpsy1;
                uint16_t wpex1;
                uint16_t wpey1;
                uint16_t wctla;
                uint16_t wctlb;
                uint16_t wctlc;
                uint16_t wctld;
                uint16_t lwta0u;
                uint16_t lwta0l;
                uint16_t lwta1u;
                uint16_t lwta1l;
                uint16_t spctl;
                uint16_t sdctl;
                uint16_t craofa;
                uint16_t craofb;
                uint16_t lnclen;
                uint16_t sfprmd;
                uint16_t ccctl;
                uint16_t sfccmd;
                uint16_t prisa;
                uint16_t prisb;
                uint16_t prisc;
                uint16_t prisd;
                uint16_t prina;
                uint16_t prinb;
                uint16_t prir;
                unsigned int :16;
                uint16_t ccrsa;
                uint16_t ccrsb;
                uint16_t ccrsc;
                uint16_t ccrsd;
                uint16_t ccrna;
                uint16_t ccrnb;
                uint16_t ccrr;
                uint16_t ccrlb;
                uint16_t clofen;
                uint16_t clofsl;
                uint16_t coar;
                uint16_t coag;
                uint16_t coab;
                uint16_t cobr;
                uint16_t cobg;
                uint16_t cobb;
        };
} vdp2_registers_t;

__BEGIN_DECLS

extern vdp2_registers_t *vdp2_regs_get(void);
extern vdp2_registers_t vdp2_regs_copy_get(void);

__END_DECLS

#endif /* !_YAUL_VDP2_MAP_H_ */
