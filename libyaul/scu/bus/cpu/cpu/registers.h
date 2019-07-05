/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_REGISTERS_H_
#define _CPU_REGISTERS_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

#define CPU_SR_T_BIT_MASK       0x00000001
#define CPU_SR_S_BIT_MASK       0x00000002
#define CPU_SR_I_BIT_MASK       0x000000F0
#define CPU_SR_Q_BIT_MASK       0x00000100
#define CPU_SR_M_BIT_MASK       0x00000200

struct cpu_registers {
        uint32_t sp;
        uint32_t r[15]; /* Registers r0..r14 */
        uint32_t macl;
        uint32_t mach;
        uint32_t vbr;
        uint32_t gbr;
        uint32_t pr;
        uint32_t pc;
        uint32_t sr;
} __packed;

static inline void __always_inline
cpu_reg_gbr_set(uint32_t reg_gbr)
{
        __asm__ volatile ("ldc %0, gbr"
            : /* No outputs */
            : "r" (reg_gbr));
}

static inline uint32_t __always_inline
cpu_reg_gbr_get(void)
{
        register uint32_t reg_gbr;

        __asm__ volatile ("stc gbr, %0"
            : "=r" (reg_gbr)
            : /* No inputs */
        );

        return reg_gbr;
}

static inline void __always_inline
cpu_reg_vbr_set(uint32_t reg_vbr)
{
        __asm__ volatile ("ldc %0, vbr"
            : /* No outputs */
            : "r" (reg_vbr));
}

static inline uint32_t __always_inline
cpu_reg_vbr_get(void)
{
        register uint32_t reg_vbr;

        __asm__ volatile ("stc vbr, %0"
            : "=r" (reg_vbr)
            : /* No inputs */
        );

        return reg_vbr;
}

static inline void __always_inline
cpu_reg_mach_set(uint32_t reg_mach)
{
        __asm__ volatile ("lds %0, mach"
            : /* No outputs */
            : "r" (reg_mach));
}

static inline uint32_t __always_inline
cpu_reg_mach_get(void)
{
        register uint32_t reg_mach;

        __asm__ volatile ("sts mach, %0"
            : "=r" (reg_mach)
            : /* No inputs */
        );

        return reg_mach;
}

static inline void __always_inline
cpu_reg_macl_set(uint32_t reg_macl)
{
        __asm__ volatile ("lds %0, macl"
            : /* No outputs */
            : "r" (reg_macl));
}

static inline uint32_t __always_inline
cpu_reg_macl_get(void)
{
        register uint32_t reg_macl;

        __asm__ volatile ("sts macl, %0"
            : "=r" (reg_macl)
            : /* No inputs */
        );

        return reg_macl;
}

static inline void __always_inline
cpu_reg_pr_set(uint32_t reg_pr)
{
        __asm__ volatile ("lds %0, pr"
            : /* No outputs */
            : "r" (reg_pr));
}

static inline uint32_t __always_inline
cpu_reg_pr_get(void)
{
        register uint32_t reg_pr;

        __asm__ volatile ("sts pr, %0"
            : "=r" (reg_pr)
            : /* No inputs */
        );

        return reg_pr;
}

static inline void __always_inline
cpu_reg_sr_set(uint32_t reg_sr)
{
        __asm__ volatile ("ldc %0, sr"
            : /* No outputs */
            : "r" (reg_sr));
}

static inline uint32_t __always_inline
cpu_reg_sr_get(void)
{
        register uint32_t reg_sr;

        __asm__ volatile ("stc sr, %0"
            : "=r" (reg_sr)
            : /* No inputs */
        );

        return reg_sr;
}

static inline uint32_t __always_inline
cpu_reg_fp_get(void)
{
        register uint32_t reg_r14;

        __asm__ volatile ("mov r14, %0"
            : "=r" (reg_r14)
            : /* No inputs */
        );

        return reg_r14;
}

static inline uint32_t __always_inline
cpu_reg_sp_get(void)
{
        register uint32_t reg_r15;

        __asm__ volatile ("mov r15, %0"
            : "=r" (reg_r15)
            : /* No inputs */
        );

        return reg_r15;
}

__END_DECLS

#endif /* !_CPU_REGISTERS_H_ */
