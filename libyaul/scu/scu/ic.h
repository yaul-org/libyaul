/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_SCU_IC_H_
#define _YAUL_SCU_IC_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>

#include <scu/map.h>

__BEGIN_DECLS

/// @defgroup SCU_IC SCU IC
/// Not yet documented.

/// @defgroup SCU_IC_HELPERS SCU IC Setters
/// These functions set the interrupt handler for their respective SCU IC
/// interrupt.

/// @addtogroup SCU_IC
/// @{

/// @brief Interrupt vectors.
typedef enum scu_ic_interrupt {
        /// VBLANK-IN interrupt.
        SCU_IC_INTERRUPT_VBLANK_IN       = 0x40,
        /// VBLANK-OUT interrupt.
        SCU_IC_INTERRUPT_VBLANK_OUT      = 0x41,
        /// HBLANK-IN interrupt.
        SCU_IC_INTERRUPT_HBLANK_IN       = 0x42,
        /// SCU timer #0 interrupt.
        SCU_IC_INTERRUPT_TIMER_0         = 0x43,
        /// SCU timer #1 interrupt.
        SCU_IC_INTERRUPT_TIMER_1         = 0x44,
        /// SCU-DSP Program end interrupt.
        SCU_IC_INTERRUPT_DSP_END         = 0x45,
        /// Sound request interrupt.
        SCU_IC_INTERRUPT_SOUND_REQUEST   = 0x46,
        /// SMPC System manager interrupt.
        SCU_IC_INTERRUPT_SYSTEM_MANAGER  = 0x47,
        /// SMPC Pad interrupt.
        SCU_IC_INTERRUPT_PAD_INTERRUPT   = 0x48,
        /// SCU-DMA level 2 end interrupt.
        SCU_IC_INTERRUPT_LEVEL_2_DMA_END = 0x49,
        /// SCU-DMA level 1 end interrupt.
        SCU_IC_INTERRUPT_LEVEL_1_DMA_END = 0x4A,
        /// SCU-DMA level 0 end interrupt.
        SCU_IC_INTERRUPT_LEVEL_0_DMA_END = 0x4B,
        /// SCU-DMA illegal interrupt.
        SCU_IC_INTERRUPT_DMA_ILLEGAL     = 0x4C,
        /// VDP1 sprite end interrupt.
        SCU_IC_INTERRUPT_SPRITE_END      = 0x4D
} scu_ic_interrupt_t;

/// @brief Mask values.
typedef enum scu_ic_mask {
        /// Value of `0`.
        SCU_IC_MASK_NONE            = 0x00000000,
        /// VBLANK-IN mask.
        SCU_IC_MASK_VBLANK_IN       = 0x00000001,
        /// VBLANK-OUT mask.
        SCU_IC_MASK_VBLANK_OUT      = 0x00000002,
        /// HBLANK-IN mask.
        SCU_IC_MASK_HBLANK_IN       = 0x00000004,
        /// SCU timer #0 mask.
        SCU_IC_MASK_TIMER_0         = 0x00000008,
        /// SCU timer #1 mask.
        SCU_IC_MASK_TIMER_1         = 0x00000010,
        /// SCU-DSP Program end mask.
        SCU_IC_MASK_DSP_END         = 0x00000020,
        /// Sound request mask.
        SCU_IC_MASK_SOUND_REQUEST   = 0x00000040,
        /// SMPC System manager mask.
        SCU_IC_MASK_SYSTEM_MANAGER  = 0x00000080,
        /// SMPC Pad mask.
        SCU_IC_MASK_PAD_INTERRUPT   = 0x00000100,
        /// SCU-DMA level 2 end mask.
        SCU_IC_MASK_LEVEL_2_DMA_END = 0x00000200,
        /// SCU-DMA level 1 end mask.
        SCU_IC_MASK_LEVEL_1_DMA_END = 0x00000400,
        /// SCU-DMA level 0 end mask.
        SCU_IC_MASK_LEVEL_0_DMA_END = 0x00000800,
        /// SCU-DMA illegal mask.
        SCU_IC_MASK_DMA_ILLEGAL     = 0x00001000,
        /// VDP1 sprite end mask.
        SCU_IC_MASK_SPRITE_END      = 0x00002000,
        /// A-Bus mask.
        SCU_IC_MASK_A_BUS           = 0x00008000,
        /// Defines all mask values.
        SCU_IC_MASK_ALL             = 0x0000BFFF
} scu_ic_mask_t;

/* GCC complains:
 *   Warning: ISO C restricts enumerator values to range of 'int'. */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
