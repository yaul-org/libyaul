/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Anders Montonen
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <cpu/cache.h>
#include <cpu/divu.h>

#include <usb-cart.h>

#include "usb-cart-internal.h"

void
usb_cart_init(void)
{
        /* Disabling A-bus refresh adds ~20 KiB/s */
        MEMORY_WRITE(32, SCU(AREF), 0x00000000);

        /* Removing wait states doubles speed */
        MEMORY_WRITE(32, SCU(ASR0), 0x00000000);
}

void
usb_cart_long_send(uint32_t w)
{
        usb_cart_byte_send(w >> 24);
        usb_cart_byte_send(w >> 16);
        usb_cart_byte_send(w >> 8);
        usb_cart_byte_send(w & 0xFF);
}

uint32_t
usb_cart_long_read(void)
{
        uint32_t b;
        b = 0;

        b |= (usb_cart_byte_read()) << 24;
        b |= (usb_cart_byte_read()) << 16;
        b |= (usb_cart_byte_read()) << 8;
        b |= (usb_cart_byte_read());

        return b;
}

uint8_t
usb_cart_byte_xchg(uint8_t c)
{
        uint8_t b;
        b = usb_cart_byte_read();

        usb_cart_byte_send(c);

        return b;
}

void
usb_cart_dma_read(void *buffer, uint32_t len)
{
        assert(buffer != NULL);

        assert(len > 0);

        /* Divide transfers by USB_CART_OUT_EP_SIZE and get the
         * remainder */
}
