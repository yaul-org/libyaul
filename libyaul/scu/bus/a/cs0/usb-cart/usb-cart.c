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
#include <cpu/dmac.h>

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

        struct dmac_ch_cfg dmac_cfg = {
                .dcc_ch = 0,
                .dcc_src_mode = DMAC_SOURCE_FIXED,
                .dcc_dst_mode = DMAC_DESTINATION_INCREMENT,
                .dcc_stride = DMAC_STRIDE_1_BYTE,
                .dcc_bus_mode = DMAC_BUS_MODE_CYCLE_STEAL,
                .dcc_src = USB_CART(FIFO),
                .dcc_dst = 0x00000000,
                .dcc_len = 0x00000000,
                .dcc_ihr = NULL
        };

        cpu_dmac_channel_wait(0);

        uint32_t p;
        p = (uint32_t)buffer;

        while (len > 0) {
                uint32_t len_mod;
                len_mod = len & (USB_CART_OUT_EP_SIZE - 1);

                uint32_t offset;
                offset = (len_mod == 0) ? USB_CART_OUT_EP_SIZE : len_mod;

                dmac_cfg.dcc_dst = p;
                dmac_cfg.dcc_len = offset;

                cpu_dmac_channel_config_set(&dmac_cfg);
                usb_cart_rxf_wait();
                cpu_dmac_channel_start(0);

                p += offset;
                len -= offset;

                cpu_dmac_channel_wait(0);
        }
}

void
usb_cart_dma_send(const void *buffer, uint32_t len)
{
        assert(buffer != NULL);

        assert(len > 0);

        struct dmac_ch_cfg dmac_cfg = {
                .dcc_ch = 0,
                .dcc_src_mode = DMAC_SOURCE_INCREMENT,
                .dcc_dst_mode = DMAC_DESTINATION_FIXED,
                .dcc_stride = DMAC_STRIDE_1_BYTE,
                .dcc_bus_mode = DMAC_BUS_MODE_CYCLE_STEAL,
                .dcc_src = 0x00000000,
                .dcc_dst = USB_CART(FIFO),
                .dcc_len = 0x00000000,
                .dcc_ihr = NULL
        };

        cpu_dmac_channel_wait(0);

        uint32_t p;
        p = (uint32_t)buffer;

        while (len > 0) {
                uint32_t len_mod;
                len_mod = len & (USB_CART_OUT_EP_SIZE - 1);

                uint32_t offset;
                offset = (len_mod == 0) ? USB_CART_OUT_EP_SIZE : len_mod;

                dmac_cfg.dcc_src = p;
                dmac_cfg.dcc_len = offset;

                cpu_dmac_channel_config_set(&dmac_cfg);
                usb_cart_txe_wait();
                cpu_dmac_channel_start(0);

                p += offset;
                len -= offset;

                cpu_dmac_channel_wait(0);
        }
}
