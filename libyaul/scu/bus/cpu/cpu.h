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
#include <cpu/registers.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CPU_CLOCK_FREQ_26MHZ    0
#define CPU_CLOCK_FREQ_28MHZ    1

uint32_t cpu_clock_freq_get(void);
void cpu_clock_freq_chg(uint32_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CPU_H_ */
