/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>

#include "arp-internal.h"

bool
arp_sync_nonblock(void)
{
        uint32_t b;

        /* Send 'D' back iff we receive an 'I' */
        if ((b = arp_xchg_byte('D')) != 'I')
                return false;
        /* Send 'O' back iff we receive an 'N' */
        if ((b = arp_xchg_byte('O')) != 'N')
                return false;

        return true;
}
