/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_INSTRUCTIONS_H_
#define _CPU_INSTRUCTIONS_H_

#include <sys/cdefs.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

static inline void __attribute__ ((always_inline))
cpu_instr_trapa(const uint8_t vector)
{
        __asm__ volatile ("trapa %[n]"
            : /* No outputs */
            : [n] "n" (vector));
}

static inline void __attribute__ ((always_inline))
cpu_instr_clrmac(void)
{
        __asm__ volatile ("clrmac"
            : /* No outputs */
            : /* No inputs */
            : "mach", "macl");
}

static inline uint32_t __attribute__ ((always_inline))
cpu_instr_xtrct(uint32_t rm, uint32_t rn)
{
        __asm__ volatile ("xtrct %[rm], %[rn]"
            : [rn] "=&r" (rn)
            : "0" (rn), [rm] "r" (rm));

        return rn;
}

static inline void __attribute__ ((always_inline))
cpu_instr_nop(void)
{
        __asm__ volatile ("nop"
            : /* No outputs */
            : /* No inputs */
        );
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_INSTRUCTIONS_H_ */
