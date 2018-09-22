/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>
#include <string.h>

#include <arp.h>

#include <cpu/instructions.h>

#include "arp-internal.h"

static void _arp_function_nop(void);
static void _arp_function_01(void);
static void _arp_function_09(void);

static void (*_arp_function_table[])(void) = {
        _arp_function_nop,
        _arp_function_01,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_09,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop,
        _arp_function_nop
};

void
arp_function_nonblock(void)
{

        if (!(arp_sync_nonblock())) {
                return;
        }

        uint32_t command;
        command = arp_byte_xchg(0x00) & 0x0F;

        _arp_function_table[command]();
}

static void
_arp_function_nop(void)
{
}

/* Read byte from memory and send to client (download
 * from client's perspective) */
static void
_arp_function_01(void)
{
        uint32_t b;
        uint32_t address;
        uint32_t len;
        uint8_t checksum;

        /* Send some bogus value? */
        arp_long_send(0x00000000);

        len = 0;
        while (true) {
                /* Read address */
                address = arp_long_read();
                /* Read length */
                len = arp_long_read();
                /* Check if we're done with transfer */
                if (len == 0) {
                        /* ACK */
                        arp_byte_xchg('O');
                        arp_byte_xchg('K');

                        /* Call ARP user callback */
                        cpu_instr_trapa(32);
                        return;
                }

                /* Set for ARP callback */
                if (arp_callback.ptr == NULL) {
                        arp_callback.ptr = (void *)address;
                }
                arp_callback.function = 0x01;
                arp_callback.exec = false;
                arp_callback.len += len;

                checksum = 0;
                for (; len > 0; len--, address++) {
                        b = MEMORY_READ(8, address);
                        arp_byte_xchg(b);

                        /* Checksum allow overflow */
                        checksum += b;
                }

                arp_byte_xchg(checksum);
        }
}

/* Upload memory and jump (if requested) */
static void
_arp_function_09(void)
{
        uint32_t b;
        uint32_t addr;
        uint32_t this_addr;
        uint32_t len;
        bool exec;

        /* Read addr */
        addr = arp_long_read();
        this_addr = addr;
        /* Read length */
        len = arp_long_read();
        /* Execute? */
        b = arp_byte_xchg(0x00);
        exec = (b == 0x01);

        /* Set for ARP callback */
        if (arp_callback.ptr == NULL) {
                arp_callback.ptr = (void *)(addr - len);
        }
        arp_callback.function = 0x09;
        arp_callback.exec = exec;
        arp_callback.len += len;

        /* XXX: Blocking */
        for (; len > 0; len--, addr++) {
                b = arp_byte_xchg(b);

                MEMORY_WRITE(8, addr, b);
        }

        /* Only call the ARP user callback for when we get the last
         * chunk of data which just happens to be the start address */
        if ((uint32_t)arp_callback.ptr == this_addr) {
                /* Call ARP user callback */
                cpu_instr_trapa(32);
        }
}
