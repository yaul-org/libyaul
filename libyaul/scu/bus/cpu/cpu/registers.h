/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CPU_REGISTERS_H_
#define _CPU_REGISTERS_H_

#include <inttypes.h>

#include <common.h>

struct cpu_registers {
        uint32_t sp;
        uint32_t r[15]; /* Registers r0..r14 */
        uint32_t macl;
        uint32_t mach;
        uint32_t vbr;
        uint32_t gbr;
        uint32_t pr;
        uint32_t pc;

#define SR_T_BIT_MASK   0x00000001
        uint32_t sr;
} __packed;

#endif /* !_CPU_REGISTERS_H_ */
