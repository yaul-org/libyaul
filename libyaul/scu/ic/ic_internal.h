/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _IC_INTERNAL_H_
#define _IC_INTERNAL_H_

/* Write and read directly to specified address. */
#define MEM_READ(x)     (*(volatile uint32_t *)(x))

/* Macros specific for processor. */
#define IC(x)          (0x25FE0000 + (x))

enum ic_regs {
        IST = 0xA4
};

#endif /* !_IC_INTERNAL_H_ */
