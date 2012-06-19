/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _CPU_INTERNAL_H_
#define _CPU_INTERNAL_H_

/* Write and read directly to specified address. */
#define MEMORY_WRITE(t, x, y) (*(volatile uint ## t ## _t *)(x) = (y))
#define MEMORY_READ(t, x) (*(volatile uint ## t ## _t *)(x))

/* Macros specific for processor. */
#define CPU(x)          (0xFFFFF000 + (x))

enum cpu_regs {
        DRCR0 = 0x0E71,
        DRCR1 = 0x0E72,
        SAR0 = 0x0F80,
        DAR0 = 0x0F84,
        TCR0 = 0x0F88,
        CHCR0 = 0x0F8C,
        SAR1 = 0x0F90,
        DAR1 = 0x0F94,
        TCR1 = 0x0F98,
        CHCR1 = 0x0F9C,
        VCRDMA0 = 0x0FA0,
        VCRDMA1 = 0x0FA8,
        DMAOR = 0x0FB0
};

#endif /* !_CPU_INTERNAL_H_ */
