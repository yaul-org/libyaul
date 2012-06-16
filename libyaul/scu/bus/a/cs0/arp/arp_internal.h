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

/* Helpers specific to this processor */
#define USER_VECTOR(x)  ((x))
#define USER_VECTOR_CALL(x) do {                                              \
        __asm__ __volatile__ ("trapa #" # x "\n"                              \
                "tst #0,r0");                                                 \
} while (false)

enum arp_regs {
        OUTPUT = 0x08,
        STATUS = 0x10,
        INPUT = 0x18
};

void arp_function_trampoline(void);

/* ARP user callback */
arp_callback_t arp_callback;

#endif /* !_ARP_INTERNAL_H_ */
