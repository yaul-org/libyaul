/*
 * Copyright (c) Israel Jacquez
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
__BEGIN_ASM
static inline void __always_inline
cpu_instr_trapa(const uint8_t vector)
{
    __declare_asm("trapa %[n]"
        : /* No outputs */
        : [n] "n" (vector));
}
__END_ASM

/// @brief Emit a `swap.b` instruction.
///
/// @details Swap the upper and lower bytes of a 2-byte value.
///
/// @param x Value to be swapped.
///
/// @see cpu_bswap16
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_swapb(uint32_t x)
{
    __register uint32_t out;

    __declare_asm("swap.b %[in], %[out]\n"
        : [out] "=&r" (out)
        : [in] "r" (x));

    return out;
}

/// @brief Emit a `swap.w` instruction.
///
/// @details Swap the upper and lower 2-bytes of a 4-byte value.
///
/// @param x Value to be swapped.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_swapw(uint32_t x)
{
    __register uint32_t out;

    __declare_asm("swap.w %[in], %[out]\n"
        : [out] "=&r" (out)
        : [in] "r" (x));

    return out;
}
__END_ASM

/// @brief Emit a `clrmac` instruction.
__BEGIN_ASM
static inline void __always_inline
cpu_instr_clrmac(void)
{
    __declare_asm("clrmac"
        : /* No outputs */
        : /* No inputs */
        : "mach", "macl");
}
__END_ASM

/// @brief Emit a `mac.w` instruction.
///
/// @param a The `rm` register.
/// @param b The `rn` register.
__BEGIN_ASM
static inline void __always_inline
cpu_instr_macw(void *a, void *b)
{
    __register uint16_t **ap = (uint16_t **)a;
    __register uint16_t **bp = (uint16_t **)b;

    __declare_asm("mac.w @%[a]+, @%[b]+"
        : [a] "+&r" (*ap),
          [b] "+&r" (*bp)
        : /* No inputs */
        : "mach", "macl", "memory");
}
__END_ASM

/// @brief Emit a `mac.l` instruction.
///
/// @param a The `rm` register.
/// @param b The `rn` register.
__BEGIN_ASM
static inline void __always_inline
cpu_instr_macl(void *a, void *b)
{
    __register uint32_t **ap = (uint32_t **)a;
    __register uint32_t **bp = (uint32_t **)b;

    __declare_asm("mac.l @%[a]+, @%[b]+"
        : [a] "+&r" (*ap),
          [b] "+&r" (*bp)
        : /* No inputs */
        : "mach", "macl", "memory");
}
__END_ASM

/// @brief Emit a `sts` instruction to fetch `mach`.
/// @returns The value of the `mach` register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_sts_mach(void)
{
    __register uint32_t out;

    __declare_asm("sts mach, %[out]"
        : [out] "=r" (out)
        : /* No inputs */
        : "0");

    return out;
}
__END_ASM

/// @brief Emit a `sts` instruction to fetch `macl`.
/// @returns The value of the `macl` register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_sts_macl(void)
{
    __register uint32_t out;

    __declare_asm("sts macl, %[out]"
        : [out] "=r" (out)
        : /* No inputs */
        : "0");

    return out;
}
__END_ASM

/// @brief Emit a `exts.w` instruction.
///
/// @param rm The `rm` register.
///
/// @returns The sign-extended 32-bit value.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_extsw(const uint32_t rm)
{
    __register uint32_t rn;

    __declare_asm("exts.w %[rm], %[rn]"
        : [rn] "=&r" (rn)
        : [rm] "r" (rm));

    return rn;
}
__END_ASM

/// @brief Emit a `neg` instruction.
///
/// @param rm The `rm` register.
///
/// @returns The negated 32-bit value.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_neg(uint32_t rm)
{
    __register uint32_t rn;

    __declare_asm("neg %[rm], %[rn]"
        : [rn] "=&r" (rn)
        : [rm] "r" (rm));

    return rn;
}
__END_ASM

/// @brief Emit a `rotl` instruction.
///
/// @param rn The `rn` register.
///
/// @returns The rotated to the left 32-bit value.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_rotl(uint32_t rn)
{
    __declare_asm("rotl %[rn]"
        : [rn] "=&r" (rn)
        : "0" (rn));

    return rn;
}
__END_ASM

/// @brief Emit a `rotr` instruction.
///
/// @param rn The `rn` register.
///
/// @returns The rotated to the right 32-bit value.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_rotr(uint32_t rn)
{
    __declare_asm("rotr %[rn]"
        : [rn] "=&r" (rn)
        : "0" (rn));

    return rn;
}
__END_ASM

/// @brief Emit a `xtrct` instruction.
///
/// @param rm The `rm` register.
/// @param rn The `rn` register.
///
/// @returns The 32-bit extracted value between the 64-bits of coupled `rm` and
/// `rn` registers.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_instr_xtrct(uint32_t rm, uint32_t rn)
{
    __declare_asm("xtrct %[rm], %[rn]"
        : [rn] "=&r" (rn)
        : "0" (rn), [rm] "r" (rm));

    return rn;
}
__END_ASM

/// @brief Emit a `nop` instruction.
__BEGIN_ASM
static inline void __always_inline
cpu_instr_nop(void)
{
    __declare_asm("nop"
        : /* No outputs */
        : /* No inputs */
    );
}
__END_ASM

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_INSTRUCTIONS_H_ */
