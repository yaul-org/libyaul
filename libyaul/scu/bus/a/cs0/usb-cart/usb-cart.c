/*
 * Copyright (c) 2012-2019
 * See LICENSE for details.
 *
 * Anders Montonen
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <cpu/cache.h>
#include <cpu/divu.h>
#include <cpu/dmac.h>
#include <cpu/frt.h>

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
        const uint8_t b = usb_cart_byte_read();

        usb_cart_byte_send(c);

        return b;
}

void
usb_cart_dma_read(void *buffer, uint32_t len)
{
        assert(buffer != NULL);

        assert(len > 0);

        if (len == 0) {
                return;
        }

        const cpu_dmac_cfg_t dmac_cfg = {
                .channel  = 0,
                .src_mode = CPU_DMAC_SOURCE_FIXED,
                .dst_mode = CPU_DMAC_DESTINATION_INCREMENT,
                .stride   = CPU_DMAC_STRIDE_1_BYTE,
                .bus_mode = CPU_DMAC_BUS_MODE_CYCLE_STEAL,
                .src      = USB_CART(FIFO),
                .dst      = (uint32_t)buffer,
                .len      = USB_CART_OUT_EP_SIZE,
                .ihr      = NULL
        };

        uint32_t aref_bits;
        aref_bits = MEMORY_READ(32, SCU(AREF));

        uint32_t asr0_bits;
        asr0_bits = MEMORY_READ(32, SCU(ASR0));

        cpu_dmac_channel_wait(0);
        cpu_dmac_channel_config_set(&dmac_cfg);

        /* Disabling A-bus refresh adds ~20 KiB/s */
        MEMORY_WRITE(32, SCU(AREF), 0x00000000);
        /* Set 9-cycle wait was tested manually, and found to be the
         * bare minimum */
        MEMORY_WRITE(32, SCU(ASR0), 0x00900000);

        int32_t len_left;
        len_left = len;

        do {
                len_left -= USB_CART_OUT_EP_SIZE;

                if (len_left < 0) {
                        break;
                }

                usb_cart_rxf_wait();

                cpu_dmac_channel_start(0);
                cpu_dmac_channel_wait(0);

                /* Reset CPU-DMAC transfer */
                cpu_dmac_channel_transfer_set(0, USB_CART_OUT_EP_SIZE);
        } while (len_left > 0);

        MEMORY_WRITE(32, SCU(ASR0), 0x00000000);

        if (len_left < 0) {
                uint32_t count;
                count = (len_left + USB_CART_OUT_EP_SIZE);

                uint32_t i;
                for (i = 0; i < count; i++) {
                        uint32_t offset;
                        offset = (uint32_t)buffer + len - count + i;

                        uint8_t *p;
                        p = (uint8_t *)offset;

                        usb_cart_rxf_wait();
                        *p = usb_cart_byte_read();
                }
        }

        MEMORY_WRITE(32, SCU(AREF), aref_bits);
        MEMORY_WRITE(32, SCU(ASR0), asr0_bits);
}

void
usb_cart_dma_send(const void *buffer, uint32_t len)
{
        assert(buffer != NULL);

        assert(len > 0);

        if (len == 0) {
                return;
        }

        const cpu_dmac_cfg_t dmac_cfg = {
                .channel = 0,
                .src_mode = CPU_DMAC_SOURCE_INCREMENT,
                .dst_mode = CPU_DMAC_DESTINATION_FIXED,
                .stride = CPU_DMAC_STRIDE_1_BYTE,
                .bus_mode = CPU_DMAC_BUS_MODE_CYCLE_STEAL,
                .src = (uint32_t)buffer,
                .dst = USB_CART(FIFO),
                .len = USB_CART_OUT_EP_SIZE,
                .ihr = NULL
        };

        uint32_t aref_bits;
        aref_bits = MEMORY_READ(32, SCU(AREF));

        uint32_t asr0_bits;
        asr0_bits = MEMORY_READ(32, SCU(ASR0));

        cpu_dmac_channel_wait(0);
        cpu_dmac_channel_config_set(&dmac_cfg);

        /* Disabling A-bus refresh adds ~20 KiB/s */
        MEMORY_WRITE(32, SCU(AREF), 0x00000000);
        /* Set 9-cycle wait was tested manually, and found to be the
         * bare minimum */
        MEMORY_WRITE(32, SCU(ASR0), 0x00900000);

        int32_t len_left;
        len_left = len;

        do {
                len_left -= USB_CART_OUT_EP_SIZE;

                if (len_left < 0) {
                        break;
                }

                usb_cart_txe_wait();

                cpu_dmac_channel_start(0);
                cpu_dmac_channel_wait(0);

                /* Reset CPU-DMAC transfer */
                cpu_dmac_channel_transfer_set(0, USB_CART_OUT_EP_SIZE);
        } while (len_left > 0);

        MEMORY_WRITE(32, SCU(ASR0), 0x00000000);

        if (len_left < 0) {
                uint32_t count;
                count = (len_left + USB_CART_OUT_EP_SIZE);

                uint32_t i;
                for (i = 0; i < count; i++) {
                        uint32_t offset;
                        offset = (uint32_t)buffer + len - count + i;

                        const uint8_t *p;
                        p = (const uint8_t *)offset;

                        usb_cart_txe_wait();
                        usb_cart_byte_send(*p);
                }
        }

        MEMORY_WRITE(32, SCU(AREF), aref_bits);
        MEMORY_WRITE(32, SCU(ASR0), asr0_bits);
}
