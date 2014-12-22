/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>

#include "arp-internal.h"

uint32_t
arp_read_long(void)
{
        uint32_t b;

        b = 0;
        b |= (arp_xchg_byte(0x00)) << 24;
        b |= (arp_xchg_byte(0x00)) << 16;
        b |= (arp_xchg_byte(0x00)) << 8;
        b |= (arp_xchg_byte(0x00));

        return b;
}
