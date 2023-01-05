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

/// @brief Base CPU address, for use with @ref cpu_map_t.
#define CPU_MAP_BASE CPU(0x00000E00UL)

/// @}

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_CPU_IO_REGISTERS CPU I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief CPU I/O register map.
/// @see CPU_MAP_BASE
typedef struct cpu_map {
        uint8_t  smr;     /* 0x00000E00, 0x00000000 */
        uint8_t  brr;     /* 0x00000E01, 0x00000000 */
        uint8_t  scr;     /* 0x00000E02, 0x00000000 */
        uint8_t  tdr;     /* 0x00000E03, 0x00000000 */
        uint8_t  ssr;     /* 0x00000E04, 0x00000000 */
        uint8_t  rdr;     /* 0x00000E05, 0x00000000 */
        unsigned int:8;   /* 0x00000E06, 0x00000000 */
        unsigned int:8;   /* 0x00000E07, 0x00000000 */
        unsigned int:8;   /* 0x00000E08, 0x00000000 */
        unsigned int:8;   /* 0x00000E09, 0x00000000 */
        unsigned int:8;   /* 0x00000E0A, 0x00000000 */
        unsigned int:8;   /* 0x00000E0B, 0x00000000 */
        unsigned int:8;   /* 0x00000E0C, 0x00000000 */
        unsigned int:8;   /* 0x00000E0D, 0x00000000 */
        unsigned int:8;   /* 0x00000E0E, 0x00000000 */
        unsigned int:8;   /* 0x00000E0F, 0x00000000 */
        uint8_t  tier;    /* 0x00000E10, 0x0000000B */
        uint8_t  ftcsr;   /* 0x00000E11, 0x0000000B */
        uint8_t  frch;    /* 0x00000E12, 0x0000000B */
        uint8_t  frcl;    /* 0x00000E13, 0x0000000B */

        union {
                struct {
                        uint8_t  ocrah; /* 0x00000E14, 0x0000000B */
                        uint8_t  ocral; /* 0x00000E15, 0x0000000B */
                } __packed;

                struct {
                        uint8_t  ocrbh; /* 0x00000E14, 0x0000000B */
                        uint8_t  ocrbl; /* 0x00000E15, 0x0000000B */
                } __packed;
        };

        uint8_t  tcr;     /* 0x00000E16, 0x0000000B */
        uint8_t  tocr;    /* 0x00000E17, 0x0000000B */
        uint8_t  ficrh;   /* 0x00000E18, 0x0000000B */
        uint8_t  ficrl;   /* 0x00000E19, 0x0000000B */
        unsigned int:8;   /* 0x00000E1A, 0x0000000B */
        unsigned int:8;   /* 0x00000E1B, 0x0000000B */
        unsigned int:8;   /* 0x00000E1C, 0x0000000B */
        unsigned int:8;   /* 0x00000E1D, 0x0000000B */
        unsigned int:8;   /* 0x00000E1E, 0x0000000B */
        unsigned int:8;   /* 0x00000E1F, 0x0000000B */
        unsigned int:8;   /* 0x00000E20, 0x0000000B */
        unsigned int:8;   /* 0x00000E21, 0x0000000B */
        unsigned int:8;   /* 0x00000E22, 0x0000000B */
        unsigned int:8;   /* 0x00000E23, 0x0000000B */
        unsigned int:8;   /* 0x00000E24, 0x0000000B */
        unsigned int:8;   /* 0x00000E25, 0x0000000B */
        unsigned int:8;   /* 0x00000E26, 0x0000000B */
        unsigned int:8;   /* 0x00000E27, 0x0000000B */
        unsigned int:8;   /* 0x00000E28, 0x0000000B */
        unsigned int:8;   /* 0x00000E29, 0x0000000B */
        unsigned int:8;   /* 0x00000E2A, 0x0000000B */
        unsigned int:8;   /* 0x00000E2B, 0x0000000B */
        unsigned int:8;   /* 0x00000E2C, 0x0000000B */
        unsigned int:8;   /* 0x00000E2D, 0x0000000B */
        unsigned int:8;   /* 0x00000E2E, 0x0000000B */
        unsigned int:8;   /* 0x00000E2F, 0x0000000B */
        unsigned int:8;   /* 0x00000E30, 0x0000000B */
        unsigned int:8;   /* 0x00000E31, 0x0000000B */
        unsigned int:8;   /* 0x00000E32, 0x0000000B */
        unsigned int:8;   /* 0x00000E33, 0x0000000B */
        unsigned int:8;   /* 0x00000E34, 0x0000000B */
        unsigned int:8;   /* 0x00000E35, 0x0000000B */
        unsigned int:8;   /* 0x00000E36, 0x0000000B */
        unsigned int:8;   /* 0x00000E37, 0x0000000B */
        unsigned int:8;   /* 0x00000E38, 0x0000000B */
        unsigned int:8;   /* 0x00000E39, 0x0000000B */
        unsigned int:8;   /* 0x00000E3A, 0x0000000B */
        unsigned int:8;   /* 0x00000E3B, 0x0000000B */
        unsigned int:8;   /* 0x00000E3C, 0x0000000B */
        unsigned int:8;   /* 0x00000E3D, 0x0000000B */
        unsigned int:8;   /* 0x00000E3E, 0x0000000B */
        unsigned int:8;   /* 0x00000E3F, 0x0000000B */
        unsigned int:8;   /* 0x00000E40, 0x0000000B */
        unsigned int:8;   /* 0x00000E41, 0x0000000B */
        unsigned int:8;   /* 0x00000E42, 0x0000000B */
        unsigned int:8;   /* 0x00000E43, 0x0000000B */
        unsigned int:8;   /* 0x00000E44, 0x0000000B */
        unsigned int:8;   /* 0x00000E45, 0x0000000B */
        unsigned int:8;   /* 0x00000E46, 0x0000000B */
        unsigned int:8;   /* 0x00000E47, 0x0000000B */
        unsigned int:8;   /* 0x00000E48, 0x0000000B */
        unsigned int:8;   /* 0x00000E49, 0x0000000B */
        unsigned int:8;   /* 0x00000E4A, 0x0000000B */
        unsigned int:8;   /* 0x00000E4B, 0x0000000B */
        unsigned int:8;   /* 0x00000E4C, 0x0000000B */
        unsigned int:8;   /* 0x00000E4D, 0x0000000B */
        unsigned int:8;   /* 0x00000E4E, 0x0000000B */
        unsigned int:8;   /* 0x00000E4F, 0x0000000B */
        unsigned int:8;   /* 0x00000E50, 0x0000000B */
        unsigned int:8;   /* 0x00000E51, 0x0000000B */
        unsigned int:8;   /* 0x00000E52, 0x0000000B */
        unsigned int:8;   /* 0x00000E53, 0x0000000B */
        unsigned int:8;   /* 0x00000E54, 0x0000000B */
        unsigned int:8;   /* 0x00000E55, 0x0000000B */
        unsigned int:8;   /* 0x00000E56, 0x0000000B */
        unsigned int:8;   /* 0x00000E57, 0x0000000B */
        unsigned int:8;   /* 0x00000E58, 0x0000000B */
        unsigned int:8;   /* 0x00000E59, 0x0000000B */
        unsigned int:8;   /* 0x00000E5A, 0x0000000B */
        unsigned int:8;   /* 0x00000E5B, 0x0000000B */
        unsigned int:8;   /* 0x00000E5C, 0x0000000B */
        unsigned int:8;   /* 0x00000E5D, 0x0000000B */
        unsigned int:8;   /* 0x00000E5E, 0x0000000B */
        unsigned int:8;   /* 0x00000E5F, 0x0000000B */
        uint16_t iprb;    /* 0x00000E60, 0x00000052 */
        uint16_t vcra;    /* 0x00000E62, 0x00000052 */
        uint16_t vcrb;    /* 0x00000E64, 0x00000052 */
        uint16_t vcrc;    /* 0x00000E66, 0x00000052 */
        uint16_t vcrd;    /* 0x00000E68, 0x00000052 */
        unsigned int:8;   /* 0x00000E6A, 0x00000052 */
        unsigned int:8;   /* 0x00000E6B, 0x00000052 */
        unsigned int:8;   /* 0x00000E6C, 0x00000052 */
        unsigned int:8;   /* 0x00000E6D, 0x00000052 */
        unsigned int:8;   /* 0x00000E6E, 0x00000052 */
        unsigned int:8;   /* 0x00000E6F, 0x00000052 */
        unsigned int:8;   /* 0x00000E70, 0x00000052 */
        uint8_t  drcr0;   /* 0x00000E71, 0x0000005B */
        uint8_t  drcr1;   /* 0x00000E72, 0x0000005B */
        unsigned int:8;   /* 0x00000E73, 0x0000005B */
        unsigned int:8;   /* 0x00000E74, 0x0000005B */
        unsigned int:8;   /* 0x00000E75, 0x0000005B */
        unsigned int:8;   /* 0x00000E76, 0x0000005B */
        unsigned int:8;   /* 0x00000E77, 0x0000005B */
        unsigned int:8;   /* 0x00000E78, 0x0000005B */
        unsigned int:8;   /* 0x00000E79, 0x0000005B */
        unsigned int:8;   /* 0x00000E7A, 0x0000005B */
        unsigned int:8;   /* 0x00000E7B, 0x0000005B */
        unsigned int:8;   /* 0x00000E7C, 0x0000005B */
        unsigned int:8;   /* 0x00000E7D, 0x0000005B */
        unsigned int:8;   /* 0x00000E7E, 0x0000005B */
        unsigned int:8;   /* 0x00000E7F, 0x0000005B */

        union {
                uint16_t wtcntw; /* 0x00000E80, 0x00000069 */
                uint16_t wtcsrw; /* 0x00000E80, 0x00000069 */

                struct {
                        uint8_t wtcsrr; /* 0x00000E80, 0x00000069 */
                        uint8_t wtcntr; /* 0x00000E81, 0x00000069 */
                } __packed;
        };

        union {
                uint16_t rstcsrw; /* 0x00000E82, 0x00000069 */

                struct {
                        unsigned int:8;  /* 0x00000E82, 0x00000069 */
                        uint8_t rstcsrr; /* 0x00000E83, 0x00000069 */
                } __packed;
        };

        unsigned int:8;   /* 0x00000E84, 0x00000069 */
        unsigned int:8;   /* 0x00000E85, 0x00000069 */
        unsigned int:8;   /* 0x00000E86, 0x00000069 */
        unsigned int:8;   /* 0x00000E87, 0x00000069 */
        unsigned int:8;   /* 0x00000E88, 0x00000069 */
        unsigned int:8;   /* 0x00000E89, 0x00000069 */
        unsigned int:8;   /* 0x00000E8A, 0x00000069 */
        unsigned int:8;   /* 0x00000E8B, 0x00000069 */
        unsigned int:8;   /* 0x00000E8C, 0x00000069 */
        unsigned int:8;   /* 0x00000E8D, 0x00000069 */
        unsigned int:8;   /* 0x00000E8E, 0x00000069 */
        unsigned int:8;   /* 0x00000E8F, 0x00000069 */
        uint8_t  fmr;     /* 0x00000E90, 0x00000076 */
        uint8_t  sbycr;   /* 0x00000E91, 0x00000076 */
        uint8_t  ccr;     /* 0x00000E92, 0x00000076 */
        unsigned int:8;   /* 0x00000E93, 0x00000076 */
        unsigned int:8;   /* 0x00000E94, 0x00000076 */
        unsigned int:8;   /* 0x00000E95, 0x00000076 */
        unsigned int:8;   /* 0x00000E96, 0x00000076 */
        unsigned int:8;   /* 0x00000E97, 0x00000076 */
        unsigned int:8;   /* 0x00000E98, 0x00000076 */
        unsigned int:8;   /* 0x00000E99, 0x00000076 */
        unsigned int:8;   /* 0x00000E9A, 0x00000076 */
        unsigned int:8;   /* 0x00000E9B, 0x00000076 */
        unsigned int:8;   /* 0x00000E9C, 0x00000076 */
        unsigned int:8;   /* 0x00000E9D, 0x00000076 */
        unsigned int:8;   /* 0x00000E9E, 0x00000076 */
        unsigned int:8;   /* 0x00000E9F, 0x00000076 */
        unsigned int:8;   /* 0x00000EA0, 0x00000076 */
        unsigned int:8;   /* 0x00000EA1, 0x00000076 */
        unsigned int:8;   /* 0x00000EA2, 0x00000076 */
        unsigned int:8;   /* 0x00000EA3, 0x00000076 */
        unsigned int:8;   /* 0x00000EA4, 0x00000076 */
        unsigned int:8;   /* 0x00000EA5, 0x00000076 */
        unsigned int:8;   /* 0x00000EA6, 0x00000076 */
        unsigned int:8;   /* 0x00000EA7, 0x00000076 */
        unsigned int:8;   /* 0x00000EA8, 0x00000076 */
        unsigned int:8;   /* 0x00000EA9, 0x00000076 */
        unsigned int:8;   /* 0x00000EAA, 0x00000076 */
        unsigned int:8;   /* 0x00000EAB, 0x00000076 */
        unsigned int:8;   /* 0x00000EAC, 0x00000076 */
        unsigned int:8;   /* 0x00000EAD, 0x00000076 */
        unsigned int:8;   /* 0x00000EAE, 0x00000076 */
        unsigned int:8;   /* 0x00000EAF, 0x00000076 */
        unsigned int:8;   /* 0x00000EB0, 0x00000076 */
        unsigned int:8;   /* 0x00000EB1, 0x00000076 */
        unsigned int:8;   /* 0x00000EB2, 0x00000076 */
        unsigned int:8;   /* 0x00000EB3, 0x00000076 */
        unsigned int:8;   /* 0x00000EB4, 0x00000076 */
        unsigned int:8;   /* 0x00000EB5, 0x00000076 */
        unsigned int:8;   /* 0x00000EB6, 0x00000076 */
        unsigned int:8;   /* 0x00000EB7, 0x00000076 */
        unsigned int:8;   /* 0x00000EB8, 0x00000076 */
        unsigned int:8;   /* 0x00000EB9, 0x00000076 */
        unsigned int:8;   /* 0x00000EBA, 0x00000076 */
        unsigned int:8;   /* 0x00000EBB, 0x00000076 */
        unsigned int:8;   /* 0x00000EBC, 0x00000076 */
        unsigned int:8;   /* 0x00000EBD, 0x00000076 */
        unsigned int:8;   /* 0x00000EBE, 0x00000076 */
        unsigned int:8;   /* 0x00000EBF, 0x00000076 */
        unsigned int:8;   /* 0x00000EC0, 0x00000076 */
        unsigned int:8;   /* 0x00000EC1, 0x00000076 */
        unsigned int:8;   /* 0x00000EC2, 0x00000076 */
        unsigned int:8;   /* 0x00000EC3, 0x00000076 */
        unsigned int:8;   /* 0x00000EC4, 0x00000076 */
        unsigned int:8;   /* 0x00000EC5, 0x00000076 */
        unsigned int:8;   /* 0x00000EC6, 0x00000076 */
        unsigned int:8;   /* 0x00000EC7, 0x00000076 */
        unsigned int:8;   /* 0x00000EC8, 0x00000076 */
        unsigned int:8;   /* 0x00000EC9, 0x00000076 */
        unsigned int:8;   /* 0x00000ECA, 0x00000076 */
        unsigned int:8;   /* 0x00000ECB, 0x00000076 */
        unsigned int:8;   /* 0x00000ECC, 0x00000076 */
        unsigned int:8;   /* 0x00000ECD, 0x00000076 */
        unsigned int:8;   /* 0x00000ECE, 0x00000076 */
        unsigned int:8;   /* 0x00000ECF, 0x00000076 */
        unsigned int:8;   /* 0x00000ED0, 0x00000076 */
        unsigned int:8;   /* 0x00000ED1, 0x00000076 */
        unsigned int:8;   /* 0x00000ED2, 0x00000076 */
        unsigned int:8;   /* 0x00000ED3, 0x00000076 */
        unsigned int:8;   /* 0x00000ED4, 0x00000076 */
        unsigned int:8;   /* 0x00000ED5, 0x00000076 */
        unsigned int:8;   /* 0x00000ED6, 0x00000076 */
        unsigned int:8;   /* 0x00000ED7, 0x00000076 */
        unsigned int:8;   /* 0x00000ED8, 0x00000076 */
        unsigned int:8;   /* 0x00000ED9, 0x00000076 */
        unsigned int:8;   /* 0x00000EDA, 0x00000076 */
        unsigned int:8;   /* 0x00000EDB, 0x00000076 */
        unsigned int:8;   /* 0x00000EDC, 0x00000076 */
        unsigned int:8;   /* 0x00000EDD, 0x00000076 */
        unsigned int:8;   /* 0x00000EDE, 0x00000076 */
        unsigned int:8;   /* 0x00000EDF, 0x00000076 */
        uint16_t icr;     /* 0x00000EE0, 0x000000C4 */
        uint16_t ipra;    /* 0x00000EE2, 0x000000C4 */
        uint16_t vcrwdt;  /* 0x00000EE4, 0x000000C4 */
        unsigned int:8;   /* 0x00000EE6, 0x000000C4 */
        unsigned int:8;   /* 0x00000EE7, 0x000000C4 */
        unsigned int:8;   /* 0x00000EE8, 0x000000C4 */
        unsigned int:8;   /* 0x00000EE9, 0x000000C4 */
        unsigned int:8;   /* 0x00000EEA, 0x000000C4 */
        unsigned int:8;   /* 0x00000EEB, 0x000000C4 */
        unsigned int:8;   /* 0x00000EEC, 0x000000C4 */
        unsigned int:8;   /* 0x00000EED, 0x000000C4 */
        unsigned int:8;   /* 0x00000EEE, 0x000000C4 */
        unsigned int:8;   /* 0x00000EEF, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF0, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF1, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF2, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF3, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF4, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF5, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF6, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF7, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF8, 0x000000C4 */
        unsigned int:8;   /* 0x00000EF9, 0x000000C4 */
        unsigned int:8;   /* 0x00000EFA, 0x000000C4 */
        unsigned int:8;   /* 0x00000EFB, 0x000000C4 */
        unsigned int:8;   /* 0x00000EFC, 0x000000C4 */
        unsigned int:8;   /* 0x00000EFD, 0x000000C4 */
        unsigned int:8;   /* 0x00000EFE, 0x000000C4 */
        unsigned int:8;   /* 0x00000EFF, 0x000000C4 */
        uint32_t dvsr;    /* 0x00000F00, 0x000000E0 */
        uint32_t dvdnt;   /* 0x00000F04, 0x000000E0 */
        uint32_t dvcr;    /* 0x00000F08, 0x000000E0 */
        uint32_t vcrdiv;  /* 0x00000F0C, 0x000000E0 */
        uint32_t dvdnth;  /* 0x00000F10, 0x000000E0 */
        uint32_t dvdntl;  /* 0x00000F14, 0x000000E0 */
        unsigned int:8;   /* 0x00000F18, 0x000000E0 */
        unsigned int:8;   /* 0x00000F19, 0x000000E0 */
        unsigned int:8;   /* 0x00000F1A, 0x000000E0 */
        unsigned int:8;   /* 0x00000F1B, 0x000000E0 */
        unsigned int:8;   /* 0x00000F1C, 0x000000E0 */
        unsigned int:8;   /* 0x00000F1D, 0x000000E0 */
        unsigned int:8;   /* 0x00000F1E, 0x000000E0 */
        unsigned int:8;   /* 0x00000F1F, 0x000000E0 */
        unsigned int:8;   /* 0x00000F20, 0x000000E0 */
        unsigned int:8;   /* 0x00000F21, 0x000000E0 */
        unsigned int:8;   /* 0x00000F22, 0x000000E0 */
        unsigned int:8;   /* 0x00000F23, 0x000000E0 */
        unsigned int:8;   /* 0x00000F24, 0x000000E0 */
        unsigned int:8;   /* 0x00000F25, 0x000000E0 */
        unsigned int:8;   /* 0x00000F26, 0x000000E0 */
        unsigned int:8;   /* 0x00000F27, 0x000000E0 */
        unsigned int:8;   /* 0x00000F28, 0x000000E0 */
        unsigned int:8;   /* 0x00000F29, 0x000000E0 */
        unsigned int:8;   /* 0x00000F2A, 0x000000E0 */
        unsigned int:8;   /* 0x00000F2B, 0x000000E0 */
        unsigned int:8;   /* 0x00000F2C, 0x000000E0 */
        unsigned int:8;   /* 0x00000F2D, 0x000000E0 */
        unsigned int:8;   /* 0x00000F2E, 0x000000E0 */
        unsigned int:8;   /* 0x00000F2F, 0x000000E0 */
        unsigned int:8;   /* 0x00000F30, 0x000000E0 */
        unsigned int:8;   /* 0x00000F31, 0x000000E0 */
        unsigned int:8;   /* 0x00000F32, 0x000000E0 */
        unsigned int:8;   /* 0x00000F33, 0x000000E0 */
        unsigned int:8;   /* 0x00000F34, 0x000000E0 */
        unsigned int:8;   /* 0x00000F35, 0x000000E0 */
        unsigned int:8;   /* 0x00000F36, 0x000000E0 */
        unsigned int:8;   /* 0x00000F37, 0x000000E0 */
        unsigned int:8;   /* 0x00000F38, 0x000000E0 */
        unsigned int:8;   /* 0x00000F39, 0x000000E0 */
        unsigned int:8;   /* 0x00000F3A, 0x000000E0 */
        unsigned int:8;   /* 0x00000F3B, 0x000000E0 */
        unsigned int:8;   /* 0x00000F3C, 0x000000E0 */
        unsigned int:8;   /* 0x00000F3D, 0x000000E0 */
        unsigned int:8;   /* 0x00000F3E, 0x000000E0 */
        unsigned int:8;   /* 0x00000F3F, 0x000000E0 */
        uint16_t barah;   /* 0x00000F40, 0x0000010C */
        uint16_t baral;   /* 0x00000F42, 0x0000010C */
        uint16_t bamrah;  /* 0x00000F44, 0x0000010C */
        uint16_t bamral;  /* 0x00000F46, 0x0000010C */
        uint32_t bbra;    /* 0x00000F48, 0x0000010C */
        unsigned int:8;   /* 0x00000F4C, 0x0000010C */
        unsigned int:8;   /* 0x00000F4D, 0x0000010C */
        unsigned int:8;   /* 0x00000F4E, 0x0000010C */
        unsigned int:8;   /* 0x00000F4F, 0x0000010C */
        unsigned int:8;   /* 0x00000F50, 0x0000010C */
        unsigned int:8;   /* 0x00000F51, 0x0000010C */
        unsigned int:8;   /* 0x00000F52, 0x0000010C */
        unsigned int:8;   /* 0x00000F53, 0x0000010C */
        unsigned int:8;   /* 0x00000F54, 0x0000010C */
        unsigned int:8;   /* 0x00000F55, 0x0000010C */
        unsigned int:8;   /* 0x00000F56, 0x0000010C */
        unsigned int:8;   /* 0x00000F57, 0x0000010C */
        unsigned int:8;   /* 0x00000F58, 0x0000010C */
        unsigned int:8;   /* 0x00000F59, 0x0000010C */
        unsigned int:8;   /* 0x00000F5A, 0x0000010C */
        unsigned int:8;   /* 0x00000F5B, 0x0000010C */
        unsigned int:8;   /* 0x00000F5C, 0x0000010C */
        unsigned int:8;   /* 0x00000F5D, 0x0000010C */
        unsigned int:8;   /* 0x00000F5E, 0x0000010C */
        unsigned int:8;   /* 0x00000F5F, 0x0000010C */
        uint16_t barbh;   /* 0x00000F60, 0x00000124 */
        uint16_t barbl;   /* 0x00000F62, 0x00000124 */
        uint16_t bamrbh;  /* 0x00000F64, 0x00000124 */
        uint16_t bamrbl;  /* 0x00000F66, 0x00000124 */
        uint32_t bbrb;    /* 0x00000F68, 0x00000124 */
        unsigned int:8;   /* 0x00000F6C, 0x00000124 */
        unsigned int:8;   /* 0x00000F6D, 0x00000124 */
        unsigned int:8;   /* 0x00000F6E, 0x00000124 */
        unsigned int:8;   /* 0x00000F6F, 0x00000124 */
        uint16_t bdrbh;   /* 0x00000F70, 0x0000012C */
        uint16_t bdrbl;   /* 0x00000F72, 0x0000012C */
        uint16_t bdmrbh;  /* 0x00000F74, 0x0000012C */
        uint16_t bdmrbl;  /* 0x00000F76, 0x0000012C */
        uint32_t brcr;    /* 0x00000F78, 0x0000012C */
        unsigned int:8;   /* 0x00000F7C, 0x0000012C */
        unsigned int:8;   /* 0x00000F7D, 0x0000012C */
        unsigned int:8;   /* 0x00000F7E, 0x0000012C */
        unsigned int:8;   /* 0x00000F7F, 0x0000012C */
        uint32_t sar0;    /* 0x00000F80, 0x00000134 */
        uint32_t dar0;    /* 0x00000F84, 0x00000134 */
        uint32_t tcr0;    /* 0x00000F88, 0x00000134 */
        uint32_t chcr0;   /* 0x00000F8C, 0x00000134 */
        uint32_t sar1;    /* 0x00000F90, 0x00000134 */
        uint32_t dar1;    /* 0x00000F94, 0x00000134 */
        uint32_t tcr1;    /* 0x00000F98, 0x00000134 */
        uint32_t chcr1;   /* 0x00000F9C, 0x00000134 */
        uint32_t vcrdma0; /* 0x00000FA0, 0x00000134 */
        unsigned int:8;   /* 0x00000FA4, 0x00000134 */
        unsigned int:8;   /* 0x00000FA5, 0x00000134 */
        unsigned int:8;   /* 0x00000FA6, 0x00000134 */
        unsigned int:8;   /* 0x00000FA7, 0x00000134 */
        uint32_t vcrdma1; /* 0x00000FA8, 0x0000013C */
        unsigned int:8;   /* 0x00000FAC, 0x0000013C */
        unsigned int:8;   /* 0x00000FAD, 0x0000013C */
        unsigned int:8;   /* 0x00000FAE, 0x0000013C */
        unsigned int:8;   /* 0x00000FAF, 0x0000013C */
        uint32_t dmaor;   /* 0x00000FB0, 0x00000144 */
        unsigned int:8;   /* 0x00000FB4, 0x00000144 */
        unsigned int:8;   /* 0x00000FB5, 0x00000144 */
        unsigned int:8;   /* 0x00000FB6, 0x00000144 */
        unsigned int:8;   /* 0x00000FB7, 0x00000144 */
        unsigned int:8;   /* 0x00000FB8, 0x00000144 */
        unsigned int:8;   /* 0x00000FB9, 0x00000144 */
        unsigned int:8;   /* 0x00000FBA, 0x00000144 */
        unsigned int:8;   /* 0x00000FBB, 0x00000144 */
        unsigned int:8;   /* 0x00000FBC, 0x00000144 */
        unsigned int:8;   /* 0x00000FBD, 0x00000144 */
        unsigned int:8;   /* 0x00000FBE, 0x00000144 */
        unsigned int:8;   /* 0x00000FBF, 0x00000144 */
        unsigned int:8;   /* 0x00000FC0, 0x00000144 */
        unsigned int:8;   /* 0x00000FC1, 0x00000144 */
        unsigned int:8;   /* 0x00000FC2, 0x00000144 */
        unsigned int:8;   /* 0x00000FC3, 0x00000144 */
        unsigned int:8;   /* 0x00000FC4, 0x00000144 */
        unsigned int:8;   /* 0x00000FC5, 0x00000144 */
        unsigned int:8;   /* 0x00000FC6, 0x00000144 */
        unsigned int:8;   /* 0x00000FC7, 0x00000144 */
        unsigned int:8;   /* 0x00000FC8, 0x00000144 */
        unsigned int:8;   /* 0x00000FC9, 0x00000144 */
        unsigned int:8;   /* 0x00000FCA, 0x00000144 */
        unsigned int:8;   /* 0x00000FCB, 0x00000144 */
        unsigned int:8;   /* 0x00000FCC, 0x00000144 */
        unsigned int:8;   /* 0x00000FCD, 0x00000144 */
        unsigned int:8;   /* 0x00000FCE, 0x00000144 */
        unsigned int:8;   /* 0x00000FCF, 0x00000144 */
        unsigned int:8;   /* 0x00000FD0, 0x00000144 */
        unsigned int:8;   /* 0x00000FD1, 0x00000144 */
        unsigned int:8;   /* 0x00000FD2, 0x00000144 */
        unsigned int:8;   /* 0x00000FD3, 0x00000144 */
        unsigned int:8;   /* 0x00000FD4, 0x00000144 */
        unsigned int:8;   /* 0x00000FD5, 0x00000144 */
        unsigned int:8;   /* 0x00000FD6, 0x00000144 */
        unsigned int:8;   /* 0x00000FD7, 0x00000144 */
        unsigned int:8;   /* 0x00000FD8, 0x00000144 */
        unsigned int:8;   /* 0x00000FD9, 0x00000144 */
        unsigned int:8;   /* 0x00000FDA, 0x00000144 */
        unsigned int:8;   /* 0x00000FDB, 0x00000144 */
        unsigned int:8;   /* 0x00000FDC, 0x00000144 */
        unsigned int:8;   /* 0x00000FDD, 0x00000144 */
        unsigned int:8;   /* 0x00000FDE, 0x00000144 */
        unsigned int:8;   /* 0x00000FDF, 0x00000144 */
        uint32_t bcr1;    /* 0x00000FE0, 0x00000174 */
        uint32_t bcr2;    /* 0x00000FE4, 0x00000174 */
        uint32_t wcr;     /* 0x00000FE8, 0x00000174 */
        uint32_t mcr;     /* 0x00000FEC, 0x00000174 */
        uint32_t rtcsr;   /* 0x00000FF0, 0x00000174 */
        uint32_t rtcnt;   /* 0x00000FF4, 0x00000174 */
        uint32_t rtcor;   /* 0x00000FF8, 0x00000174 */
} __packed cpu_map_t;

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
