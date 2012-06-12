/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "arp.h"

#include "arp_internal.h"

uint8_t
arp_read_byte(void)
{
        uint8_t b;

        while ((arp_busy_status()));

        b = MEM_READ(ARP(INPUT));
        /* Write back? */
        MEM_POKE(ARP(OUTPUT), b);

        return b;
}
