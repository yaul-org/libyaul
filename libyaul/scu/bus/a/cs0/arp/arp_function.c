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
        bool execute;

        arp_sync();

        b = arp_xchg_byte(0x00);
        switch (b) {
        case 0x01:
                /* Read byte from memory and send to client (download
                 * from client's perspective) */

                /* Send some bogus value? */
                arp_send_long(0x00000000);

                while (true) {
                        /* Read address */
                        address = arp_read_long();
                        /* Read length */
                        len = arp_read_long();

                        if (len == 0) {
                                /* ACK */
                                arp_xchg_byte('O');
                                arp_xchg_byte('K');
                                return;
                        }

                        checksum = 0;
                        for (; len > 0; len--, address++) {
                                b = MEM_READ(address);
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

                /* Read address */
                address = arp_read_long();
                b = arp_xchg_byte(0x00);
                MEM_POKE(address, b);
                return;
        case 0x09:
                /* Upload memory and jump */
                /* Read address */
                address = arp_read_long();
                /* Read length */
                len = arp_read_long();
                execute = (arp_xchg_byte(0x00) == 0x01);

                for (; len > 0; len--, address++) {
                        b = arp_xchg_byte(b);
                        MEM_POKE(address, b);
                }

                if (execute)
                        ((void (*)(void))address)();
                return;
        }
}
