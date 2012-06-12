/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#ifndef _ARP_INTERNAL_H_
#define _ARP_INTERNAL_H_

#include <inttypes.h>

/* Write and read directly to specified address */
#define MEM_POKE(x, y)  (*(volatile uint8_t *)(x) = (y))
#define MEM_READ(x)     (*(volatile uint8_t *)(x))

/* Macros specific for processor */
#define ARP(x)          (0x22000001 + ((x) << 16))

enum arp_regs {
        OUTPUT = 0x08,
        STATUS = 0x10,
        INPUT = 0x18
};

#endif /* !_ARP_INTERNAL_H_ */
