/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <arp.h>

#include "arp-internal.h"

uint8_t
arp_read_byte(void)
{
        uint8_t b;

        while ((arp_busy_status()));

        b = MEMORY_READ(8, ARP(INPUT));
        /* Write back? */
        MEMORY_WRITE(8, ARP(OUTPUT), b);

        return b;
}
