/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCSP_H_
#define _YAUL_SCSP_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include <scu/map.h>
#include <scsp/map.h>
#include <scsp/slot.h>

__BEGIN_DECLS

/// @defgroup SCSP SCSP
/// SCSP common control registers.
///
/// All registers are 16-bit and accessed as whole words, never as bytes.
/// Words with write-only fields (0x400, 0x402, 0x406, 0x408, 0x412-0x416,
/// 0x418-0x41C, SCIRE, SCILV0-SCILV2, MCIEB, MCIRE) are written as whole
/// words, built with the pack helpers, and have no getters: a read cannot
/// recover those fields.
///
/// Register layouts are in @ref MEMORY_MAP_SCSP_IO_REGISTERS. Fields are raw
/// integer values; no value enums are defined.

/// @addtogroup SCSP
/// @{

/// @brief Size in bytes of the sound memory installed in the Saturn.
///
/// 512 KB at sound-CPU address 000000H-07FFFFH; 080000H-0FFFFFH is an
/// uninstalled expansion area and must not be accessed. The range assumes
/// MEM4MB is 1 (smaller when it is 0) and DAC18B is 0. Nothing in the ABI
/// sets those words automatically; see @ref scsp_mvol_pack.
#define SCSP_SOUND_MEMORY_SIZE (0x00080000UL)

/// @brief Take over the SCSP from the sound CPU.
///
/// Halts the sound CPU (68EC000) with the SMPC sound off command and waits
/// until it has stopped, so its program (the BIOS sound driver) can no
/// longer rewrite SCSP registers or sound RAM. Then disables all SCSP
/// interrupts and clears their pending flags.
///
/// Call once, before the SH-2 writes SCSP registers or sound RAM, and before
/// an INTBACK can be issued (for example from a VBlank-OUT handler calling
/// @ref smpc_peripheral_intback_issue): the SMPC command register is not
/// locked, so interleaved commands can deadlock the handler. Call from
/// user_init, before such a handler is installed.
///
/// The wait after the halt is a fixed, conservative delay, chosen with a wide
/// margin over the time the halt actually takes. Not yet tested on hardware.
void scsp_init(void);

/// @brief Pack the master volume word.
///
/// @param mem4mb MEM4MB (bit 9): true for 4Mbit DRAM, false for 1Mbit.
/// @param dac18b DAC18B (bit 8): true for the 18-bit D/A interface.
/// @param mvol MVOL (bits 3:0): output level. Lowering it cannot undo clipping.
///
/// @return The packed word for offset 0x400.
static inline __always_inline uint16_t
scsp_mvol_pack(bool mem4mb, bool dac18b, uint8_t mvol)
{
    return (mem4mb ? 0x0200 : 0x0000) |
           (dac18b ? 0x0100 : 0x0000) |
           (uint16_t)(mvol & 0x0F);
}

/// @brief Write the master volume word (SCSP_MVOL, offset 0x400).
///
/// The word is write-only; use @ref scsp_mvol_pack to build it.
static inline __always_inline void
scsp_mvol_set(uint16_t mvol)
{
    /* Bits 15:10 and 7:4 (VER, read-only) must be written as 0 */
    assert((mvol & ~0x030F) == 0x0000);

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    scsp_ioregs->buffer[SCSP_MVOL / 2] = mvol;
}

/// @brief Pack the DSP ring buffer word.
///
/// @param rbl RBL (bits 8:7): length, 0: 8K, 1: 16K, 2: 32K, 3: 64K words.
/// @param rbp RBP (bits 6:0, RBP[19:13]): buffer start, on a 4K-word boundary.
///
/// @return The packed word for offset 0x402.
static inline __always_inline uint16_t
scsp_rbl_rbp_pack(uint8_t rbl, uint32_t rbp)
{
    /* 4K words = 0x2000 bytes */
    assert((rbp & 0x00001FFF) == 0x00000000);
    assert(rbl <= 3);
    /* The buffer (8K-64K words) must stay inside installed sound memory,
     * because the DSP's behaviour past its end is unspecified. */
    assert(rbp < SCSP_SOUND_MEMORY_SIZE);
    assert((0x00004000UL << rbl) <= (SCSP_SOUND_MEMORY_SIZE - rbp));

    return (uint16_t)(((rbl & 0x03) << 7) | ((rbp >> 13) & 0x7F));
}

