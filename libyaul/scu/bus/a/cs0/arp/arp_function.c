/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include "arp.h"

#include "arp_internal.h"


#include <cons/vdp2.h>

void
arp_function(void)
{
        uint32_t b;

        uint32_t address;
        uint32_t len;
        uint8_t checksum;

        arp_sync();

        b = arp_xchg_byte(0x00);
        switch (b) {
        case 0x01:
                /* Read byte from memory and send to client (download
                 * from client's perspective) */

                /* 1. Send some bogus value? */
                arp_send_long(0x00000000);

                while (true) {
                        /* 2. Read address */
                        address = arp_read_long();
                        /* 2. Read length */
                        len = arp_read_long();

                        if (len == 0) {
                                /* ACK */
                                arp_xchg_byte('O');
                                arp_xchg_byte('K');
                                return;
                        }

                        checksum = 0;
                        for (; len > 0; len--, address++) {
                                b = *(volatile uint8_t *)address;
                                arp_xchg_byte(b);

                                /* Checksum allow overflow */
                                checksum += b;
                        }

                        arp_xchg_byte(checksum);
                }
                break;
        case 0x08:
                /* Write byte from client to memory (upload from
                 * client's perspective) */
                return;
        case 0x09:
                /* Upload memory and jump */
                return;
        }
}
