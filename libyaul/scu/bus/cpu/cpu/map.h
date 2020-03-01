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
#define CPU(x)          (0xFFFFF000UL + (x))

#define SMR             0x0E00UL
#define BRR             0x0E01UL
#define SCR             0x0E02UL
#define TDR             0x0E03UL
#define SSR             0x0E04UL
#define RDR             0x0E05UL
#define TIER            0x0E10UL
#define FTCSR           0x0E11UL
#define FRCH            0x0E12UL
#define FRCL            0x0E13UL
#define OCRAH           0x0E14UL
#define OCRAL           0x0E15UL
#define OCRBH           0x0E14UL
#define OCRBL           0x0E15UL
#define TCR             0x0E16UL
#define TOCR            0x0E17UL
#define FICRH           0x0E18UL
#define FICRL           0x0E19UL
#define IPRB            0x0E60UL
#define VCRA            0x0E62UL
#define VCRB            0x0E64UL
#define VCRC            0x0E66UL
#define VCRD            0x0E68UL
#define DRCR0           0x0E71UL
#define DRCR1           0x0E72UL

#define WTCSRW          0x0E80UL
#define WTCSRR          0x0E80UL

#define WTCNTW          0x0E80UL
#define WTCNTR          0x0E81UL

#define RSTCSRW         0x0E82UL
#define RSTCSRR         0x0E83UL

#define FMR             0x0E90UL
#define SBYCR           0x0E91UL
#define CCR             0x0E92UL
#define ICR             0x0EE0UL
#define IPRA            0x0EE2UL
#define VCRWDT          0x0EE4UL
#define DVSR            0x0F00UL
#define DVDNT           0x0F04UL
#define DVCR            0x0F08UL
#define VCRDIV          0x0F0CUL
#define DVDNTH          0x0F10UL
#define DVDNTL          0x0F14UL
#define BARAH           0x0F40UL
#define BARAL           0x0F42UL
#define BAMRAH          0x0F44UL
#define BAMRAL          0x0F46UL
#define BBRA            0x0F48UL
#define BARBH           0x0F60UL
#define BARBL           0x0F62UL
#define BAMRBH          0x0F64UL
#define BAMRBL          0x0F66UL
#define BBRB            0x0F68UL
#define BDRBH           0x0F70UL
#define BDRBL           0x0F72UL
#define BDMRBH          0x0F74UL
#define BDMRBL          0x0F76UL
#define BRCR            0x0F78UL
#define SAR0            0x0F80UL
#define DAR0            0x0F84UL
#define TCR0            0x0F88UL
#define CHCR0           0x0F8CUL
#define SAR1            0x0F90UL
#define DAR1            0x0F94UL
#define TCR1            0x0F98UL
#define CHCR1           0x0F9CUL
#define VCRDMA0         0x0FA0UL
#define VCRDMA1         0x0FA8UL
#define DMAOR           0x0FB0UL
#define BCR1            0x0FE0UL
#define BCR2            0x0FE4UL
#define WCR             0x0FE8UL
#define MCR             0x0FECUL
#define RTCSR           0x0FF0UL
#define RTCNT           0x0FF4UL
#define RTCOR           0x0FF8UL

#endif /* !_CPU_MAP_H_ */