/// @brief Write the DSP ring buffer word (SCSP_RBL_RBP, offset 0x402).
///
/// The word is write-only; use @ref scsp_rbl_rbp_pack to build it.
static inline __always_inline void
scsp_rbl_rbp_set(uint16_t rbl_rbp)
{
    /* Bits 15:9 are reserved. The same limits as scsp_rbl_rbp_pack apply to
     * the decoded length and address. */
    assert((rbl_rbp & 0xFE00) == 0x0000);
    /* Check the start first so the subtraction below cannot wrap */
    assert(((uint32_t)(rbl_rbp & 0x007F) << 13) < SCSP_SOUND_MEMORY_SIZE);
    assert((0x00004000UL << ((rbl_rbp >> 7) & 0x03)) <=
        (SCSP_SOUND_MEMORY_SIZE - ((uint32_t)(rbl_rbp & 0x007F) << 13)));

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    scsp_ioregs->buffer[SCSP_RBL_RBP / 2] = rbl_rbp;
}

/// @brief Get the MIDI input buffer (MIBUF, bits 7:0 of word 0x404).
///
/// @return The MIDI input byte; received data is stored here automatically.
///
/// @note The 31.25Kbps MIDI interface has no peripheral circuit or DIN
/// connector, so MIDI applications cannot be created.
static inline __always_inline uint8_t
scsp_mibuf_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MIBUF: bits 7:0 */
    return scsp_ioregs->buffer[SCSP_MIBUF / 2] & 0x00FF;
}

/// @brief Get the MIDI input overflow flag (MIOVF, bit 10 of word 0x404).
///
/// @return true when data arrived with the MIDI-IN buffer full. An overflow
/// stops MIDI communication, causing a transmission error.
static inline __always_inline bool
scsp_miovf_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MIOVF: bit 10 */
    return (scsp_ioregs->buffer[SCSP_MIBUF / 2] & 0x0400) != 0x0000;
}

/// @brief Get the MIDI input full flag (MIFULL, bit 9 of word 0x404).
///
/// @return true when all 4 bytes of the MIDI-IN buffer hold data.
static inline __always_inline bool
scsp_mifull_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MIFULL: bit 9 */
    return (scsp_ioregs->buffer[SCSP_MIBUF / 2] & 0x0200) != 0x0000;
}

/// @brief Get the MIDI input empty flag (MIEMP, bit 8 of word 0x404).
///
/// @return true when the MIDI input FIFO is empty.
static inline __always_inline bool
scsp_miemp_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MIEMP: bit 8 */
    return (scsp_ioregs->buffer[SCSP_MIBUF / 2] & 0x0100) != 0x0000;
}

/// @brief Get the MIDI output full flag (MOFULL, bit 12 of word 0x404).
///
/// @return true when all 4 bytes of the MIDI-OUT buffer hold data.
static inline __always_inline bool
scsp_mofull_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MOFULL: bit 12 */
    return (scsp_ioregs->buffer[SCSP_MIBUF / 2] & 0x1000) != 0x0000;
}

/// @brief Get the MIDI output empty flag (MOEMP, bit 11 of word 0x404).
///
/// @return true when all MIDI-OUT data has been sent and none is pending.
static inline __always_inline bool
scsp_moemp_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MOEMP: bit 11 */
    return (scsp_ioregs->buffer[SCSP_MIBUF / 2] & 0x0800) != 0x0000;
}

/// @brief Write the MIDI output buffer (MOBUF, bits 7:0 of word 0x406).
///
/// Writing data here sends it to the MIDI-OUT side automatically. MOBUF is
/// write-only; the whole word is written, with the upper bits as 0.
static inline __always_inline void
scsp_mobuf_set(uint8_t mobuf)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MOBUF: bits 7:0 */
    scsp_ioregs->buffer[SCSP_MOBUF / 2] = mobuf;
}

/// @brief Set the monitored slot (MSLC, bits 15:11 of word 0x408).
///
/// Selects the slot (0 to 31) that CA reports. MSLC is write-only; the whole
/// word is written, with the read-only CA bits as 0.
static inline __always_inline void
scsp_mslc_set(uint8_t slot)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MSLC: bits 15:11 */
    scsp_ioregs->buffer[SCSP_MSLC / 2] = (uint16_t)(slot & 0x1F) << 11;
}