/// @brief Interrupt status bits.
typedef enum scu_ic_status_reg_t {
        /// Value of 0 (zero).
        SCU_IC_IST_NONE            = 0x00000000UL,
        /// HBLANK-IN status bit.
        SCU_IC_IST_VBLANK_IN       = 0x00000001UL,
        /// VBLANK-OUT status bit.
        SCU_IC_IST_VBLANK_OUT      = 0x00000002UL,
        /// HBLANK-IN status bit.
        SCU_IC_IST_HBLANK_IN       = 0x00000004UL,
        /// SCU timer #0 status bit.
        SCU_IC_IST_TIMER_0         = 0x00000008UL,
        /// SCU timer #1 status bit.
        SCU_IC_IST_TIMER_1         = 0x00000010UL,
        /// SCU-DSP Program end status bit.
        SCU_IC_IST_DSP_END         = 0x00000020UL,
        /// Sound request status bit.
        SCU_IC_IST_SOUND_REQUEST   = 0x00000040UL,
        /// SMPC System manager status bit.
        SCU_IC_IST_SYSTEM_MANAGER  = 0x00000080UL,
        /// SMPC Pad status bit.
        SCU_IC_IST_PAD_INTERRUPT   = 0x00000100UL,
        /// SCU-DMA level 2 end status bit.
        SCU_IC_IST_LEVEL_2_DMA_END = 0x00000200UL,
        /// SCU-DMA level 1 end status bit.
        SCU_IC_IST_LEVEL_1_DMA_END = 0x00000400UL,
        /// SCU-DMA level 0 end status bit.
        SCU_IC_IST_LEVEL_0_DMA_END = 0x00000800UL,
        /// SCU-DMA illegal status bit.
        SCU_IC_IST_DMA_ILLEGAL     = 0x00001000UL,
        /// VDP1 sprite end status bit.
        SCU_IC_IST_SPRITE_END      = 0x00002000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_16     = 0x00010000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_15     = 0x00020000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_14     = 0x00040000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_13     = 0x00080000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_12     = 0x00100000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_11     = 0x00200000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_10     = 0x00400000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_09     = 0x00800000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_08     = 0x01000000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_07     = 0x02000000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_06     = 0x04000000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_05     = 0x08000000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_04     = 0x10000000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_03     = 0x20000000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_02     = 0x40000000UL,
        /// External interrupt status bit.
        SCU_IC_IST_EXTERNAL_01     = 0x80000000UL
} scu_ic_status_reg_t;
#pragma GCC diagnostic pop

/// @brief SCU IC priority entry.
typedef struct scu_ic_priority {
        /// CPU `sr` register priority level mask.
        uint16_t sr_mask;
        /// SCU @ref IMS mask.
        scu_ic_interrupt_t scu_mask:16;
} __packed __aligned(4) scu_ic_priority_t;

static_assert(sizeof(scu_ic_priority_t) == 4);

/// @brief Callback type.
/// @see scu_ic_ihr_set
typedef void (*scu_ic_ihr_t)(void);

