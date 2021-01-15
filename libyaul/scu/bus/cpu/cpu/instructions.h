/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_INSTRUCTIONS_H_
#define _CPU_INSTRUCTIONS_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

static inline void __always_inline
cpu_instr_trapa(const uint8_t vector)
{
        __asm__ volatile ("trapa %[n]"
            : /* No outputs */
            : [n] "n" (vector));
}

static inline uint32_t __always_inline
cpu_instr_swapb(uint32_t x)
{
        register uint32_t out;

        __asm__ volatile ("swap.b %[in], %[out]\n"
                : [out] "=&r" (out)
                : [in] "r" (x));

        return out;
}

static inline uint32_t __always_inline
cpu_instr_swapw(uint32_t x)
{
        register uint32_t out;

        __asm__ volatile ("swap.w %[in], %[out]\n"
                : [out] "=&r" (out)
                : [in] "r" (x));

        return out;
}

static inline void __always_inline
cpu_instr_clrmac(void)
{
        __asm__ volatile ("clrmac"
            : /* No outputs */
            : /* No inputs */
            : "mach", "macl");
}

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

static inline uint32_t __always_inline
cpu_instr_extsw(const uint32_t rm)
{
        register uint32_t rn;

        __asm__ volatile ("exts.w %[rm], %[rn]"
            : [rn] "=&r" (rn)
            : [rm] "r" (rm));

        return rn;
}

static inline uint32_t __always_inline
cpu_instr_neg(uint32_t rm)
{
        register uint32_t rn;

        __asm__ volatile ("neg %[rm], %[rn]"
            : [rn] "=&r" (rn)
            : [rm] "r" (rm));

        return rn;
}

static inline uint32_t __always_inline
cpu_instr_rotl(uint32_t rn)
{
        __asm__ volatile ("rotl %[rn]"
            : [rn] "=&r" (rn)
            : "0" (rn));

        return rn;
}

static inline uint32_t __always_inline
cpu_instr_rotr(uint32_t rn)
{
        __asm__ volatile ("rotr %[rn]"
            : [rn] "=&r" (rn)
            : "0" (rn));

        return rn;
}

static inline uint32_t __always_inline
cpu_instr_xtrct(uint32_t rm, uint32_t rn)
{
        __asm__ volatile ("xtrct %[rm], %[rn]"
            : [rn] "=&r" (rn)
            : "0" (rn), [rm] "r" (rm));

        return rn;
}

static inline void __always_inline
cpu_instr_nop(void)
{
        __asm__ volatile ("nop"
            : /* No outputs */
            : /* No inputs */
        );
}

__END_DECLS

#endif /* !_CPU_INSTRUCTIONS_H_ */
