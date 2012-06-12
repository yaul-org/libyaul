/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _TIMER_INTERNAL_H_
#define _TIMER_INTERNAL_H_

/* Write and read directly to specified address. */
#define MEM_POKE(x, y)  (*(volatile uint32_t *)(x) = (y))
#define MEM_READ(x)     (*(volatile uint32_t *)(x))

/* Macros specific for processor. */
#define TIMER(x)          (0x25FE0000 + (x))

enum timer_regs {
        T0C = 0x90,
        T1S = 0x94,
        T1MD = 0x98
};

#endif /* !_TIMER_INTERNAL_H_ */
