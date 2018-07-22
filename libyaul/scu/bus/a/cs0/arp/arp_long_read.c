/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>

#include "arp-internal.h"

uint32_t
arp_long_read(void)
{
        uint32_t b;

        b = 0;
        b |= (arp_byte_xchg(0x00)) << 24;
        b |= (arp_byte_xchg(0x00)) << 16;
        b |= (arp_byte_xchg(0x00)) << 8;
        b |= (arp_byte_xchg(0x00));

        return b;
}
