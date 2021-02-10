/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_MAP_H_
#define _CPU_MAP_H_

#include <scu/map.h>

/// @defgroup CPU_MAP_HELPERS CPU memory map helpers
/// @defgroup CPU_MAP_REGISTERS CPU memory map registers

/// @addtogroup CPU_MAP_HELPERS
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

/// @addtogroup CPU_MAP_REGISTERS
/// @{

/// CPU register.
#define SMR             0x0E00UL
/// CPU register.
#define BRR             0x0E01UL
/// CPU register.
#define SCR             0x0E02UL
/// CPU register.
#define TDR             0x0E03UL
/// CPU register.
#define SSR             0x0E04UL
/// CPU register.
#define RDR             0x0E05UL
/// CPU register.
#define TIER            0x0E10UL
/// CPU register.
#define FTCSR           0x0E11UL
/// CPU register.
#define FRCH            0x0E12UL
/// CPU register.
#define FRCL            0x0E13UL
/// CPU register.
#define OCRAH           0x0E14UL
/// CPU register.
#define OCRAL           0x0E15UL
/// CPU register.
#define OCRBH           0x0E14UL
/// CPU register.
#define OCRBL           0x0E15UL
/// CPU register.
#define TCR             0x0E16UL
/// CPU register.
#define TOCR            0x0E17UL
/// CPU register.
#define FICRH           0x0E18UL
/// CPU register.
#define FICRL           0x0E19UL
/// CPU register.
#define IPRB            0x0E60UL
/// CPU register.
#define VCRA            0x0E62UL
/// CPU register.
#define VCRB            0x0E64UL
/// CPU register.
#define VCRC            0x0E66UL
/// CPU register.
#define VCRD            0x0E68UL
/// CPU register.
#define DRCR0           0x0E71UL
/// CPU register.
#define DRCR1           0x0E72UL

/// CPU register.
#define WTCSRW          0x0E80UL
/// CPU register.
#define WTCSRR          0x0E80UL

/// CPU register.
#define WTCNTW          0x0E80UL
/// CPU register.
#define WTCNTR          0x0E81UL

/// CPU register.
#define RSTCSRW         0x0E82UL
/// CPU register.
#define RSTCSRR         0x0E83UL

/// CPU register.
#define FMR             0x0E90UL
/// CPU register.
#define SBYCR           0x0E91UL
/// CPU register.
#define CCR             0x0E92UL
/// CPU register.
#define ICR             0x0EE0UL
/// CPU register.
#define IPRA            0x0EE2UL
/// CPU register.
#define VCRWDT          0x0EE4UL
/// CPU register.
#define DVSR            0x0F00UL
/// CPU register.
#define DVDNT           0x0F04UL
/// CPU register.
#define DVCR            0x0F08UL
/// CPU register.
#define VCRDIV          0x0F0CUL
/// CPU register.
#define DVDNTH          0x0F10UL
/// CPU register.
#define DVDNTL          0x0F14UL
/// CPU register.
#define BARAH           0x0F40UL
/// CPU register.
#define BARAL           0x0F42UL
/// CPU register.
#define BAMRAH          0x0F44UL
/// CPU register.
#define BAMRAL          0x0F46UL
/// CPU register.
#define BBRA            0x0F48UL
/// CPU register.
#define BARBH           0x0F60UL
/// CPU register.
#define BARBL           0x0F62UL
/// CPU register.
#define BAMRBH          0x0F64UL
/// CPU register.
#define BAMRBL          0x0F66UL
/// CPU register.
#define BBRB            0x0F68UL
/// CPU register.
#define BDRBH           0x0F70UL
/// CPU register.
#define BDRBL           0x0F72UL
/// CPU register.
#define BDMRBH          0x0F74UL
/// CPU register.
#define BDMRBL          0x0F76UL
/// CPU register.
#define BRCR            0x0F78UL
/// CPU register.
#define SAR0            0x0F80UL
/// CPU register.
#define DAR0            0x0F84UL
/// CPU register.
#define TCR0            0x0F88UL
/// CPU register.
#define CHCR0           0x0F8CUL
/// CPU register.
#define SAR1            0x0F90UL
/// CPU register.
#define DAR1            0x0F94UL
/// CPU register.
#define TCR1            0x0F98UL
/// CPU register.
#define CHCR1           0x0F9CUL
/// CPU register.
#define VCRDMA0         0x0FA0UL
/// CPU register.
#define VCRDMA1         0x0FA8UL
/// CPU register.
#define DMAOR           0x0FB0UL
/// CPU register.
#define BCR1            0x0FE0UL
/// CPU register.
#define BCR2            0x0FE4UL
/// CPU register.
#define WCR             0x0FE8UL
/// CPU register.
#define MCR             0x0FECUL
/// CPU register.
#define RTCSR           0x0FF0UL
/// CPU register.
#define RTCNT           0x0FF4UL
/// CPU register.
#define RTCOR           0x0FF8UL

/// @}

#endif /* !_CPU_MAP_H_ */
