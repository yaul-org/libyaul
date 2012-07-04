/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <arp.h>
#include <cons/vdp2.h>

#include <stdlib.h>
#include <string.h>

#include "arp-internal.h"

static void arp_function_01(void);
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
                return;
        case 0x09:
                arp_function_09();
        default:
                return;
        }
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

        /* Send some bogus value? */
        arp_send_long(0x00000000);

        /* XXX Still blocking */
        len = 0;
        while (true) {
                /* Read address */
                address = arp_read_long();
                /* Read length */
                len = arp_read_long();
                /* Check if we're done with transfer */
                if (len == 0) {
                        /* ACK */
                        arp_xchg_byte('O');
                        arp_xchg_byte('K');

                        /* Call ARP user callback */
                        USER_VECTOR_CALL(32);
                        return;
                }

                /* Set for ARP callback */
                if (arp_callback.ptr == NULL)
                        arp_callback.ptr = (void *)address;
                arp_callback.function = 0x01;
                arp_callback.exec = false;
                arp_callback.len += len;

                checksum = 0;
                for (; len > 0; len--, address++) {
                        b = MEMORY_READ(8, address);
                        arp_xchg_byte(b);

                        /* Checksum allow overflow */
                        checksum += b;
                }

                arp_xchg_byte(checksum);
        }
}

/* Upload memory and jump (if requested) */
static void
arp_function_09(void)
{
        uint32_t b;
        uint32_t addr;
        uint32_t this_addr;
        uint32_t len;
        bool exec;

        /* Read addr */
        addr = arp_read_long();
        this_addr = addr;
        /* Read length */
        len = arp_read_long();
        /* Execute? */
        b = arp_xchg_byte(0x00);
        exec = (b == 0x01);

        /* Set for ARP callback */
        if (arp_callback.ptr == NULL)
                arp_callback.ptr = (void *)(addr - len);
        arp_callback.function = 0x09;
        arp_callback.exec = exec;
        arp_callback.len += len;

        /* XXX
         * Blocking */
        for (; len > 0; len--, addr++) {
                b = arp_xchg_byte(b);
                /* Write to memory */
                MEMORY_WRITE(8, addr, b);
        }

        /* Only call the ARP user callback for when we get the last
         * chunk of data which just happens to be the start address */
        if ((uint32_t)arp_callback.ptr == this_addr) {
                /* Call ARP user callback */
                USER_VECTOR_CALL(32);
        }
}
