/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>

#include "arp-internal.h"

void
arp_long_send(uint32_t w)
{

        arp_byte_xchg(w >> 24);
        arp_byte_xchg(w >> 16);
        arp_byte_xchg(w >> 8);
        arp_byte_xchg(w & 0xFF);
}
