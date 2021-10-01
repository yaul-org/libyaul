/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_INSTRUCTIONS_H_
#define _YAUL_CPU_INSTRUCTIONS_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

/// @addtogroup CPU
/// @{

/// @brief Emit a `trap` instruction.
///
/// @param vector Interrupt vector number (`rn`).
static inline void __always_inline
cpu_instr_trapa(const uint8_t vector)
{
        __asm__ volatile ("trapa %[n]"
            : /* No outputs */
            : [n] "n" (vector));
}

/// @brief Emit a `swap.b` instruction.
///
/// @details Swap the upper and lower bytes of a 2-byte value.
/// 
/// @param x Value to be swapped.
///
/// @see cpu_bswap16
static inline uint32_t __always_inline
cpu_instr_swapb(uint32_t x)
{
        register uint32_t out;

        __asm__ volatile ("swap.b %[in], %[out]\n"
                : [out] "=&r" (out)
                : [in] "r" (x));

        return out;
}

/// @brief Emit a `swap.w` instruction.
///
/// @details Swap the upper and lower 2-bytes of a 4-byte value.
/// 
/// @param x Value to be swapped.
static inline uint32_t __always_inline
cpu_instr_swapw(uint32_t x)
{
        register uint32_t out;

        __asm__ volatile ("swap.w %[in], %[out]\n"
                : [out] "=&r" (out)
                : [in] "r" (x));

        return out;
}

/// @brief Emit a `clrmac` instruction.
static inline void __always_inline
cpu_instr_clrmac(void)
{
        __asm__ volatile ("clrmac"
            : /* No outputs */
            : /* No inputs */
            : "mach", "macl");
}

/// @brief Emit a `mac.w` instruction.
///
/// @param a The `rm` register.
/// @param b The `rn` register.
static inline void __always_inline
cpu_instr_macw(void *a, void *b)
{
        register uint16_t **ap = (uint16_t **)a;
        register uint16_t **bp = (uint16_t **)b;

        __asm__ volatile ("mac.w @%[a]+, @%[b]+"
            : [a] "+&r" (*ap),
              [b] "+&r" (*bp)
            : /* No inputs */
            : "mach", "macl", "memory");
}

/// @brief Emit a `mac.l` instruction.
///
/// @param a The `rm` register.
/// @param b The `rn` register.
static inline void __always_inline
cpu_instr_macl(void *a, void *b)
{
        register uint32_t **ap = (uint32_t **)a;
        register uint32_t **bp = (uint32_t **)b;

        __asm__ volatile ("mac.l @%[a]+, @%[b]+"
            : [a] "+&r" (*ap),
              [b] "+&r" (*bp)
            : /* No inputs */
            : "mach", "macl", "memory");
}

/// @brief Emit a `sts` instruction to fetch `mach`.
/// @returns The value of the `mach` register.
static inline uint32_t __always_inline
cpu_instr_sts_mach(void)
{
        register uint32_t out;

        __asm__ volatile ("sts mach, %[out]"
            : [out] "=r" (out)
            : /* No inputs */
            : "0");

        return out;
}

/// @brief Emit a `sts` instruction to fetch `macl`.
/// @returns The value of the `macl` register.
static inline uint32_t __always_inline
cpu_instr_sts_macl(void)
{
        register uint32_t out;

        __asm__ volatile ("sts macl, %[out]"
            : [out] "=r" (out)
            : /* No inputs */
            : "0");

        return out;
}

/// @brief Emit a `exts.w` instruction.
///
/// @param rm The `rm` register.
///
/// @returns The sign-extended 32-bit value.
static inline uint32_t __always_inline
cpu_instr_extsw(const uint32_t rm)
{
        register uint32_t rn;

        __asm__ volatile ("exts.w %[rm], %[rn]"
            : [rn] "=&r" (rn)
            : [rm] "r" (rm));

        return rn;
}

/// @brief Emit a `neg` instruction.
///
/// @param rm The `rm` register.
///
/// @returns The negated 32-bit value.
static inline uint32_t __always_inline
cpu_instr_neg(uint32_t rm)
{
        register uint32_t rn;

        __asm__ volatile ("neg %[rm], %[rn]"
            : [rn] "=&r" (rn)
            : [rm] "r" (rm));

        return rn;
}

/// @brief Emit a `rotl` instruction.
///
/// @param rn The `rn` register.
///
/// @returns The rotated to the left 32-bit value.
static inline uint32_t __always_inline
cpu_instr_rotl(uint32_t rn)
{
        __asm__ volatile ("rotl %[rn]"
            : [rn] "=&r" (rn)
            : "0" (rn));

        return rn;
}

/// @brief Emit a `rotr` instruction.
///
/// @param rn The `rn` register.
///
/// @returns The rotated to the right 32-bit value.
static inline uint32_t __always_inline
cpu_instr_rotr(uint32_t rn)
{
        __asm__ volatile ("rotr %[rn]"
            : [rn] "=&r" (rn)
            : "0" (rn));

        return rn;
}


/// @brief Emit a `xtrct` instruction.
///
/// @param rm The `rm` register.
/// @param rn The `rn` register.
///
/// @returns The 32-bit extracted value between the 64-bits of coupled `rm` and
/// `rn` registers.
static inline uint32_t __always_inline
cpu_instr_xtrct(uint32_t rm, uint32_t rn)
{
        __asm__ volatile ("xtrct %[rm], %[rn]"
            : [rn] "=&r" (rn)
            : "0" (rn), [rm] "r" (rm));

        return rn;
}

/// @brief Emit a `nop` instruction.
static inline void __always_inline
cpu_instr_nop(void)
{
        __asm__ volatile ("nop"
            : /* No outputs */
            : /* No inputs */
        );
}

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_INSTRUCTIONS_H_ */
