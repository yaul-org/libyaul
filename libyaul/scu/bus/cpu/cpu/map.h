/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_MAP_H_
#define _CPU_MAP_H_

#include <scu/map.h>

/* For writing to WTCSRW and WTCNTW, use 16-bit writes only:
 * WTCSRW = 0xA518 | (value & 0xFF)
 * WTCNTW = 0x5A00 | (value & 0xFF)
 *
 * For writing to RSTCSRW, use 16-bit writes only.
 *
 * When clearing WOVF bit:
 * RSTCSRW = 0xA500 | 0x00
 *
 * When writing RSTE and RSTS bits:
 * RSTCSRW = 0x5A00 | (value & 0xFF)
 *
 * For reading, use 8-bit reads on *R registers only */

#define MEMORY_WRITE_WTCNT(x) do {                                             \
        (*(volatile uint16_t *)(CPU(WTCNTW)) = (0x5A00 | ((x) & 0x00FF)));     \
} while (false)

#define MEMORY_WRITE_WTCSR(x) do {                                             \
        (*(volatile uint16_t *)(CPU(WTCSRW)) = (0xA500 | ((x) & 0x00FF)));     \
} while (false)

#define MEMORY_CLEAR_WOVF_RSTCSR() do {                                        \
        (*(volatile uint16_t *)(CPU(RSTCSRW)) = 0xA500);                       \
} while (false)

#define MEMORY_CLEAR_RSTCSR(x) do {                                            \
        (*(volatile uint16_t *)(CPU(RSTCSRW)) = (0x5A3F | ((x) & 0x00FF)));    \
} while (false)

/* Macros specific for processor. */
#define CPU(x)          (0xFFFFF000 + (x))

#define SMR             0x0E00
#define BRR             0x0E01
#define SCR             0x0E02
#define TDR             0x0E03
#define SSR             0x0E04
#define RDR             0x0E05
#define TIER            0x0E10
#define FTCSR           0x0E11
#define FRCH            0x0E12
#define FRCL            0x0E13
#define OCRAH           0x0E14
#define OCRAL           0x0E15
#define OCRBH           0x0E14
#define OCRBL           0x0E15
#define TCR             0x0E16
#define TOCR            0x0E17
#define FICRH           0x0E18
#define FICRL           0x0E19
#define IPRB            0x0E60
#define VCRA            0x0E62
#define VCRB            0x0E64
#define VCRC            0x0E66
#define VCRD            0x0E68
#define DRCR0           0x0E71
#define DRCR1           0x0E72

#define WTCSRW          0x0E80
#define WTCSRR          0x0E80

#define WTCNTW          0x0E80
#define WTCNTR          0x0E81

#define RSTCSRW         0x0E82
#define RSTCSRR         0x0E83

#define FMR             0x0E90
#define SBYCR           0x0E91
#define CCR             0x0E92
#define ICR             0x0EE0
#define IPRA            0x0EE2
#define VCRWDT          0x0EE4
#define DVSR            0x0F00
#define DVDNT           0x0F04
#define DVCR            0x0F08
#define VCRDIV          0x0F0C
#define DVDNTH          0x0F10
#define DVDNTL          0x0F14
#define BARAH           0x0F40
#define BARAL           0x0F42
#define BAMRAH          0x0F44
#define BAMRAL          0x0F46
#define BBRA            0x0F48
#define BARBH           0x0F60
#define BARBL           0x0F62
#define BAMRBH          0x0F64
#define BAMRBL          0x0F66
#define BBRB            0x0F68
#define BDRBH           0x0F70
#define BDRBL           0x0F72
#define BDMRBH          0x0F74
#define BDMRBL          0x0F76
#define BRCR            0x0F78
#define SAR0            0x0F80
#define DAR0            0x0F84
#define TCR0            0x0F88
#define CHCR0           0x0F8C
#define SAR1            0x0F90
#define DAR1            0x0F94
#define TCR1            0x0F98
#define CHCR1           0x0F9C
#define VCRDMA0         0x0FA0
#define VCRDMA1         0x0FA8
#define DMAOR           0x0FB0
#define BCR1            0x0FE0
#define BCR2            0x0FE4
#define WCR             0x0FE8
#define MCR             0x0FEC
#define RTCSR           0x0FF0
#define RTCNT           0x0FF4
#define RTCOR           0x0FF8

#endif /* !_CPU_MAP_H_ */
