/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _USB_CART_H_
#define _USB_CART_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stdbool.h>

#include <usb-cart/map.h>

__BEGIN_DECLS

/* 64-byte packet contains a 2-byte payload */
#define USB_CART_OUT_EP_SIZE 62

static inline bool __always_inline
usb_cart_rxf_full(void)
{
        /* When the RXF# flag is low, this indicates there is still
         * unread data in the internal receive buffer remaining to be
         * read by the downstream FPGA or micro */
        return ((MEMORY_READ(8, USB_CART(FLAGS)) & 0x01) == 0x01);
}

static inline bool __always_inline
usb_cart_txe_full(void)
{
        /* When the TXE# flag is low, this indicates there is enough
         * internal transmit buffer space available for writing data
         * back to the host */
        return ((MEMORY_READ(8, USB_CART(FLAGS)) & 0x02) == 0x02);
}

static inline void __always_inline
usb_cart_rxf_wait(void)
{
        /* When the RXF# flag is low, this indicates there is still
         * unread data in the internal receive buffer remaining to be
         * read by the downstream FPGA or micro */
        while ((usb_cart_rxf_full()));
}

static inline void __always_inline
usb_cart_txe_wait(void)
{
        /* When the TXE# flag is low, this indicates there is enough
         * internal transmit buffer space available for writing data
         * back to the host */
        while ((usb_cart_txe_full()));
}

static inline uint8_t __always_inline
usb_cart_byte_read(void)
{
        usb_cart_rxf_wait();
        return MEMORY_READ(8, USB_CART(FIFO));
}

static inline void __always_inline
usb_cart_byte_send(uint8_t c)
{
        usb_cart_txe_wait();
        MEMORY_WRITE(8, USB_CART(FIFO), c);
}

extern uint8_t usb_cart_byte_xchg(uint8_t);

extern uint32_t usb_cart_long_read(void);
extern void usb_cart_long_send(uint32_t);

extern void usb_cart_dma_read(void *, uint32_t);
extern void usb_cart_dma_send(const void *, uint32_t);

__END_DECLS

#endif /* !_USB_CART_H_ */
