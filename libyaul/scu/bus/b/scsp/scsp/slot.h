/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCSP_SLOT_H_
#define _YAUL_SCSP_SLOT_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <scu/map.h>
#include <scsp/map.h>

__BEGIN_DECLS

/// @defgroup SCSP_SLOT SCSP Slot
/// @ingroup SCSP
/// SCSP slot registers.
///
/// All registers are 16-bit and accessed as whole words, never as bytes.
/// Register layouts are in @ref MEMORY_MAP_SCSP_IO_REGISTERS.

/// @addtogroup SCSP_SLOT
/// @{

/// @brief Total number of SCSP slots.
#define SCSP_SLOT_COUNT         (32)

/// @brief Number of 16-bit register words per SCSP slot.
#define SCSP_SLOT_WORD_COUNT    (12)

/// @brief SCSP slot word index 0 (KYONEX, KYONB, SBCTL, SSCTL, LPCTL, PCM8B, SA[19:16]).
#define SCSP_SLOT_WORD_KYONEX   0
/// @brief SCSP slot word index 1 (SA[15:0]).
#define SCSP_SLOT_WORD_SA       1
/// @brief SCSP slot word index 2 (LSA[15:0]).
#define SCSP_SLOT_WORD_LSA      2
/// @brief SCSP slot word index 3 (LEA[15:0]).
#define SCSP_SLOT_WORD_LEA      3
/// @brief SCSP slot word index 4 (D2R, D1R, EGHOLD, AR).
#define SCSP_SLOT_WORD_D2R      4
/// @brief SCSP slot word index 5 (LPSLNK, KRS, DL, RR).
#define SCSP_SLOT_WORD_LPSLNK   5
/// @brief SCSP slot word index 6 (STWINH, SDIR, TL).
#define SCSP_SLOT_WORD_STWINH   6
/// @brief SCSP slot word index 7 (MDL, MDXSL, MDYSL).
#define SCSP_SLOT_WORD_MDL      7
/// @brief SCSP slot word index 8 (OCT, FNS).
#define SCSP_SLOT_WORD_OCT      8
/// @brief SCSP slot word index 9 (LFORE, LFOF, PLFOWS, PLFOS, ALFOWS, ALFOS).
#define SCSP_SLOT_WORD_LFORE    9
/// @brief SCSP slot word index 10 (ISEL, IMXL).
#define SCSP_SLOT_WORD_ISEL     10
/// @brief SCSP slot word index 11 (DISDL, DIPAN, EFSDL, EFPAN).
#define SCSP_SLOT_WORD_DISDL    11

/// @brief SCSP slot word index 0 alias.
#define SCSP_SLOT_WORD_SA_HI    SCSP_SLOT_WORD_KYONEX
/// @brief SCSP slot word index 1 alias.
#define SCSP_SLOT_WORD_SA_LO    SCSP_SLOT_WORD_SA
/// @brief SCSP slot word index 4 alias.
#define SCSP_SLOT_WORD_EG1      SCSP_SLOT_WORD_D2R
/// @brief SCSP slot word index 5 alias.
#define SCSP_SLOT_WORD_RR       SCSP_SLOT_WORD_LPSLNK
/// @brief SCSP slot word index 5 alias.
#define SCSP_SLOT_WORD_EG2      SCSP_SLOT_WORD_LPSLNK
/// @brief SCSP slot word index 6 alias.
#define SCSP_SLOT_WORD_TL       SCSP_SLOT_WORD_STWINH
/// @brief SCSP slot word index 8 alias.
#define SCSP_SLOT_WORD_OCT_FNS  SCSP_SLOT_WORD_OCT
/// @brief SCSP slot word index 8 alias.
#define SCSP_SLOT_WORD_FNS      SCSP_SLOT_WORD_OCT
/// @brief SCSP slot word index 9 alias.
#define SCSP_SLOT_WORD_LFO      SCSP_SLOT_WORD_LFORE
/// @brief SCSP slot word index 10 alias.
#define SCSP_SLOT_WORD_IMXL     SCSP_SLOT_WORD_ISEL
/// @brief SCSP slot word index 11 alias.
#define SCSP_SLOT_WORD_PAN      SCSP_SLOT_WORD_DISDL

