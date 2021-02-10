/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_MAP_H_
#define _CPU_MAP_H_

#include <scu/map.h>

/// @defgroup CPU_IO_REGISTERS CPU I/O Registers

/// @addtogroup CPU_WDT
/// @{

/// @brief Write to the CPU-WDT @ref WTCNTW register.
///
/// @param x The 2-byte value.
#define MEMORY_WRITE_WTCNT(x)                                                  \
do {                                                                           \
        (*(volatile uint16_t *)(CPU(WTCNTW)) = (0x5A00 | ((x) & 0x00FF)));     \
} while (false)

/// @brief Write to the CPU-WDT @ref WTCSRW register.
///
/// @details When clearing the `WOVF` bit:
/// @code{.c}
/// *(volatile uint16_t *)RSTCSRW = 0xA500 | 0x00;
/// @endcode
/// 
/// @param x The 2-byte value.
#define MEMORY_WRITE_WTCSR(x)                                                  \
do {                                                                           \
        (*(volatile uint16_t *)(CPU(WTCSRW)) = (0xA500 | ((x) & 0x00FF)));     \
} while (false)

/// @brief Write `0xA500` to the CPU-WDT @ref RSTCSRW register.
///
/// @details For writing to @ref WTCSRW and @ref WTCNTW, use 16-bit writes only:
///
/// @code{.c}
/// *(volatile uint16_t *)WTCSRW = 0xA518 | (value & 0xFF);
/// *(volatile uint16_t *)WTCNTW = 0x5A00 | (value & 0xFF);
/// @endcode
#define MEMORY_CLEAR_WOVF_RSTCSR()                                             \
do {                                                                           \
        (*(volatile uint16_t *)(CPU(RSTCSRW)) = 0xA500);                       \
} while (false)

/// @brief Write to the CPU-WDT @ref RSTCSRW register.
///
/// @param x The 2-byte value.
#define MEMORY_CLEAR_RSTCSR(x)                                                 \
do {                                                                           \
        (*(volatile uint16_t *)(CPU(RSTCSRW)) = (0x5A3F | ((x) & 0x00FF)));    \
} while (false)

/// @brief Access the CPU I/O registers.
///
/// @param x The byte offset.
///
/// @see MEMORY_READ
/// @see MEMORY_WRITE
#define CPU(x) (0xFFFFF000UL + (x))

/// @}

/// @addtogroup CPU_IO_REGISTERS
/// @{

/// CPU I/O register.
#define SMR             0x0E00UL
/// CPU I/O register.
#define BRR             0x0E01UL
/// CPU I/O register.
#define SCR             0x0E02UL
/// CPU I/O register.
#define TDR             0x0E03UL
/// CPU I/O register.
#define SSR             0x0E04UL
/// CPU I/O register.
#define RDR             0x0E05UL
/// CPU I/O register.
#define TIER            0x0E10UL
/// CPU I/O register.
#define FTCSR           0x0E11UL
/// CPU I/O register.
#define FRCH            0x0E12UL
/// CPU I/O register.
#define FRCL            0x0E13UL
/// CPU I/O register.
#define OCRAH           0x0E14UL
/// CPU I/O register.
#define OCRAL           0x0E15UL
/// CPU I/O register.
#define OCRBH           0x0E14UL
/// CPU I/O register.
#define OCRBL           0x0E15UL
/// CPU I/O register.
#define TCR             0x0E16UL
/// CPU I/O register.
#define TOCR            0x0E17UL
/// CPU I/O register.
#define FICRH           0x0E18UL
/// CPU I/O register.
#define FICRL           0x0E19UL
/// CPU I/O register.
#define IPRB            0x0E60UL
/// CPU I/O register.
#define VCRA            0x0E62UL
/// CPU I/O register.
#define VCRB            0x0E64UL
/// CPU I/O register.
#define VCRC            0x0E66UL
/// CPU I/O register.
#define VCRD            0x0E68UL
/// CPU I/O register.
#define DRCR0           0x0E71UL
/// CPU I/O register.
#define DRCR1           0x0E72UL

/// CPU I/O register.
#define WTCSRW          0x0E80UL
/// CPU I/O register.
#define WTCSRR          0x0E80UL

/// CPU I/O register.
#define WTCNTW          0x0E80UL
/// CPU I/O register.
#define WTCNTR          0x0E81UL

/// CPU I/O register.
#define RSTCSRW         0x0E82UL
/// CPU I/O register.
#define RSTCSRR         0x0E83UL

/// CPU I/O register.
#define FMR             0x0E90UL
/// CPU I/O register.
#define SBYCR           0x0E91UL
/// CPU I/O register.
#define CCR             0x0E92UL
/// CPU I/O register.
#define ICR             0x0EE0UL
/// CPU I/O register.
#define IPRA            0x0EE2UL
/// CPU I/O register.
#define VCRWDT          0x0EE4UL
/// CPU I/O register.
#define DVSR            0x0F00UL
/// CPU I/O register.
#define DVDNT           0x0F04UL
/// CPU I/O register.
#define DVCR            0x0F08UL
/// CPU I/O register.
#define VCRDIV          0x0F0CUL
/// CPU I/O register.
#define DVDNTH          0x0F10UL
/// CPU I/O register.
#define DVDNTL          0x0F14UL
/// CPU I/O register.
#define BARAH           0x0F40UL
/// CPU I/O register.
#define BARAL           0x0F42UL
/// CPU I/O register.
#define BAMRAH          0x0F44UL
/// CPU I/O register.
#define BAMRAL          0x0F46UL
/// CPU I/O register.
#define BBRA            0x0F48UL
/// CPU I/O register.
#define BARBH           0x0F60UL
/// CPU I/O register.
#define BARBL           0x0F62UL
/// CPU I/O register.
#define BAMRBH          0x0F64UL
/// CPU I/O register.
#define BAMRBL          0x0F66UL
/// CPU I/O register.
#define BBRB            0x0F68UL
/// CPU I/O register.
#define BDRBH           0x0F70UL
/// CPU I/O register.
#define BDRBL           0x0F72UL
/// CPU I/O register.
#define BDMRBH          0x0F74UL
/// CPU I/O register.
#define BDMRBL          0x0F76UL
/// CPU I/O register.
#define BRCR            0x0F78UL
/// CPU I/O register.
#define SAR0            0x0F80UL
/// CPU I/O register.
#define DAR0            0x0F84UL
/// CPU I/O register.
#define TCR0            0x0F88UL
/// CPU I/O register.
#define CHCR0           0x0F8CUL
/// CPU I/O register.
#define SAR1            0x0F90UL
/// CPU I/O register.
#define DAR1            0x0F94UL
/// CPU I/O register.
#define TCR1            0x0F98UL
/// CPU I/O register.
#define CHCR1           0x0F9CUL
/// CPU I/O register.
#define VCRDMA0         0x0FA0UL
/// CPU I/O register.
#define VCRDMA1         0x0FA8UL
/// CPU I/O register.
#define DMAOR           0x0FB0UL
/// CPU I/O register.
#define BCR1            0x0FE0UL
/// CPU I/O register.
#define BCR2            0x0FE4UL
/// CPU I/O register.
#define WCR             0x0FE8UL
/// CPU I/O register.
#define MCR             0x0FECUL
/// CPU I/O register.
#define RTCSR           0x0FF0UL
/// CPU I/O register.
#define RTCNT           0x0FF4UL
/// CPU I/O register.
#define RTCOR           0x0FF8UL

/// @}

#endif /* !_CPU_MAP_H_ */
