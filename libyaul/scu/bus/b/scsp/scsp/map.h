/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCSP_MAP_H_
#define _YAUL_SCSP_MAP_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>

/// @addtogroup MEMORY_MAP
/// @defgroup MEMORY_MAP_SCSP_IO_REGISTERS SCSP I/O
/// @ingroup MEMORY_MAP
/// @{

/// @brief SCSP common control register CA alias.
#define SCSP_CA                     0x0408UL

/// @brief SCSP common control register DMEA alias.
#define SCSP_DMEA                   0x0412UL

/// @brief SCSP common control register DMEA[15:1] word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_DMEA_LO                0x0412UL

/// @brief SCSP common control register DMEA[19:16] and DRGA[11:1] word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_DRGA                   0x0414UL

/// @brief SCSP common control register DTLG and DMA control word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_DTLG                   0x0416UL

#define SCSP_INT_CPU_MANUAL         (1U << 5)

/// @brief SCSP common control register Main CPU Interrupt Enable (MCIEB).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_MCIEB                  0x042AUL

/// @brief SCSP common control register Main CPU Interrupt Pending (MCIPD).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_MCIPD                  0x042CUL

/// @brief SCSP common control register Main CPU Interrupt Reset (MCIRE).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_MCIRE                  0x042EUL

/// @brief SCSP common control register MIBUF word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_MIBUF                  0x0404UL

/// @brief SCSP common control register MOBUF word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_MOBUF                  0x0406UL

/// @brief SCSP common control register MSLC / CA word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_MSLC                   0x0408UL

/// @brief SCSP common control register MVOL word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_MVOL                   0x0400UL

/// @brief SCSP common control register RBL/RBP word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_RBL_RBP                0x0402UL

/// @brief SCSP common control register Sound CPU Interrupt Enable (SCIEB).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_SCIEB                  0x041EUL

/// @brief SCSP common control register Sound CPU Interrupt Level 0 (SCILV0).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_SCILV0                 0x0424UL

/// @brief SCSP common control register Sound CPU Interrupt Level 1 (SCILV1).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_SCILV1                 0x0426UL

/// @brief SCSP common control register Sound CPU Interrupt Level 2 (SCILV2).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_SCILV2                 0x0428UL

/// @brief SCSP common control register Sound CPU Interrupt Pending (SCIPD).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_SCIPD                  0x0420UL

/// @brief SCSP common control register Sound CPU Interrupt Reset (SCIRE).
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_SCIRE                  0x0422UL

/// @brief SCSP common control register Timer A word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_TIMA                   0x0418UL

/// @brief SCSP common control register Timer B word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_TIMB                   0x041AUL

/// @brief SCSP common control register Timer C word.
/// @see SCSP
/// @see MEMORY_WRITE, MEMORY_READ
#define SCSP_TIMC                   0x041CUL

/// @brief SCSP slot I/O registers (12 words, 0x18 bytes).
typedef struct scsp_slot_regs {
    union {
        /// @brief SCSP slot I/O register buffer.
        uint16_t buffer[12];
        /// @brief SCSP slot I/O register raw buffer.
        uint16_t raw[12];

        struct {
            /* 0x00 */
            uint16_t :3;
            uint16_t kyonex:1;
            uint16_t kyonb:1;
            uint16_t sbctl:2;
            uint16_t ssctl:2;
            uint16_t lpctl:2;
            uint16_t pcm8b:1;
            uint16_t sa_high:4;

            /* 0x02 */
            union {
                uint16_t sa;
                uint16_t sa_low;
            };

            /* 0x04 */
            uint16_t lsa;

            /* 0x06 */
            uint16_t lea;

            /* 0x08 */
            uint16_t d2r:5;
            uint16_t d1r:5;
            uint16_t eghold:1;
            uint16_t ar:5;

            /* 0x0A */
            uint16_t :1;
            uint16_t lpslnk:1;
            uint16_t krs:4;
            uint16_t dl:5;
            uint16_t rr:5;

            /* 0x0C */
            uint16_t :6;
            uint16_t stwinh:1;
            uint16_t sdir:1;
            uint16_t tl:8;

            /* 0x0E */
            uint16_t mdl:4;
            uint16_t mdxsl:6;
            uint16_t mdysl:6;

            /* 0x10 */
            uint16_t :1;
            uint16_t oct:4;
            uint16_t :1;
            uint16_t fns:10;

            /* 0x12 */
            uint16_t lfore:1;
            uint16_t lfof:5;
            uint16_t plfows:2;
            uint16_t plfos:3;
            uint16_t alfows:2;
            uint16_t alfos:3;

            /* 0x14 */
            uint16_t :9;
            uint16_t isel:4;
            uint16_t imxl:3;

            /* 0x16 */
            uint16_t disdl:3;
            uint16_t dipan:5;
            uint16_t efsdl:3;
            uint16_t efpan:5;
        } __packed;
    };
} __aligned(2) __packed scsp_slot_regs_t;

