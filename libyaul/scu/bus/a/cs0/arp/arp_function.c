/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "arp.h"

#include "arp_internal.h"

void
arp_function(void)
{
        uint32_t b;

        arp_sync();

        b = arp_xchg_byte(0x00);
        switch (b) {
        case 0x01:
                /* Read byte from memory and send to client (download
                 * from client's perspective) */
                return;
        case 0x08:
                /* Write byte from client to memory (upload from
                 * client's perspective) */
        case 0x09:
                /* Upload memory and jump */
        }
}
