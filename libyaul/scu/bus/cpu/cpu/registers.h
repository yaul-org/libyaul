/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_CPU_REGISTERS_H_
#define _YAUL_CPU_REGISTERS_H_

#include <stdint.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

/// @defgroup CPU_REGISTERS CPU Registers

/// @addtogroup CPU_REGISTERS
/// @{

/// @brief CPU `sr` register `T` bit that indicates carry/borrow or overflow/underflow.
#define CPU_SR_T_BIT_MASK       0x00000001
/// @brief CPU `sr` register `S` bit is used by the multiply/accumulate instructions.
#define CPU_SR_S_BIT_MASK       0x00000002
/// @brief CPU `sr` register `I` bits are used for the interrupt mask bits.
#define CPU_SR_I_BITS_MASK      0x000000F0
/// @brief CPU `sr` register `Q` bit used by `div0u`, `divu0s`, and `div1` instructions.
#define CPU_SR_Q_BIT_MASK       0x00000100
/// @brief CPU `sr` register `M` bit used by `divu0`, `divu0s`, and `div1` instructions.
#define CPU_SR_M_BIT_MASK       0x00000200

/// @brief Represent the register configuration.
typedef struct cpu_registers {
        /// `sp` register (or `r15`).
        uint32_t sp;
        /// Registers `r0`..`r14`.
        uint32_t r[15];
        /// `macl` register.
        uint32_t macl;
        /// `mach` register.
        uint32_t mach;
        /// `vbr` register.
        uint32_t vbr;
        /// `gbr` register.
        uint32_t gbr;
        /// `pr` register.
        uint32_t pr;
        /// `pc` register.
        uint32_t pc;
        /// `sr` register.
        uint32_t sr;
} __packed __aligned(4) cpu_registers_t;

/// @brief Set the `gbr` register.
///
/// The appropriate `gbr` value will be set depending on which CPU you're
/// executing this inlined function on.
///
/// @warning Yaul may use this register at some point in the future.
///
/// @param reg_gbr The 32-bit value.
__BEGIN_ASM
static inline void __always_inline
cpu_reg_gbr_set(uint32_t reg_gbr)
{
        __declare_asm("ldc %0, gbr"
            : /* No outputs */
            : "r" (reg_gbr));
}
__END_ASM

/// @brief Obtain the 32-bit value of the `gbr` register.
///
/// @details The appropriate `gbr` value will be returned depending on which CPU
/// you're executing this inlined function on.
///
/// @returns The 32-bit value of the `gbr` register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_reg_gbr_get(void)
{
        __register uint32_t reg_gbr;

        __declare_asm("stc gbr, %0"
            : "=r" (reg_gbr)
            : /* No inputs */
        );

        return reg_gbr;
}
__END_ASM

/// @brief Set the `vbr` register.
///
/// @details The `vbr` register defaults to `0x0600000` for the master CPU, and
/// `0x06000400` for the slave CPU.
///
/// The appropriate `vbr` value will be set depending on which CPU you're
/// executing this inlined function on.
///
/// @param reg_vbr The 32-bit value.
__BEGIN_ASM
static inline void __always_inline
cpu_reg_vbr_set(uint32_t reg_vbr)
{
        __declare_asm("ldc %0, vbr"
            : /* No outputs */
            : "r" (reg_vbr));
}
__END_ASM

/// @brief Obtain the 32-bit value of the `vbr` register.
///
/// @details The appropriate `vbr` value will be returned depending on which CPU
/// you're executing this inlined function on.
///
/// @returns The 32-bit value of the `vbr` register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_reg_vbr_get(void)
{
        __register uint32_t reg_vbr;

        __declare_asm("stc vbr, %0"
            : "=r" (reg_vbr)
            : /* No inputs */
        );

        return reg_vbr;
}
__END_ASM

/// @brief Set the `mach` register.
///
/// The appropriate `mach` value will be set depending on which CPU you're
/// executing this inlined function on.
///
/// @param reg_mach The 32-bit value.
__BEGIN_ASM
static inline void __always_inline
cpu_reg_mach_set(uint32_t reg_mach)
{
        __declare_asm("lds %0, mach"
            : /* No outputs */
            : "r" (reg_mach));
}
__END_ASM