/// @brief Get the slot call address (CA, bits 10:7 of word 0x408).
///
/// Counts the monitored slot's output position from SA in 4K (4096) sample
/// steps: 1 means the slot is at least 4K samples past SA.
///
/// @return The CA field of the slot status register.
static inline __always_inline uint8_t
scsp_ca_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* CA: bits 10:7 */
    return (scsp_ioregs->buffer[SCSP_CA / 2] >> 7) & 0x0F;
}

/// @brief Start a DMA transfer.
///
/// Transfers words between the SCSP registers and sound memory, with addresses
/// increasing. The register area is 0x000-0xEE3, so at most 0xEE4 bytes move.
///
/// Writes DMEA (0x412), DRGA/DMEA-high (0x414), then DTLG and control (0x416)
/// with DEXE set, which starts the transfer. DEXE is never written as 0
/// (invalid); it clears itself when the transfer ends.
///
/// @param dmea DMEA[19:1]: sound memory byte address, bit 0 is 0. The transfer
/// must end inside installed sound memory (@ref SCSP_SOUND_MEMORY_SIZE).
/// @param drga DRGA[11:1]: SCSP register byte address, word aligned.
/// @param dtlg DTLG[11:1]: length in bytes, bit 0 is 0, at most 0xEE4.
/// @param ddir DDIR: false for sound memory to registers, true for the reverse.
/// @param dgate DGATE: when true, the destination is 0 cleared first.
///
/// @note The DMA registers cannot change during a transfer, and access to the
/// DMA controller's registers through DMA is prohibited.
static inline __always_inline void
scsp_dma_start(uint32_t dmea, uint16_t drga, uint16_t dtlg, bool ddir,
    bool dgate)
{
    /* Word aligned, 20-bit (DMEA[19:1]) and 12-bit (DRGA[11:1],
     * DTLG[11:1]) fields */
    assert((dmea & 0x00000001) == 0x00000000);
    /* Compare against the remaining memory so the sum cannot wrap */
    assert(dmea < SCSP_SOUND_MEMORY_SIZE);
    assert(dtlg <= (SCSP_SOUND_MEMORY_SIZE - dmea));
    assert((drga & 0x00000001) == 0x00000000);
    assert((drga & ~0x00000FFE) == 0x00000000);
    assert((dtlg & 0x00000001) == 0x00000000);
    assert(dtlg <= 0x0EE4);

    /* Stay inside the register area (0x000-0xEE3) and never touch the
     * DMA registers (0x412-0x416) */
    assert((uint32_t)drga + dtlg <= 0x0EE4);
    assert(((uint32_t)drga + dtlg <= SCSP_DMEA_LO) ||
        (drga >= (SCSP_DTLG + 2)));

    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* DMEA[15:1] */
    scsp_ioregs->buffer[SCSP_DMEA / 2] = dmea & 0xFFFE;

    /* DMEA[19:16] (bits 15:12), DRGA[11:1] (bits 11:1) */
    scsp_ioregs->buffer[SCSP_DRGA / 2] =
        ((dmea & 0x000F0000) >> 4) | (drga & 0x0FFE);

    /* DGATE (bit 14), DDIR (bit 13), DEXE (bit 12), DTLG[11:1] */
    scsp_ioregs->buffer[SCSP_DTLG / 2] =
        (dgate ? 0x4000 : 0x0000) |
        (ddir ? 0x2000 : 0x0000) |
        0x1000 |
        (dtlg & 0x0FFE);
}

/// @brief Check whether a DMA transfer is in progress (DEXE, bit 12 of
/// word 0x416).
///
/// @return true while a transfer is in progress; DEXE clears when it ends.
static inline __always_inline bool
scsp_dma_is_busy(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* DEXE: bit 12 */
    return (scsp_ioregs->buffer[SCSP_DTLG / 2] & 0x1000) != 0x0000;
}

