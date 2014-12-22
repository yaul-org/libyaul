/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>

#include "arp-internal.h"

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
