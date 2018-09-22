/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>

#include "arp-internal.h"

uint8_t
arp_byte_xchg(uint8_t c)
{
        while ((arp_busy_status()));

        /* Read back a byte */
        uint8_t b;
        b = MEMORY_READ(8, ARP(INPUT));

        MEMORY_WRITE(8, ARP(OUTPUT), c);

        return b;
}