/// @brief Get a 16-bit register word from slot @p slot.
///
/// @param slot The slot number (0 to 31).
/// @param word The word index (0 to 11, see SCSP_SLOT_WORD_*).
///
/// @return The 16-bit register word value.
static inline __always_inline uint16_t
scsp_slot_word_get(uint8_t slot, uint8_t word)
{
    assert(slot < SCSP_SLOT_COUNT);
    assert(word < SCSP_SLOT_WORD_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    return scsp_ioregs->slots[slot].regs.buffer[word];
}

/// @brief Set a 16-bit register word for slot @p slot.
///
/// @param slot  The slot number (0 to 31).
/// @param word  The word index (0 to 11, see SCSP_SLOT_WORD_*).
/// @param value The 16-bit register word value to write.
///
/// @note Word 0 holds KYONEX, which acts on all slots. Bits 15:12 are written
/// as 0, so this never executes KEY_ON/OFF; use @ref scsp_slot_kyonex_execute.
static inline __always_inline void
scsp_slot_word_set(uint8_t slot, uint8_t word, uint16_t value)
{
    assert(slot < SCSP_SLOT_COUNT);
    assert(word < SCSP_SLOT_WORD_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    if (word == SCSP_SLOT_WORD_KYONEX) {
        value &= 0x0FFF;
    }

    scsp_ioregs->slots[slot].regs.buffer[word] = value;
}

/// @brief Get the 20-bit waveform start address for slot @p slot.
///
/// Combines SA[19:16] (bits 3:0 of word 0) and SA[15:0] (word 1).
///
/// @param slot The slot number (0 to 31).
///
/// @return The 20-bit start address (SA[19:0]).
static inline __always_inline uint32_t
scsp_slot_sa_get(uint8_t slot)
{
    assert(slot < SCSP_SLOT_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    const uint16_t sa_hi = scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_KYONEX];
    const uint16_t sa_lo = scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_SA];

    return ((uint32_t)(sa_hi & 0x000F) << 16) | (uint32_t)sa_lo;
}

/// @brief Set the 20-bit waveform start address for slot @p slot.
///
/// Writes SA[19:16] to word 0 bits 3:0 and SA[15:0] to word 1. Word 0 bits
/// 11:4 are kept; bits 15:12 (including KYONEX) are written as 0.
///
/// @param slot The slot number (0 to 31).
/// @param sa   The 20-bit start address (SA[19:0]).
static inline __always_inline void
scsp_slot_sa_set(uint8_t slot, uint32_t sa)
{
    assert(slot < SCSP_SLOT_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SA[19:16]: bits 3:0 of word 0; keep bits 11:4, write bits 15:12 as 0 */
    uint16_t word0 = scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_KYONEX];

    word0 = (word0 & 0x0FF0) | ((sa >> 16) & 0x000F);

    scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_KYONEX] = word0;

    /* SA[15:0]: word 1 */
    scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_SA] = (uint16_t)(sa & 0xFFFF);
}

/// @brief Get the loop start address for slot @p slot.
///
/// @param slot The slot number (0 to 31).
///
/// @return The 16-bit loop start address (LSA[15:0]).
static inline __always_inline uint16_t
scsp_slot_lsa_get(uint8_t slot)
{
    assert(slot < SCSP_SLOT_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    return scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_LSA];
}

/// @brief Set the loop start address for slot @p slot.
///
/// @param slot The slot number (0 to 31).
/// @param lsa  The 16-bit loop start address (LSA[15:0]).
static inline __always_inline void
scsp_slot_lsa_set(uint8_t slot, uint16_t lsa)
{
    assert(slot < SCSP_SLOT_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_LSA] = lsa;
}

/// @brief Get the loop end address for slot @p slot.
///
/// @param slot The slot number (0 to 31).
///
/// @return The 16-bit loop end address (LEA[15:0]).
static inline __always_inline uint16_t
scsp_slot_lea_get(uint8_t slot)
{
    assert(slot < SCSP_SLOT_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    return scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_LEA];
}

/// @brief Set the loop end address for slot @p slot.
///
/// @param slot The slot number (0 to 31).
/// @param lea  The 16-bit loop end address (LEA[15:0]).
static inline __always_inline void
scsp_slot_lea_set(uint8_t slot, uint16_t lea)
{
    assert(slot < SCSP_SLOT_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_LEA] = lea;
}

/// @brief Execute KEY_ON / KEY_OFF for all 32 slots (KYONEX).
///
/// Each slot applies its own KYONB. KYONEX is write-only and needs no 0
/// written afterwards. Implementation: a 16-bit read-modify-write of slot 0's
/// word 0 keeps bits 11:0, sets bit 12 and writes bits 15:13 as 0, so slot 0's
/// settings and KYONB are unchanged.
static inline __always_inline void
scsp_slot_kyonex_execute(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    const uint16_t word0 =
        scsp_ioregs->slots[0].regs.buffer[SCSP_SLOT_WORD_KYONEX];

    /* Keep bits 11:0, set KYONEX (bit 12), clear bits 15:13 */
    scsp_ioregs->slots[0].regs.buffer[SCSP_SLOT_WORD_KYONEX] =
        (word0 & 0x0FFF) | 0x1000;
}

/// @brief Get the record KEY_ON / KEY_OFF bit (KYONB) for slot @p slot.
///
/// @param slot The slot number (0 to 31).
///
/// @return The KYONB bit (bit 11 of word 0).
static inline __always_inline bool
scsp_slot_kyonb_get(uint8_t slot)
{
    assert(slot < SCSP_SLOT_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* KYONB: bit 11 */
    return (scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_KYONEX] & 0x0800) != 0x0000;
}

/// @brief Set the record KEY_ON / KEY_OFF bit (KYONB) for slot @p slot.
///
/// Read-modify-write of word 0: bits 10:0 are kept, bit 11 is set, and bits
/// 15:12 are written as 0, so KEY_ON/OFF is never executed.
///
/// @param slot  The slot number (0 to 31).
/// @param kyonb The KYONB bit value (true = KEY_ON, false = KEY_OFF).
static inline __always_inline void
scsp_slot_kyonb_set(uint8_t slot, bool kyonb)
{
    assert(slot < SCSP_SLOT_COUNT);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    uint16_t word0 = scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_KYONEX];

    /* KYONB: bit 11; bits 15:12 are written as 0 (never execute KYONEX) */
    word0 = (word0 & 0x07FF) | (kyonb ? 0x0800 : 0x0000);

    scsp_ioregs->slots[slot].regs.buffer[SCSP_SLOT_WORD_KYONEX] = word0;
}

/// @}

__END_DECLS

#endif /* !_YAUL_SCSP_SLOT_H_ */
