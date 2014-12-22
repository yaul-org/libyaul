/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "arp.h"

#include "arp-internal.h"

bool
arp_busy_status(void)
{
        uint8_t status;
        bool busy;

        status = MEMORY_READ(8, ARP(STATUS));
        status &= 0x01;
        /* Busy? */
        busy = (status == 0x00);

        return busy;
}
