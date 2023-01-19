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

#include <fix16.h>
#include <int16.h>

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_VDP2_IO_REGISTERS VDP2 I/O
/// @ingroup MEMORY_MAP
/// @{


/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TVMD   0x0000UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define EXTEN  0x0002UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TVSTAT 0x0004UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VRSIZE 0x0006UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define HCNT   0x0008UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCNT   0x000AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RAMCTL 0x000EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CYCA0L 0x0010UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CYCA0U 0x0012UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CYCA1L 0x0014UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CYCA1U 0x0016UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CYCB0L 0x0018UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CYCB0U 0x001AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CYCB1L 0x001CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CYCB1U 0x001EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BGON   0x0020UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MZCTL  0x0022UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SFSEL  0x0024UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SFCODE 0x0026UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CHCTLA 0x0028UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CHCTLB 0x002AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BMPNA  0x002CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BMPNB  0x002EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PNCN0  0x0030UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PNCN1  0x0032UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PNCN2  0x0034UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PNCN3  0x0036UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PNCR   0x0038UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PLSZ   0x003AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPOFN  0x003CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPOFR  0x003EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPABN0 0x0040UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPCDN0 0x0042UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPABN1 0x0044UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPCDN1 0x0046UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPABN2 0x0048UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPCDN2 0x004AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPABN3 0x004CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPCDN3 0x004EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPABRA 0x0050UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPCDRA 0x0052UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPEFRA 0x0054UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPGHRA 0x0056UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPIJRA 0x0058UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPKLRA 0x005AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPMNRA 0x005CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPOPRA 0x005EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPABRB 0x0060UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPCDRB 0x0062UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPEFRB 0x0064UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPGHRB 0x0066UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPIJRB 0x0068UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPKLRB 0x006AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPMNRB 0x006CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MPOPRB 0x006EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCXIN0 0x0070UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCXDN0 0x0072UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCYIN0 0x0074UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCYDN0 0x0076UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMXIN0 0x0078UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMXDN0 0x007AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMYIN0 0x007CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMYDN0 0x007EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCXIN1 0x0080UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCXDN1 0x0082UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCYIN1 0x0084UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCYDN1 0x0086UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMXIN1 0x0088UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMXDN1 0x008AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMYIN1 0x008CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMYDN1 0x008EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCXN2  0x0090UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCYN2  0x0092UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCXN3  0x0094UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCYN3  0x0096UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ZMCTL  0x0098UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCRCTL 0x009AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCSTAU 0x009CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCSTAL 0x009EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LSTA0U 0x00A0UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LSTA0L 0x00A2UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LSTA1U 0x00A4UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LSTA1L 0x00A6UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LCTAU  0x00A8UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LCTAL  0x00AAUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BKTAU  0x00ACUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BKTAL  0x00AEUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RPMD   0x00B0UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RPRCTL 0x00B2UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define KTCTL  0x00B4UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define KTAOF  0x00B6UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define OVPNRA 0x00B8UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define OVPNRB 0x00BAUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RPTAU  0x00BCUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RPTAL  0x00BEUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WPSX0  0x00C0UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WPSY0  0x00C2UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WPEX0  0x00C4UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WPEY0  0x00C6UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WPSX1  0x00C8UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WPSY1  0x00CAUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WPEX1  0x00CCUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WPEY1  0x00CEUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WCTLA  0x00D0UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WCTLB  0x00D2UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WCTLC  0x00D4UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WCTLD  0x00D6UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LWTA0U 0x00D8UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LWTA0L 0x00DAUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LWTA1U 0x00DCUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LWTA1L 0x00DEUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SPCTL  0x00E0UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SDCTL  0x00E2UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CRAOFA 0x00E4UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CRAOFB 0x00E6UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define LNCLEN 0x00E8UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SFPRMD 0x00EAUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCCTL  0x00ECUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SFCCMD 0x00EEUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PRISA  0x00F0UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PRISB  0x00F2UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PRISC  0x00F4UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PRISD  0x00F6UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PRINA  0x00F8UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PRINB  0x00FAUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define PRIR   0x00FCUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCRSA  0x0100UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCRSB  0x0102UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCRSC  0x0104UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCRSD  0x0106UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCRNA  0x0108UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCRNB  0x010AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCRR   0x010CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCRLB  0x010EUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CLOFEN 0x0110UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CLOFSL 0x0112UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define COAR   0x0114UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define COAG   0x0116UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define COAB   0x0118UL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define COBR   0x011AUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define COBG   0x011CUL
/// @brief VDP2 I/O register.
/// @see VDP2
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define COBB   0x011EUL