/// @brief Pack the timer A word.
///
/// @param tactl TACTL (bits 10:8): increment cycle for timer A
/// (0: every sample, ..., 7: every 128 samples).
/// @param tima TIMA (bits 7:0): timer A 8-bit up counter. Counting
/// begins immediately after the timer word is set; when all of the bits
/// reach 1, a request for interrupt occurs.
///
/// @return The packed 16-bit word for offset 0x418.
static inline __always_inline uint16_t
scsp_tima_pack(uint8_t tactl, uint8_t tima)
{
    return (uint16_t)(((tactl & 0x07) << 8) | tima);
}

/// @brief Write the timer A word (SCSP_TIMA, offset 0x418).
///
/// The word is write-only; use @ref scsp_tima_pack to build it.
static inline __always_inline void
scsp_tima_set(uint16_t tima)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* Bits 15:11 are reserved */
    scsp_ioregs->buffer[SCSP_TIMA / 2] = tima & 0x07FF;
}

/// @brief Pack the timer B word.
///
/// @param tbctl TBCTL (bits 10:8): increment cycle for timer B
/// (0: every sample, ..., 7: every 128 samples).
/// @param timb TIMB (bits 7:0): timer B 8-bit up counter. Counting
/// begins immediately after the timer word is set; when all of the bits
/// reach 1, a request for interrupt occurs.
///
/// @return The packed 16-bit word for offset 0x41A.
static inline __always_inline uint16_t
scsp_timb_pack(uint8_t tbctl, uint8_t timb)
{
    return (uint16_t)(((tbctl & 0x07) << 8) | timb);
}

/// @brief Write the timer B word (SCSP_TIMB, offset 0x41A).
///
/// The word is write-only; use @ref scsp_timb_pack to build it.
static inline __always_inline void
scsp_timb_set(uint16_t timb)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* Bits 15:11 are reserved */
    scsp_ioregs->buffer[SCSP_TIMB / 2] = timb & 0x07FF;
}

/// @brief Pack the timer C word.
///
/// @param tcctl TCCTL (bits 10:8): increment cycle for timer C
/// (0: every sample, ..., 7: every 128 samples).
/// @param timc TIMC (bits 7:0): timer C 8-bit up counter. Counting
/// begins immediately after the timer word is set; when all of the bits
/// reach 1, a request for interrupt occurs.
///
/// @return The packed 16-bit word for offset 0x41C.
static inline __always_inline uint16_t
scsp_timc_pack(uint8_t tcctl, uint8_t timc)
{
    return (uint16_t)(((tcctl & 0x07) << 8) | timc);
}

/// @brief Write the timer C word (SCSP_TIMC, offset 0x41C).
///
/// The word is write-only; use @ref scsp_timc_pack to build it.
static inline __always_inline void
scsp_timc_set(uint16_t timc)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* Bits 15:11 are reserved */
    scsp_ioregs->buffer[SCSP_TIMC / 2] = timc & 0x07FF;
}

/// @brief Get the sound CPU interrupt enable register (SCIEB).
///
/// @return The raw SCIEB field (bits 10:0). A 1 enables that interrupt for
/// the sound CPU. Bits 15:11 are reserved; use the SCSP_INT_* masks.
static inline __always_inline uint16_t
scsp_scieb_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SCIEB[10:0] */
    return scsp_ioregs->buffer[SCSP_SCIEB / 2] & 0x07FF;
}

/// @brief Set the sound CPU interrupt enable register (SCIEB).
///
/// A 1 enables that interrupt for the sound CPU; bits 15:11 are written as 0.
static inline __always_inline void
scsp_scieb_set(uint16_t scieb)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SCIEB[10:0] */
    scsp_ioregs->buffer[SCSP_SCIEB / 2] = scieb & 0x07FF;
}

/// @brief Get the sound CPU interrupt pending register (SCIPD).
///
/// Flags each sound CPU interrupt request, whatever SCIEB is set to. A flag
/// stays set until its SCIRE bit is written.
///
/// @return The raw SCIPD field (bits 10:0). Bits 15:11 are reserved; use the
/// SCSP_INT_* masks.
static inline __always_inline uint16_t
scsp_scipd_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SCIPD[10:0] */
    return scsp_ioregs->buffer[SCSP_SCIPD / 2] & 0x07FF;
}

/// @brief Apply a CPU manual interrupt to the sound CPU.
///
/// Writes bit 5 (SCSP_INT_CPU_MANUAL) of SCIPD, the only writable bit.
/// Writing 0 is invalid, so this always writes 1.
static inline __always_inline void
scsp_scipd_cpu_manual_interrupt(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SCIPD bit 5 is the only writable bit */
    scsp_ioregs->buffer[SCSP_SCIPD / 2] = SCSP_INT_CPU_MANUAL;
}

