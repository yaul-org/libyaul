/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_MAP_H_
#define _YAUL_CPU_MAP_H_

#include <sys/cdefs.h>

#include <scu/map.h>

/// @addtogroup MEMORY_MAP_AREAS
/// @{

/// @brief Access the CPU I/O registers.
///
/// @param x The byte offset.
#define CPU(x) (0xFFFFF000UL + (x))

/// @brief Base CPU address, for use with @ref cpu_ioregs_t.
#define CPU_IOREG_BASE CPU(0x00000E00UL)

/// @}

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_CPU_IO_REGISTERS CPU I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief CPU I/O register map.
/// @see CPU_IOREG_BASE
typedef struct cpu_ioregs {
        /// @brief CPU I/O register.
        uint8_t  smr;
        /// @brief CPU I/O register.
        uint8_t  brr;
        /// @brief CPU I/O register.
        uint8_t  scr;
        /// @brief CPU I/O register.
        uint8_t  tdr;
        /// @brief CPU I/O register.
        uint8_t  ssr;
        /// @brief CPU I/O register.
        uint8_t  rdr;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint8_t  tier;
        /// @brief CPU I/O register.
        uint8_t  ftcsr;
        /// @brief CPU I/O register.
        uint8_t  frch;
        /// @brief CPU I/O register.
        uint8_t  frcl;

        union {
                struct {
                        /// @brief CPU I/O register.
                        uint8_t  ocrah;
                        /// @brief CPU I/O register.
                        uint8_t  ocral;
                } __packed;

                struct {
                        /// @brief CPU I/O register.
                        uint8_t  ocrbh;
                        /// @brief CPU I/O register.
                        uint8_t  ocrbl;
                } __packed;
        };

        /// @brief CPU I/O register.
        uint8_t  tcr;
        /// @brief CPU I/O register.
        uint8_t  tocr;
        /// @brief CPU I/O register.
        uint8_t  ficrh;
        /// @brief CPU I/O register.
        uint8_t  ficrl;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint16_t iprb;
        /// @brief CPU I/O register.
        uint16_t vcra;
        /// @brief CPU I/O register.
        uint16_t vcrb;
        /// @brief CPU I/O register.
        uint16_t vcrc;
        /// @brief CPU I/O register.
        uint16_t vcrd;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint8_t  drcr0;
        /// @brief CPU I/O register.
        uint8_t  drcr1;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;

        union {
                /// @brief CPU I/O register.
                uint16_t wtcntw;
                /// @brief CPU I/O register.
                uint16_t wtcsrw;

                struct {
                        /// @brief CPU I/O register.
                        uint8_t wtcsrr;
                        /// @brief CPU I/O register.
                        uint8_t wtcntr;
                } __packed;
        };

        union {
                /// @brief CPU I/O register.
                uint16_t rstcsrw;

                struct {
                        unsigned int:8;
                        /// @brief CPU I/O register.
                        uint8_t rstcsrr;
                } __packed;
        };

        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint8_t  fmr;
        /// @brief CPU I/O register.
        uint8_t  sbycr;
        /// @brief CPU I/O register.
        uint8_t  ccr;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint16_t icr;
        /// @brief CPU I/O register.
        uint16_t ipra;
        /// @brief CPU I/O register.
        uint16_t vcrwdt;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint32_t dvsr;
        /// @brief CPU I/O register.
        uint32_t dvdnt;
        /// @brief CPU I/O register.
        uint32_t dvcr;
        /// @brief CPU I/O register.
        uint32_t vcrdiv;
        /// @brief CPU I/O register.
        uint32_t dvdnth;
        /// @brief CPU I/O register.
        uint32_t dvdntl;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint16_t barah;
        /// @brief CPU I/O register.
        uint16_t baral;
        /// @brief CPU I/O register.
        uint16_t bamrah;
        /// @brief CPU I/O register.
        uint16_t bamral;
        /// @brief CPU I/O register.
        uint32_t bbra;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint16_t barbh;
        /// @brief CPU I/O register.
        uint16_t barbl;
        /// @brief CPU I/O register.
        uint16_t bamrbh;
        /// @brief CPU I/O register.
        uint16_t bamrbl;
        /// @brief CPU I/O register.
        uint32_t bbrb;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint16_t bdrbh;
        /// @brief CPU I/O register.
        uint16_t bdrbl;
        /// @brief CPU I/O register.
        uint16_t bdmrbh;
        /// @brief CPU I/O register.
        uint16_t bdmrbl;
        /// @brief CPU I/O register.
        uint32_t brcr;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint32_t sar0;
        /// @brief CPU I/O register.
        uint32_t dar0;
        /// @brief CPU I/O register.
        uint32_t tcr0;
        /// @brief CPU I/O register.
        uint32_t chcr0;
        /// @brief CPU I/O register.
        uint32_t sar1;
        /// @brief CPU I/O register.
        uint32_t dar1;
        /// @brief CPU I/O register.
        uint32_t tcr1;
        /// @brief CPU I/O register.
        uint32_t chcr1;
        /// @brief CPU I/O register.
        uint32_t vcrdma0;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint32_t vcrdma1;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint32_t dmaor;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        unsigned int:8;
        /// @brief CPU I/O register.
        uint32_t bcr1;
        /// @brief CPU I/O register.
        uint32_t bcr2;
        /// @brief CPU I/O register.
        uint32_t wcr;
        /// @brief CPU I/O register.
        uint32_t mcr;
        /// @brief CPU I/O register.
        uint32_t rtcsr;
        /// @brief CPU I/O register.
        uint32_t rtcnt;
        /// @brief CPU I/O register.
        uint32_t rtcor;
} __packed cpu_ioregs_t;

