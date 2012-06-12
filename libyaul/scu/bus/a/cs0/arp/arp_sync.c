/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "arp.h"

#include "arp_internal.h"

void
arp_sync(void)
{
        uint32_t b;

        do {
                /* Send 'D' back iff we receive an 'I' */
                if ((b = arp_xchg_byte('D')) != 'I')
                        continue;
                /* Send 'O' back iff we receive an 'N' */
                b = arp_xchg_byte('O');
        } while (b != 'N');
}
