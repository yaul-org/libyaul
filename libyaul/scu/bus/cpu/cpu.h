/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_H_
#define _CPU_H_

#include <cpu/cache.h>
#include <cpu/divu.h>
#include <cpu/dmac.h>
#include <cpu/dual.h>
#include <cpu/endian.h>
#include <cpu/frt.h>
#include <cpu/instructions.h>
#include <cpu/intc.h>
#include <cpu/registers.h>
#include <cpu/sync.h>
#include <cpu/wdt.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern void cpu_init(void);
extern void _slave_polling_entry(void);
extern void _slave_ici_entry(void);
extern void _exception_illegal_instruction(void);
extern void _exception_illegal_slot(void);
extern void _exception_cpu_address_error(void);
extern void _exception_dma_address_error(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_CPU_H_ */
