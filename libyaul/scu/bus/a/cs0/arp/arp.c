/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/cdefs.h>

#include <arp.h>
#include <flash.h>

#include <cpu/intc.h>
#include <cpu/instructions.h>

#include "arp-internal.h"

#define VERSION_STRING_LEN (255)

typedef void (*arp_function_t)(void);

static arp_callback_t _callback;
static arp_callback_handler_t _handler = NULL;

static void _arp_function_nop(void);
static void _arp_function_01(void);
static void _arp_function_09(void);

static const arp_function_t _arp_function_table[] = {
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

static inline void __always_inline
_invoke_callback(void)
{
        if (_handler != NULL) {
                _handler(&_callback);
        }

        _callback.function_type = ARP_FUNCTION_TYPE_NONE;
        _callback.ptr = NULL;
        _callback.len = 0;
}

bool
arp_busy_status(void)
{
        const uint8_t status = MEMORY_READ(8, ARP(STATUS)) & 0x01;
        const bool busy = (status == 0x00);

        if (!busy) {
                MEMORY_WRITE(8, ARP(status), 0x00);
        }

        return busy;
}

uint8_t
arp_byte_read(void)
{
        while ((arp_busy_status())) {
        }

        const uint8_t b = MEMORY_READ(8, ARP(INPUT));

        MEMORY_WRITE(8, ARP(OUTPUT), b);

        return b;
}

uint8_t
arp_byte_xchg(uint8_t c)
{
        while ((arp_busy_status())) {
        }

        const uint8_t b = MEMORY_READ(8, ARP(INPUT));

        MEMORY_WRITE(8, ARP(OUTPUT), c);

        return b;
}

void
arp_function_callback_set(arp_callback_handler_t handler)
{
        /* Disable interrupts */
        const uint32_t sr_mask = cpu_intc_mask_get();

        cpu_intc_mask_set(15);

        /* Clear ARP callback */
        (void)memset(&_callback.ptr, 0x00, sizeof(_callback));

        _handler = handler;

        /* Enable interrupts */
        cpu_intc_mask_set(sr_mask);
}

void
arp_function_nonblock(void)
{
        if (!(arp_sync_nonblock())) {
                return;
        }

        const uint32_t command = arp_byte_xchg(0x00) & 0x0F;

        _arp_function_table[command]();
}

uint32_t
arp_long_read(void)
{
        uint32_t b;
        b = 0x00000000;

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
        while (!(arp_sync_nonblock())) {
        }
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

bool
arp_detect(void)
{
        const uint16_t vendor_id = flash_vendor_get();

        return (vendor_id != 0xFFFF);
}

char *
arp_version_string_get(void)
{
        const char * const version_str = (const char *)ARP(VERSION);

        char *buffer;
        if ((buffer = (char *)malloc(VERSION_STRING_LEN + 1)) == NULL) {
                return NULL;
        }

        (void)memset(buffer, '\0', VERSION_STRING_LEN + 1);
        (void)memcpy(buffer, version_str, VERSION_STRING_LEN);

        return buffer;
}

static void
_arp_function_nop(void)
{
}

static void
_arp_function_01(void)
{
        /* Send some bogus value? */
        arp_long_send(0x00000000);

        while (true) {
                uint32_t address;
                /* Read address */
                address = arp_long_read();
                /* Read length */
                uint32_t len;
                len = arp_long_read();
                /* Check if we're done with transfer */
                if (len == 0) {
                        /* ACK */
                        arp_byte_xchg('O');
                        arp_byte_xchg('K');

                        _invoke_callback();
                        return;
                }

                /* Set for ARP callback */
                if (_callback.ptr == NULL) {
                        _callback.ptr = (void *)address;
                }
                _callback.function_type = ARP_FUNCTION_TYPE_DOWNLOAD;
                _callback.len += len;

                uint8_t checksum;
                checksum = 0;
                for (; len > 0; len--, address++) {
                        const uint32_t b = MEMORY_READ(8, address);

                        arp_byte_xchg(b);

                        /* Checksum allow overflow */
                        checksum += b;
                }

                arp_byte_xchg(checksum);
        }
}

static void
_arp_function_09(void)
{
        /* Read addr */
        uint32_t addr;
        addr = arp_long_read();

        uint32_t this_addr;
        this_addr = addr;

        /* Read length */
        uint32_t len;
        len = arp_long_read();

        /* Execute? */
        const uint8_t exec_byte = arp_byte_xchg(0x00);
        const bool exec = (exec_byte == 0x01);

        /* Set for ARP callback */
        if (_callback.ptr == NULL) {
                _callback.ptr = (void *)(addr - len);
        }

        _callback.function_type = (exec) ? ARP_FUNCTION_TYPE_EXEC : ARP_FUNCTION_TYPE_UPLOAD;
        _callback.len += len;

        /* XXX: Blocking */
        for (; len > 0; len--, addr++) {
                const uint8_t b = arp_byte_xchg(exec_byte);

                MEMORY_WRITE(8, addr, b);
        }

        /* Only call the ARP user callback for when we get the last chunk of
         * data which just happens to be the start address */
        if ((uint32_t)_callback.ptr == this_addr) {
                _invoke_callback();
        }
}