/// @brief Set the interrupt handler for the specified SCU related interrupt.
///
/// @details This is a BIOS call. The function should @em not use `rte` to
/// return, unlike @ref cpu_intc_ihr_set.
///
/// It's possible to use @ref cpu_intc_ihr_set to set an SCU related interrupt.
/// However, the @ref IST, as well as the @ref IMS I/O registers need to be
/// updated.
///
/// Below is the disassembly of the BIOS function.
///
/// @code
/// 0x06000794: shll2 r4
/// 0x06000796: tst r5, r5
/// 0x06000798: bf 0x0600079C
/// 0x0600079A: mov.l @(0x014, pc), r5 ! 0x060007AC -> Pointer to empty handler
/// 0x0600079C: mov.l @(0x008, pc), r0 ! 0x060007A4 -> Table of handlers at 0x06000A00
/// 0x0600079E: rts
/// 0x060007A0: mov.l r5, @(r0, r4)
/// 0x060007A2: nop
/// 0x060007A4: 0x06000900
/// 0x060007A8: 0x20000600
/// 0x060007AC: 0x0600083C
/// @endcode
///
/// The actual handler is located at `0x060008F4`:
///
/// @code
/// 0x060008F0: mov.l r0, @-r15
/// 0x060008F2: mov #0x42, r0
/// 0x060008F4: mov.l r1, @-r15        ! Start of handler
/// 0x060008F6: mov.l @(0x064, pc), r1 ! 0x06000958 -> 0x06000348 (SCU interrupt mask)
/// 0x060008F8: mov.l r2, @-r15
/// 0x060008FA: mov.l @r1, r2
/// 0x060008FC: mov.l r3, @-r15
/// 0x060008FE: mov.l r2, @-r15        ! Store initial mask value for trampoline epilogue
/// 0x06000900: mov.l r4, @-r15
/// 0x06000902: mov.l @(0x060, pc), r3 ! 0x06000960 -> 0x06000980
/// 0x06000904: mov.l r5, @-r15
/// 0x06000906: shll2 r0
/// 0x06000908: mov.l @(r0, r3), r3
/// 0x0600090A: mov.l @(0x054, pc), r5 ! 0x0600095C -> 0x25FE00A0 (IMS I/O register)
/// 0x0600090C: mov r3, r4
/// 0x0600090E: shlr16 r3
/// 0x06000910: ldc r3, sr
/// 0x06000912: exts.w r4, r4
/// 0x06000914: or r4, r2
/// 0x06000916: mov.l r2, @r1
/// 0x06000918: mov.l r2, @r5
/// 0x0600091A: mov.l @(0x050, pc), r3 ! 0x06000968, value -> 0x06000900
/// 0x0600091C: mov.l r6, @-r15
/// 0x0600091E: mov.l @(r0, r3), r6
/// 0x06000920: mov.l r7, @-r15
/// 0x06000922: sts.l pr, @-r15
/// 0x06000924: jsr @r6                ! Call user set handler
/// 0x06000926: stc.l gbr, @-r15
/// 0x06000928: ldc.l @r15+, gbr
/// 0x0600092A: lds.l @r15+, pr
/// 0x0600092C: mov.l @r15+, r7
/// 0x0600092E: mov.w @(0x026, pc), r0 ! 0x06000954, value: 0x00F0
/// 0x06000930: mov.l @r15+, r6
/// 0x06000932: mov.l @(0x028, pc), r3 ! 0x06000958, value: 0x06000348
/// 0x06000934: mov.l @r15+, r5
/// 0x06000936: mov.l @(0x028, pc), r1 ! 0x0600095C, value: 0x25FE00A0
/// 0x06000938: mov.l @r15+, r4
/// 0x0600093A: mov.l @r15+, r2
/// 0x0600093C: ldc r0, sr             ! Disables interrupts
/// 0x0600093E: mov.l r2, @r3
/// 0x06000940: mov.l @r15+, r3
/// 0x06000942: mov.l r2, @r1
/// 0x06000944: mov.l @r15+, r2
/// 0x06000946: mov.l @r15+, r1
/// 0x06000948: mov.l @r15+, r0
///
/// 0x0600094A: rte                    ! Default handler
/// 0x0600094C: nop
/// @endcode
///
/// @warning Do not use this function to set your interrupt handler, unless
/// there is an explicit need to do so. To see which functions and macros to use
/// for each interrupt, see @ref SCU_IC_HELPERS.
///
/// @param vector The vector number.
/// @param ihr    The interrupt handler.
static inline void __always_inline
scu_ic_ihr_set(scu_ic_interrupt_t vector, scu_ic_ihr_t ihr)
{
        register uint32_t * const bios_address = (uint32_t *)0x06000300;

        ((void (*)(uint32_t, void (*)(void)))*bios_address)(vector, ihr);
}

/// @brief Clear the interrupt handler for the specified SCU related interrupt.
///
/// @details This is a BIOS call.
///
/// @warning Do not use this function to clear your interrupt handler, unless
/// there is an explicit need to do so. To see which functions and macros to use
/// for each interrupt, see @ref SCU_IC_HELPERS.
///
/// @param vector The vector number.
static inline void __always_inline
scu_ic_ihr_clear(scu_ic_interrupt_t vector)
{
        scu_ic_ihr_set(vector, NULL);
}

/// @brief Obtain the interrupt handler for the specified SCU related interrupt.
///
/// @details This is a BIOS call.
///
/// @param vector The vector
static inline scu_ic_ihr_t
scu_ic_ihr_get(scu_ic_interrupt_t vector)
{
        register uint32_t * const bios_address = (uint32_t *)0x06000304;

        return ((void (*(*)(uint32_t))(void))*bios_address)(vector);
}

/// @brief Write a mask value the SCU I/O register @ref IMS.
///
/// @details This is a BIOS call.
///
/// @param mask The 32-bit mask.
static inline void __always_inline
scu_ic_mask_set(scu_ic_mask_t mask)
{
        register uint32_t * const bios_address = (uint32_t *)0x06000340;

        ((void (*)(uint32_t))*bios_address)(mask);
}

