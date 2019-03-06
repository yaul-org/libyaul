/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <sys/cdefs.h>

#include <arp.h>

#include <cpu/intc.h>
#include <cpu/instructions.h>

#include "arp-internal.h"

struct arp_callback _arp_callback;

static void (*_arp_cb)(const struct arp_callback *) = NULL;

static void _arp_trampoline(void);
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

uint8_t
arp_byte_read(void)
{
        uint8_t b;

        while ((arp_busy_status()));

        b = MEMORY_READ(8, ARP(INPUT));
        /* Write back */
        MEMORY_WRITE(8, ARP(OUTPUT), b);

        return b;
}

uint8_t
arp_byte_xchg(uint8_t c)
{
        while ((arp_busy_status()));

        /* Read back a byte */
        uint8_t b;
        b = MEMORY_READ(8, ARP(INPUT));

        MEMORY_WRITE(8, ARP(OUTPUT), c);

        return b;
}

void
arp_function_callback_set(void (*cb)(const struct arp_callback *))
{
        /* Disable interrupts */
        uint32_t sr_mask;
        sr_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        /* Clear ARP callback */
        memset(&_arp_callback.ptr, 0x00, sizeof(_arp_callback));

        assert(cb != NULL);
        _arp_cb = cb;
        cpu_intc_ihr_set(32, _arp_trampoline);

        /* Enable interrupts */
        cpu_intc_mask_set(sr_mask);
}

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

uint32_t
arp_long_read(void)
{
        uint32_t b;

        b = 0;
        b |= (arp_byte_xchg(0x00)) << 24;
        b |= (arp_byte_xchg(0x00)) << 16;
        b |= (arp_byte_xchg(0x00)) << 8;
        b |= (arp_byte_xchg(0x00));

        return b;
}

void
arp_long_send(uint32_t w)
{

        arp_byte_xchg(w >> 24);
        arp_byte_xchg(w >> 16);
        arp_byte_xchg(w >> 8);
        arp_byte_xchg(w & 0xFF);
}

void __noreturn
arp_return(void)
{
        ((void (*)(void))0x02000100)();

        while (true) {
        }

        __builtin_unreachable();
}

void
arp_sync(void)
{

        while (!(arp_sync_nonblock()));
}

bool
arp_sync_nonblock(void)
{
        uint32_t b;

        b = MEMORY_READ(8, ARP(INPUT));

        if (b != 'I') {
                return false;
        }

        /* Send 'D' back iff we receive an 'I' */
        if ((b = arp_byte_xchg('D')) != 'I') {
                return false;
        }

        /* Send 'O' back iff we receive an 'N' */
        if ((b = arp_byte_xchg('O')) != 'N') {
                return false;
        }

        return true;
}

char *
arp_version_get(void)
{
        const char *arp_ver;
        char *buf;
        size_t ver_len;

        arp_ver = (const char *)ARP(VERSION);
        ver_len = 255;
        if ((buf = (char *)malloc(ver_len + 1)) == NULL) {
                return NULL;
        }
        (void)memset(buf, '\0', ver_len);
        (void)memcpy(buf, arp_ver, ver_len);
        buf[ver_len] = '\0';

        return buf;
}

static void
_arp_trampoline(void)
{
        _arp_cb(&_arp_callback);

        /* Clear ARP user callback */
        _arp_callback.function = 0x00;
        _arp_callback.ptr = NULL;
        _arp_callback.exec = false;
        _arp_callback.len = 0;
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
                if (_arp_callback.ptr == NULL) {
                        _arp_callback.ptr = (void *)address;
                }
                _arp_callback.function = 0x01;
                _arp_callback.exec = false;
                _arp_callback.len += len;

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
        if (_arp_callback.ptr == NULL) {
                _arp_callback.ptr = (void *)(addr - len);
        }
        _arp_callback.function = 0x09;
        _arp_callback.exec = exec;
        _arp_callback.len += len;

        /* XXX: Blocking */
        for (; len > 0; len--, addr++) {
                b = arp_byte_xchg(b);

                MEMORY_WRITE(8, addr, b);
        }

        /* Only call the ARP user callback for when we get the last
         * chunk of data which just happens to be the start address */
        if ((uint32_t)_arp_callback.ptr == this_addr) {
                /* Call ARP user callback */
                cpu_instr_trapa(32);
        }
}
