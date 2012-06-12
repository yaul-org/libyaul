/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "arp.h"

#include "arp_internal.h"

uint8_t
arp_xchg_byte(uint8_t c)
{
        uint8_t b;

        while ((arp_busy_status()));

        /* Read back a byte */
        b = MEM_READ(ARP(INPUT));
        /* Write to memory */
        MEM_POKE(ARP(OUTPUT), c);

        return b;
}