/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SMR             0x0E00UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BRR             0x0E01UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SCR             0x0E02UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TDR             0x0E03UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SSR             0x0E04UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RDR             0x0E05UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TIER            0x0E10UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FTCSR           0x0E11UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FRCH            0x0E12UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FRCL            0x0E13UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define OCRAH           0x0E14UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define OCRAL           0x0E15UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define OCRBH           0x0E14UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define OCRBL           0x0E15UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TCR             0x0E16UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TOCR            0x0E17UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FICRH           0x0E18UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FICRL           0x0E19UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define IPRB            0x0E60UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCRA            0x0E62UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCRB            0x0E64UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCRC            0x0E66UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCRD            0x0E68UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DRCR0           0x0E71UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DRCR1           0x0E72UL

/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WTCSRW          0x0E80UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WTCSRR          0x0E80UL

/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WTCNTW          0x0E80UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WTCNTR          0x0E81UL

/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RSTCSRW         0x0E82UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RSTCSRR         0x0E83UL

/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define FMR             0x0E90UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SBYCR           0x0E91UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CCR             0x0E92UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define ICR             0x0EE0UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define IPRA            0x0EE2UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCRWDT          0x0EE4UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DVSR            0x0F00UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DVDNT           0x0F04UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DVCR            0x0F08UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCRDIV          0x0F0CUL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DVDNTH          0x0F10UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DVDNTL          0x0F14UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BARAH           0x0F40UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BARAL           0x0F42UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BAMRAH          0x0F44UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BAMRAL          0x0F46UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BBRA            0x0F48UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BARBH           0x0F60UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BARBL           0x0F62UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BAMRBH          0x0F64UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BAMRBL          0x0F66UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BBRB            0x0F68UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BDRBH           0x0F70UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BDRBL           0x0F72UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BDMRBH          0x0F74UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BDMRBL          0x0F76UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BRCR            0x0F78UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SAR0            0x0F80UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DAR0            0x0F84UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TCR0            0x0F88UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CHCR0           0x0F8CUL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define SAR1            0x0F90UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DAR1            0x0F94UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define TCR1            0x0F98UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define CHCR1           0x0F9CUL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCRDMA0         0x0FA0UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define VCRDMA1         0x0FA8UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define DMAOR           0x0FB0UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BCR1            0x0FE0UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define BCR2            0x0FE4UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define WCR             0x0FE8UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define MCR             0x0FECUL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RTCSR           0x0FF0UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RTCNT           0x0FF4UL
/// @brief CPU I/O register.
/// @see CPU
/// @see MEMORY_WRITE
/// @see MEMORY_READ
#define RTCOR           0x0FF8UL

/// @}

#endif /* !_YAUL_CPU_MAP_H_ */
