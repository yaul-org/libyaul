/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SP      0x00
#define R0      0x04
#define R1      0x08
#define R2      0x0C
#define R3      0x10
#define R4      0x14
#define R5      0x18
#define R6      0x1C
#define R7      0x20
#define R8      0x24
#define R9      0x28
#define R10     0x2C
#define R11     0x30
#define R12     0x34
#define R13     0x38
#define R14     0x3C
#define MACL    0x40
#define MACH    0x44
#define VBR     0x48
#define GBR     0x50
#define PR      0x54
#define PC      0x58
#define SR      0x5C

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
} __attribute__ ((packed));

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_REGISTERS_H_ */