static_assert(sizeof(scsp_slot_regs_t) == 0x18);

/// @brief SCSP common control I/O registers (24 words, 0x30 bytes).
typedef struct scsp_cmn_regs {
    union {
        /// @brief SCSP common control I/O register buffer.
        uint16_t buffer[24];
        /// @brief SCSP common control I/O register raw buffer.
        uint16_t raw[24];

        struct {
            /* 0x400 */
            uint16_t :6;
            uint16_t mem4mb:1;
            uint16_t dac18b:1;
            uint16_t :4;
            uint16_t mvol:4;

            /* 0x402 */
            uint16_t :7;
            uint16_t rbl:2;
            uint16_t rbp:7;

            /* 0x404 */
            uint16_t :3;
            uint16_t mofull:1;
            uint16_t moemp:1;
            uint16_t miovf:1;
            uint16_t mifull:1;
            uint16_t miemp:1;
            uint16_t mibuf:8;

            /* 0x406 */
            uint16_t :8;
            uint16_t mobuf:8;

            /* 0x408 */
            uint16_t mslc:5;
            uint16_t ca:4;
            uint16_t :7;

            /* 0x40A - 0x410: reserved */
            unsigned int :16;
            unsigned int :16;
            unsigned int :16;
            unsigned int :16;

            /* 0x412 */
            uint16_t dmea_low:15;
            uint16_t :1;

            /* 0x414 */
            uint16_t dmea_high:4;
            uint16_t drga:11;
            uint16_t :1;

            /* 0x416 */
            uint16_t :1;
            uint16_t dgate:1;
            uint16_t ddir:1;
            uint16_t dexe:1;
            uint16_t dtlg:11;
            uint16_t :1;

            /* 0x418 */
            uint16_t :5;
            uint16_t tactl:3;
            uint16_t tima:8;

            /* 0x41A */
            uint16_t :5;
            uint16_t tbctl:3;
            uint16_t timb:8;

            /* 0x41C */
            uint16_t :5;
            uint16_t tcctl:3;
            uint16_t timc:8;

            /* 0x41E */
            uint16_t :5;
            uint16_t scieb:11;

            /* 0x420 */
            uint16_t :5;
            uint16_t scipd:11;

            /* 0x422 */
            uint16_t :5;
            uint16_t scire:11;

            /* 0x424 */
            uint16_t :8;
            uint16_t scilv0:8;

            /* 0x426 */
            uint16_t :8;
            uint16_t scilv1:8;

            /* 0x428 */
            uint16_t :8;
            uint16_t scilv2:8;

            /* 0x42A */
            uint16_t :5;
            uint16_t mcieb:11;

            /* 0x42C */
            uint16_t :5;
            uint16_t mcipd:11;

            /* 0x42E */
            uint16_t :5;
            uint16_t mcire:11;
        } __packed;
    };
} __aligned(2) __packed scsp_cmn_regs_t;

static_assert(sizeof(scsp_cmn_regs_t) == 0x30);

/// @brief SCSP I/O register map.
typedef union scsp_ioregs {
    /// @brief SCSP I/O register buffer.
    uint16_t buffer[536];

    struct {
        struct {
            /// @brief SCSP slot registers.
            scsp_slot_regs_t regs;
            uint16_t reserved[4];
        } slots[32];

        /// @brief SCSP common control registers.
        scsp_cmn_regs_t cmn;
    };
} __aligned(2) __packed scsp_ioregs_t;

static_assert(sizeof(scsp_ioregs_t) == 0x430);

/// @}

#endif /* !_YAUL_SCSP_MAP_H_ */