/// @brief Change the mask value in the SCU I/O register @ref IMS.
///
/// @details This function is rather confusing. Effectively, the @p and_mask is
/// the 32-bit mask value that enables/disables (masks) interrupts, while @p
/// or_mask is the 32-bit mask value that enables interrupts.
///
/// To disable (mask) the @ref SCU_IC_MASK_VBLANK_IN interrupt,
/// @code{.c}
/// scu_ic_mask_chg(SCU_IC_MASK_ALL, SCU_IC_MASK_VBLANK_IN);
/// @endcode
///
/// To unmask (enable) @ref SCU_IC_MASK_VBLANK_IN, but mask @ref SCU_IC_MASK_SPRITE_END,
/// @code{.c}
/// scu_ic_mask_chg(SCU_IC_MASK_ALL & ~SCU_IC_MASK_VBLANK_IN, SCU_IC_MASK_SPRITE_END);
/// @endcode
///
/// To mask all interrupts,
/// @code{.c}
/// scu_ic_mask_chg(SCU_IC_MASK_NONE, SCU_IC_MASK_ALL);
/// @endcode
///
/// To unmask all interrupts:
/// @code{.c}
/// scu_ic_mask_chg(SCU_IC_MASK_NONE, SCU_IC_MASK_NONE);
/// @endcode
///
/// This is a BIOS call.
///
/// @param and_mask The bitwise AND mask.
/// @param or_mask  The bitwise OR mask.
static inline void __always_inline
scu_ic_mask_chg(scu_ic_mask_t and_mask, scu_ic_mask_t or_mask)
{
        register uint32_t * const bios_address = (uint32_t *)0x06000344;

        ((void (*)(uint32_t, uint32_t))*bios_address)(and_mask, or_mask);
}

/// @brief Obtain the SCU @ref IMS mask.
///
/// @details This is a BIOS call.
///
/// @returns The 32-bit mask value.
static inline scu_ic_mask_t __always_inline
scu_ic_mask_get(void)
{
        register uint32_t * const bios_address = (uint32_t *)0x06000348;

        return (scu_ic_mask_t)*bios_address;
}

/// @brief Obtain the 32-bit SCU @ref IST value.
/// @returns The 32-bit SCU @ref IST value.
static inline scu_ic_status_reg_t __always_inline
scu_ic_status_get(void)
{
        return (scu_ic_status_reg_t)MEMORY_READ(32, SCU(IST));
}

/// @brief Write to the SCU @ref IST register.
///
/// @details Below describes what happens when reading and writing to the @ref
/// IST register:
/// @htmlinclude scu_ic_status_table.html
/// 
/// @param value The 32-bit-value.
static inline void __always_inline
scu_ic_status_set(scu_ic_status_reg_t value)
{
        MEMORY_WRITE(32, SCU(IST), value);
}

/// @brief Change value in the SCU @ref IST register.
///
/// @details This function behaves exactly in the same way as @ref
/// scu_ic_mask_chg, except the SCU @ref IST is changed.
///
/// @param and_mask The bitwise AND mask.
/// @param or_mask  The bitwise OR mask.
static inline void __always_inline
scu_ic_status_chg(scu_ic_status_reg_t and_mask, scu_ic_status_reg_t or_mask)
{
        volatile uint32_t * const reg_ist = (volatile uint32_t *)SCU(IST);

        *reg_ist = (*reg_ist & and_mask) | or_mask;
}

/// @brief Obtain a read-only pointer to the priority table.
///
/// @details The priority table is 128 bytes. Each priority table entry is
/// 32-bits and is split between SR and SCU interrupt mask. Upper 16-bits is the
/// SR interrupt mask value, and the lower 16-bits is the SCU interrupt mask
/// value and is signed extended.
///
/// As an exmple, for @em HBLANK-IN, disable all interrupts, but allow @em
/// VBLANK-IN and @em VBLANK-OUT.
///
/// Below is the decoded table,
/// @htmlinclude scu_ic_priority_table.html
///
/// This is a BIOS call.
///
/// @returns A read-only pointer to the priority table.
static inline const scu_ic_priority_t * __always_inline
scu_ic_priority_table_get(void)
{
        register scu_ic_priority_t * const bios_address =
            (scu_ic_priority_t *)0x06000A80;

        return bios_address;
}

/// @brief Set the priority table.
/// @details For a detailed explanation, see @ref scu_ic_priority_table_get.
///
/// The size of the table is irrelevant, as only 128 bytes will be considered.
///
/// This is a BIOS call.
///
/// @param table Pointer to the priority table.
static inline void __always_inline
scu_ic_priority_table_set(const scu_ic_priority_t *table)
{
        register uint32_t * const bios_address = (uint32_t *)0x06000280;

        ((void (*)(const scu_ic_priority_t *))*bios_address)(table);
}

/// @}

__END_DECLS

#endif /* !_YAUL_SCU_IC_H_ */
