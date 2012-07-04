/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <inttypes.h>

#include <cpu/registers.h>

void exception_cpu_address_error(void);
void exception_dma_address_error(void);
void exception_ihr_cpu_address_error(struct cpu_registers *);
void exception_ihr_dma_address_error(struct cpu_registers *);
void exception_ihr_illegal_instruction(struct cpu_registers *);
void exception_ihr_illegal_slot(struct cpu_registers *);
void exception_illegal_instruction(void);
void exception_illegal_slot(void);

#endif /* !_EXCEPTION_H_ */
