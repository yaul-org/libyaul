/*
 * Copyright (c) 2012-2016 Israel Jacquez
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
        busy = (status == 0x00);

        if (!busy) {
                MEMORY_WRITE(8, ARP(status), 0x0);
        }

        return busy;
}
