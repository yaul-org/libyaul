/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_H_
#define _CPU_H_

#include <cpu/intc.h>
#include <cpu/dmac.h>
#include <cpu/frt.h>
#include <cpu/registers.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CPU_CLOCK_SPEED_26MHZ   0
#define CPU_CLOCK_SPEED_28MHZ   1

static inline void __attribute__ ((always_inline))
cpu_nop(void)
{
        __asm__ volatile ("nop"
            : /* No outputs */
            : /* No inputs */
        );
}

extern void cpu_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_H_ */
