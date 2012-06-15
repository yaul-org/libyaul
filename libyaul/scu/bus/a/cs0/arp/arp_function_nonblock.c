/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <arp.h>
#include <cons/vdp2.h>

#include <stdlib.h>
#include <string.h>

#include "arp_internal.h"

static void arp_function_01(void);
static void arp_function_08(void);
static void arp_function_09(void);

void
arp_function_nonblock(void)
{
        uint32_t b;

        if (!(arp_sync_nonblock()))
                return;

        b = arp_xchg_byte(0x00);
        switch (b) {
        case 0x01:
                arp_function_01();
                break;
        case 0x08:
                arp_function_08();
                break;
        case 0x09:
                arp_function_09();
        default:
                return;
        }

        /* Call ARP user callback */
        USER_VECTOR_CALL(32);
}

/* Read byte from memory and send to client (download
 * from client's perspective) */
static void
arp_function_01(void)
{
        uint32_t b;
        uint32_t address;
        uint32_t len;
        uint8_t checksum;

        /* Set for ARP callback */
        arp_callback.function = 0x01;

        /* Send some bogus value? */
        arp_send_long(0x00000000);

        /* XXX Still blocking */
        len = 0;
        while (true) {
                /* Read address */
                address = arp_read_long();
                /* Read length */
                len = arp_read_long();

                /* Set for ARP callback */
                if (arp_callback.ptr == NULL)
                        arp_callback.ptr = (void *)address;
                arp_callback.len += len;

                if (len == 0) {
                        /* ACK */
                        arp_xchg_byte('O');
                        arp_xchg_byte('K');
                        break;
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
}

/* Write byte from client to memory (upload from
 * client's perspective) */
static void
arp_function_08(void)
{
        uint32_t b;
        uint32_t address;

        /* Read address */
        address = arp_read_long();

        /* Set for ARP callback */
        arp_callback.function = 0x08;
        arp_callback.ptr = (void *)address;
        arp_callback.len = 1;

        b = arp_xchg_byte(0x00);
        MEM_POKE(address, b);

}

/* Upload memory and jump */
static void
arp_function_09(void)
{
        uint32_t b;
        uint32_t address;
        uint32_t len;
        bool execute;

        /* Read address */
        address = arp_read_long();
        /* Read length */
        len = arp_read_long();
        /* Execute? */
        execute = arp_xchg_byte(0x00) == 0x01;

        /* Set for ARP callback */
        arp_callback.function = 0x09;
        arp_callback.ptr = (void *)address;
        arp_callback.len += len;

        /* XXX Still blocking */
        for (; len > 0; len--, address++) {
                b = arp_xchg_byte(b);
                MEM_POKE(address, b);
        }

        /* Only call the ARP user callback for when we get the last
         * chunk of data */
        if (execute) {
                /* Call ARP user callback */
                USER_VECTOR_CALL(32);
        }
}
