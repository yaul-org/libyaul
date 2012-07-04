/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
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