/// @brief Obtain the 32-bit value of the `mach` register.
///
/// @details The appropriate `mach` value will be returned depending on which CPU
/// you're executing this inlined function on.
///
/// @returns The 32-bit value of the `mach` register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_reg_mach_get(void)
{
        __register uint32_t reg_mach;

        __declare_asm("sts mach, %0"
            : "=r" (reg_mach)
            : /* No inputs */
        );

        return reg_mach;
}
__END_ASM

/// @brief Set the `macl` register.
///
/// The appropriate `macl` value will be set depending on which CPU you're
/// executing this inlined function on.
///
/// @param reg_macl The 32-bit value.
__BEGIN_ASM
static inline void __always_inline
cpu_reg_macl_set(uint32_t reg_macl)
{
        __declare_asm("lds %0, macl"
            : /* No outputs */
            : "r" (reg_macl));
}
__END_ASM

/// @brief Obtain the 32-bit value of the `macl` register.
///
/// @details The appropriate `macl` value will be returned depending on which CPU
/// you're executing this inlined function on.
///
/// @returns The 32-bit value of the `macl` register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_reg_macl_get(void)
{
        __register uint32_t reg_macl;

        __declare_asm("sts macl, %0"
            : "=r" (reg_macl)
            : /* No inputs */
        );

        return reg_macl;
}
__END_ASM

/// @brief Set the `pr` register
///
/// The appropriate `pr` value will be set depending on which CPU you're
/// executing this inlined function on.
///
/// @param reg_pr The 32-bit value.
__BEGIN_ASM
static inline void __always_inline
cpu_reg_pr_set(uint32_t reg_pr)
{
        __declare_asm("lds %0, pr"
            : /* No outputs */
            : "r" (reg_pr));
}
__END_ASM

/// @brief Obtain the 32-bit value of the `pr` register.
///
/// @details The appropriate `pr` value will be returned depending on which CPU
/// you're executing this inlined function on.
///
/// @returns The 32-bit value of the `pr` register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_reg_pr_get(void)
{
        __register uint32_t reg_pr;

        __declare_asm("sts pr, %0"
            : "=r" (reg_pr)
            : /* No inputs */
        );

        return reg_pr;
}
__END_ASM

/// @brief Set the `sr` register
///
/// The appropriate `sr` value will be set depending on which CPU you're
/// executing this inlined function on.
///
/// @param reg_sr The 32-bit value.
__BEGIN_ASM
static inline void __always_inline
cpu_reg_sr_set(uint32_t reg_sr)
{
        __declare_asm("ldc %0, sr"
            : /* No outputs */
            : "r" (reg_sr));
}
__END_ASM

/// @brief Obtain the 32-bit value of the `sr` register.
///
/// @details The appropriate `sr` value will be returned depending on which CPU
/// you're executing this inlined function on.
///
/// @returns The 32-bit value of the `sr` register.
static inline uint32_t __always_inline
__BEGIN_ASM
cpu_reg_sr_get(void)
{
        __register uint32_t reg_sr;

        __declare_asm("stc sr, %0"
            : "=r" (reg_sr)
            : /* No inputs */
        );

        return reg_sr;
}
__END_ASM

/// @brief Obtain the 32-bit value of the `fp` (or `r14`) register.
///
/// @details The appropriate `fp` (or `r14`) value will be returned depending on
/// which CPU you're executing this inlined function on.
///
/// @note Yaul passes `-fomit-frame-pointer` to GCC, so the the frame pointer is
/// not being tracked.
///
/// @returns The 32-bit value of the `fp` (or `r14`) register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_reg_fp_get(void)
{
        __register uint32_t reg_r14;

        __declare_asm("mov r14, %0"
            : "=r" (reg_r14)
            : /* No inputs */
        );

        return reg_r14;
}
__END_ASM

/// @brief Obtain the 32-bit value of the `sp` register.
///
/// @details The appropriate `sp` value will be returned depending on which CPU
/// you're executing this inlined function on.
///
/// @returns The 32-bit value of the `sp` register.
__BEGIN_ASM
static inline uint32_t __always_inline
cpu_reg_sp_get(void)
{
        __register uint32_t reg_r15;

        __declare_asm("mov r15, %0"
            : "=r" (reg_r15)
            : /* No inputs */
        );

        return reg_r15;
}
__END_ASM

/// @}

__END_DECLS

#endif /* !_YAUL_CPU_REGISTERS_H_ */