/// @brief Set the sound CPU interrupt reset register (SCIRE).
///
/// A 1 resets the matching SCIPD flag. Bits 15:11 are written as 0. SCIRE is
/// write-only, so there is no get function.
static inline __always_inline void
scsp_scire_set(uint16_t scire)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SCIRE[10:0] */
    scsp_ioregs->buffer[SCSP_SCIRE / 2] = scire & 0x07FF;
}

/// @brief Set the sound CPU interrupt level bit 0 register (SCILV0).
///
/// SCILV0 to SCILV2 hold the 3-bit auto-vector level, one bit each (bit 0 in
/// SCILV0, bit 1 in SCILV1, bit 2 in SCILV2). Level 0 applies no interrupt.
///
/// Bits 15:8 are written as 0. SCILV0 is write-only, so no get function.
static inline __always_inline void
scsp_scilv0_set(uint8_t scilv0)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SCILV0[7:0] */
    scsp_ioregs->buffer[SCSP_SCILV0 / 2] = scilv0;
}

/// @brief Set the sound CPU interrupt level bit 1 register (SCILV1).
///
/// Bit 1 of the level code (see @ref scsp_scilv0_set). Bits 15:8 are written
/// as 0; SCILV1 is write-only, so no get function.
static inline __always_inline void
scsp_scilv1_set(uint8_t scilv1)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SCILV1[7:0] */
    scsp_ioregs->buffer[SCSP_SCILV1 / 2] = scilv1;
}

/// @brief Set the sound CPU interrupt level bit 2 register (SCILV2).
///
/// Bit 2 of the level code (see @ref scsp_scilv0_set). Bits 15:8 are written
/// as 0; SCILV2 is write-only, so no get function.
static inline __always_inline void
scsp_scilv2_set(uint8_t scilv2)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* SCILV2[7:0] */
    scsp_ioregs->buffer[SCSP_SCILV2 / 2] = scilv2;
}

/// @brief Set the main CPU interrupt enable register (MCIEB).
///
/// A 1 enables that interrupt for the main CPU; bits 15:11 are written as 0.
/// MCIEB is write-only, so no get function.
static inline __always_inline void
scsp_mcieb_set(uint16_t mcieb)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MCIEB[10:0] */
    scsp_ioregs->buffer[SCSP_MCIEB / 2] = mcieb & 0x07FF;
}

/// @brief Get the main CPU interrupt pending register (MCIPD).
///
/// Flags each main CPU interrupt request, whatever MCIEB is set to. A flag
/// stays set until its MCIRE bit is written.
///
/// @return The raw MCIPD field (bits 10:0). Bits 15:11 are reserved; use the
/// SCSP_INT_* masks.
static inline __always_inline uint16_t
scsp_mcipd_get(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MCIPD[10:0] */
    return scsp_ioregs->buffer[SCSP_MCIPD / 2] & 0x07FF;
}

/// @brief Apply a CPU manual interrupt to the main CPU.
///
/// Writes bit 5 (SCSP_INT_CPU_MANUAL) of MCIPD, the only writable bit.
/// Writing 0 is invalid, so this always writes 1.
static inline __always_inline void
scsp_mcipd_cpu_manual_interrupt(void)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MCIPD bit 5 is the only writable bit */
    scsp_ioregs->buffer[SCSP_MCIPD / 2] = SCSP_INT_CPU_MANUAL;
}

/// @brief Set the main CPU interrupt reset register (MCIRE).
///
/// A 1 resets the matching MCIPD flag. Bits 15:11 are written as 0. MCIRE is
/// write-only, so there is no get function.
static inline __always_inline void
scsp_mcire_set(uint16_t mcire)
{
    volatile scsp_ioregs_t * const scsp_ioregs =
        (volatile scsp_ioregs_t *)SCSP(0x00000000);

    /* MCIRE[10:0] */
    scsp_ioregs->buffer[SCSP_MCIRE / 2] = mcire & 0x07FF;
}

/// @}

__END_DECLS

#endif /* !_YAUL_SCSP_H_ */
