/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "arp.h"

#include "arp_internal.h"

bool
arp_busy_status(void)
{
        uint8_t status;
        bool busy;

        status = MEM_READ(ARP(STATUS));
        status &= 0x01;
        /* Busy? */
        busy = (status == 0x00);

        return busy;
}
