/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_H_
#define _CPU_H_

#include <cpu/instructions.h>
#include <cpu/cache.h>
#include <cpu/divu.h>
#include <cpu/dmac.h>
#include <cpu/endian.h>
#include <cpu/frt.h>
#include <cpu/intc.h>
#include <cpu/registers.h>
#include <cpu/sync.h>
#include <cpu/wdt.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void cpu_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_H_ */
