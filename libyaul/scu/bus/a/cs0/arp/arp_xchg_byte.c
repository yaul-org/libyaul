/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>

#include "arp-internal.h"

uint8_t
arp_xchg_byte(uint8_t c)
{
        uint8_t b;

        while ((arp_busy_status()));

        /* Read back a byte */
        b = MEMORY_READ(8, ARP(INPUT));
        /* Write to memory */
        MEMORY_WRITE(8, ARP(OUTPUT), c);

        return b;
}