/// @brief VDP2 I/O register.
typedef union vdp2_ioregs {
        /// @brief VDP2 I/O register buffer.
        uint16_t buffer[144];

        struct {
                /// @brief VDP2 I/O register.
                uint16_t tvmd;
                /// @brief VDP2 I/O register.
                uint16_t exten;
                /// @brief VDP2 I/O register.
                uint16_t tvstat;
                /// @brief VDP2 I/O register.
                uint16_t vrsize;
                /// @brief VDP2 I/O register.
                uint16_t hcnt;
                /// @brief VDP2 I/O register.
                uint16_t vcnt;
                unsigned int :16;
                /// @brief VDP2 I/O register.
                uint16_t ramctl;

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t cyca0l;
                                /// @brief VDP2 I/O register.
                                uint16_t cyca0u;
                                /// @brief VDP2 I/O register.
                                uint16_t cyca1l;
                                /// @brief VDP2 I/O register.
                                uint16_t cyca1u;
                                /// @brief VDP2 I/O register.
                                uint16_t cycb0l;
                                /// @brief VDP2 I/O register.
                                uint16_t cycb0u;
                                /// @brief VDP2 I/O register.
                                uint16_t cycb1l;
                                /// @brief VDP2 I/O register.
                                uint16_t cycb1u;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t cyc[4];
                };

                /// @brief VDP2 I/O register.
                uint16_t bgon;
                /// @brief VDP2 I/O register.
                uint16_t mzctl;
                /// @brief VDP2 I/O register.
                uint16_t sfsel;
                /// @brief VDP2 I/O register.
                uint16_t sfcode;
                /// @brief VDP2 I/O register.
                uint16_t chctla;
                /// @brief VDP2 I/O register.
                uint16_t chctlb;
                /// @brief VDP2 I/O register.
                uint16_t bmpna;
                /// @brief VDP2 I/O register.
                uint16_t bmpnb;
                /// @brief VDP2 I/O register.
                uint16_t pncn0;
                /// @brief VDP2 I/O register.
                uint16_t pncn1;
                /// @brief VDP2 I/O register.
                uint16_t pncn2;
                /// @brief VDP2 I/O register.
                uint16_t pncn3;
                /// @brief VDP2 I/O register.
                uint16_t pncr;
                /// @brief VDP2 I/O register.
                uint16_t plsz;
                /// @brief VDP2 I/O register.
                uint16_t mpofn;
                /// @brief VDP2 I/O register.
                uint16_t mpofr;
                /// @brief VDP2 I/O register.
                uint16_t mpabn0;
                /// @brief VDP2 I/O register.
                uint16_t mpcdn0;
                /// @brief VDP2 I/O register.
                uint16_t mpabn1;
                /// @brief VDP2 I/O register.
                uint16_t mpcdn1;
                /// @brief VDP2 I/O register.
                uint16_t mpabn2;
                /// @brief VDP2 I/O register.
                uint16_t mpcdn2;
                /// @brief VDP2 I/O register.
                uint16_t mpabn3;
                /// @brief VDP2 I/O register.
                uint16_t mpcdn3;
                /// @brief VDP2 I/O register.
                uint16_t mpabra;
                /// @brief VDP2 I/O register.
                uint16_t mpcdra;
                /// @brief VDP2 I/O register.
                uint16_t mpefra;
                /// @brief VDP2 I/O register.
                uint16_t mpghra;
                /// @brief VDP2 I/O register.
                uint16_t mpijra;
                /// @brief VDP2 I/O register.
                uint16_t mpklra;
                /// @brief VDP2 I/O register.
                uint16_t mpmnra;
                /// @brief VDP2 I/O register.
                uint16_t mpopra;
                /// @brief VDP2 I/O register.
                uint16_t mpabrb;
                /// @brief VDP2 I/O register.
                uint16_t mpcdrb;
                /// @brief VDP2 I/O register.
                uint16_t mpefrb;
                /// @brief VDP2 I/O register.
                uint16_t mpghrb;
                /// @brief VDP2 I/O register.
                uint16_t mpijrb;
                /// @brief VDP2 I/O register.
                uint16_t mpklrb;
                /// @brief VDP2 I/O register.
                uint16_t mpmnrb;
                /// @brief VDP2 I/O register.
                uint16_t mpoprb;

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t scxin0;
                                /// @brief VDP2 I/O register.
                                uint16_t scxdn0;
                                /// @brief VDP2 I/O register.
                                uint16_t scyin0;
                                /// @brief VDP2 I/O register.
                                uint16_t scydn0;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        fix16_vec2_t sc0;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t zmxin0;
                                /// @brief VDP2 I/O register.
                                uint16_t zmxdn0;
                                /// @brief VDP2 I/O register.
                                uint16_t zmyin0;
                                /// @brief VDP2 I/O register.
                                uint16_t zmydn0;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        fix16_vec2_t zm0;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t scxin1;
                                /// @brief VDP2 I/O register.
                                uint16_t scxdn1;
                                /// @brief VDP2 I/O register.
                                uint16_t scyin1;
                                /// @brief VDP2 I/O register.
                                uint16_t scydn1;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        fix16_vec2_t sc1;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t zmxin1;
                                /// @brief VDP2 I/O register.
                                uint16_t zmxdn1;
                                /// @brief VDP2 I/O register.
                                uint16_t zmyin1;
                                /// @brief VDP2 I/O register.
                                uint16_t zmydn1;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        fix16_vec2_t zm1;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t scxn2;
                                /// @brief VDP2 I/O register.
                                uint16_t scyn2;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        int16_vec2_t scn2;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t scxn3;
                                /// @brief VDP2 I/O register.
                                uint16_t scyn3;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        int16_vec2_t scn3;
                };

                /// @brief VDP2 I/O register.
                uint16_t zmctl;
                /// @brief VDP2 I/O register.
                uint16_t scrctl;

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t vcstau;
                                /// @brief VDP2 I/O register.
                                uint16_t vcstal;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t vcsta;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t lsta0u;
                                /// @brief VDP2 I/O register.
                                uint16_t lsta0l;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t lsta0;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t lsta1u;
                                /// @brief VDP2 I/O register.
                                uint16_t lsta1l;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t lsta1;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t lctau;
                                /// @brief VDP2 I/O register.
                                uint16_t lctal;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t lcta;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t bktau;
                                /// @brief VDP2 I/O register.
                                uint16_t bktal;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t bkta;
                };

                /// @brief VDP2 I/O register.
                uint16_t rpmd;
                /// @brief VDP2 I/O register.
                uint16_t rprctl;
                /// @brief VDP2 I/O register.
                uint16_t ktctl;
                /// @brief VDP2 I/O register.
                uint16_t ktaof;
                /// @brief VDP2 I/O register.
                uint16_t ovpnra;
                /// @brief VDP2 I/O register.
                uint16_t ovpnrb;

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t rptau;
                                /// @brief VDP2 I/O register.
                                uint16_t rptal;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t rpta;
                };

                /// @brief VDP2 I/O register.
                uint16_t wpsx0;
                /// @brief VDP2 I/O register.
                uint16_t wpsy0;
                /// @brief VDP2 I/O register.
                uint16_t wpex0;
                /// @brief VDP2 I/O register.
                uint16_t wpey0;
                /// @brief VDP2 I/O register.
                uint16_t wpsx1;
                /// @brief VDP2 I/O register.
                uint16_t wpsy1;
                /// @brief VDP2 I/O register.
                uint16_t wpex1;
                /// @brief VDP2 I/O register.
                uint16_t wpey1;
                /// @brief VDP2 I/O register.
                uint16_t wctla;
                /// @brief VDP2 I/O register.
                uint16_t wctlb;
                /// @brief VDP2 I/O register.
                uint16_t wctlc;
                /// @brief VDP2 I/O register.
                uint16_t wctld;

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t lwta0u;
                                /// @brief VDP2 I/O register.
                                uint16_t lwta0l;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t lwta0;
                };

                union {
                        struct {
                                /// @brief VDP2 I/O register.
                                uint16_t lwta1u;
                                /// @brief VDP2 I/O register.
                                uint16_t lwta1l;
                        } __packed;

                        /// @brief VDP2 I/O register.
                        uint32_t lwta1;
                };

                /// @brief VDP2 I/O register.
                uint16_t spctl;
                /// @brief VDP2 I/O register.
                uint16_t sdctl;
                /// @brief VDP2 I/O register.
                uint16_t craofa;
                /// @brief VDP2 I/O register.
                uint16_t craofb;
                /// @brief VDP2 I/O register.
                uint16_t lnclen;
                /// @brief VDP2 I/O register.
                uint16_t sfprmd;
                /// @brief VDP2 I/O register.
                uint16_t ccctl;
                /// @brief VDP2 I/O register.
                uint16_t sfccmd;
                /// @brief VDP2 I/O register.
                uint16_t prisa;
                /// @brief VDP2 I/O register.
                uint16_t prisb;
                /// @brief VDP2 I/O register.
                uint16_t prisc;
                /// @brief VDP2 I/O register.
                uint16_t prisd;
                /// @brief VDP2 I/O register.
                uint16_t prina;
                /// @brief VDP2 I/O register.
                uint16_t prinb;
                /// @brief VDP2 I/O register.
                uint16_t prir;
                unsigned int :16;
                uint16_t ccrsa;
                /// @brief VDP2 I/O register.
                uint16_t ccrsb;
                /// @brief VDP2 I/O register.
                uint16_t ccrsc;
                /// @brief VDP2 I/O register.
                uint16_t ccrsd;
                /// @brief VDP2 I/O register.
                uint16_t ccrna;
                /// @brief VDP2 I/O register.
                uint16_t ccrnb;
                /// @brief VDP2 I/O register.
                uint16_t ccrr;
                /// @brief VDP2 I/O register.
                uint16_t ccrlb;
                /// @brief VDP2 I/O register.
                uint16_t clofen;
                /// @brief VDP2 I/O register.
                uint16_t clofsl;
                /// @brief VDP2 I/O register.
                uint16_t coar;
                /// @brief VDP2 I/O register.
                uint16_t coag;
                /// @brief VDP2 I/O register.
                uint16_t coab;
                /// @brief VDP2 I/O register.
                uint16_t cobr;
                /// @brief VDP2 I/O register.
                uint16_t cobg;
                /// @brief VDP2 I/O register.
                uint16_t cobb;
        };
} __aligned(4) __packed vdp2_ioregs_t;

/// @}

typedef uint32_t vdp2_cram_t;
typedef uint32_t vdp2_vram_t;

__BEGIN_DECLS

/// @brief Not yet documented.
///
/// @returns Not yet documented.
extern vdp2_ioregs_t *vdp2_regs_get(void);

/// @brief Not yet documented.
///
/// @returns Not yet documented.
extern vdp2_ioregs_t vdp2_regs_copy_get(void);

__END_DECLS

#endif /* !_YAUL_VDP2_MAP_H_ */
