/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _DRAM_CARTRIDGE_INTERNAL_H_
#define _DRAM_CARTRIDGE_INTERNAL_H_

#include <inttypes.h>

/* Write and read directly to specified address. */
#define MEM_POKE(x, y)  (*(volatile uint32_t *)(x) = (y))
#define MEM_READ(x)     (*(volatile uint32_t *)(x))

/* Macros specific for processor. */
#define SCU(x)          (0x25FE0000 + (x))
#define CS1(x)          (0x24000000 + (x))
#define DUMMY(x)        (0x25000000 + (x))

enum dram_cartridge_regs {
        ASR0 = 0xB0,
        ASR1 = 0xB4,
        AREF = 0xB8,
        ID = 0x00FFFFFF,
        UNKNOWN = 0x007EFFFC
};

extern uint32_t id;

#endif /* _DRAM_CARTRIDGE_INTERNAL_H_ */
