/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_USB_CART_H_
#define _YAUL_USB_CART_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stdbool.h>

#include <usb-cart/map.h>

__BEGIN_DECLS

/// @defgroup USB_CART USB Cart

/// @addtogroup USB_CART
/// @{

/// @brief 64-byte packet containing 62-byte payload.
#define USB_CART_OUT_EP_SIZE 62

/// @brief Determine if the receive buffer is full.
///
/// @details When the @c RXF# flag is low, this indicates there is still unread
/// data in the internal receive buffer remaining to be read by the downstream
/// FPGA or micro.
static inline bool __always_inline
usb_cart_rxf_full(void)
{
        return ((MEMORY_READ(8, USB_CART(FLAGS)) & 0x01) == 0x01);
}

/// @brief Determine if the transfer buffer is full.
///
/// @details When the @c TXE# flag is low, this indicates there is enough
/// internal transmit buffer space available for writing data back to the host.
static inline bool __always_inline
usb_cart_txe_full(void)
{
        return ((MEMORY_READ(8, USB_CART(FLAGS)) & 0x02) == 0x02);
}

/// @brief Wait until the receive buffer is not full, but not necessarily empty.
///
/// @details When the @c RXF# flag is low, this indicates there is still unread
/// data in the internal receive buffer remaining to be read by the downstream
/// FPGA or micro.
static inline void __always_inline
usb_cart_rxf_wait(void)
{
        while ((usb_cart_rxf_full())) {
        }
}

/// @brief Wait until the transfer buffer is not full, but not necessarily
/// empty.
///
/// @details When the TXE# flag is low, this indicates there is enough internal
/// transmit buffer space available for writing data back to the host.
static inline void __always_inline
usb_cart_txe_wait(void)
{
        while ((usb_cart_txe_full())) {
        }
}

/// @brief Read a 8-bit value.
///
/// @details Waits until the receive buffer has is non-empty.
///
/// @returns A 8-bit value value read.
static inline uint8_t __always_inline
usb_cart_byte_read(void)
{
        usb_cart_rxf_wait();

        return MEMORY_READ(8, USB_CART(FIFO));
}

/// @brief Write an 8-bit value.
///
/// @details Waits until the transfer buffer is not full.
///
/// @param c The 8-bit value to write.
static inline void __always_inline
usb_cart_byte_send(uint8_t c)
{
        usb_cart_txe_wait();
        MEMORY_WRITE(8, USB_CART(FIFO), c);
}

/// @brief Reads an 8-bit value and writes an 8-bit value.
///
/// @details The function first waits until the receive buffer is non-empty, the
/// reads a byte then stores the value.
///
/// The function then waits until the transfer buffer is not full to send a
/// byte.
///
/// @param c The 8-bit value to exchange.
///
/// @returns The first 8-bit read value.
extern uint8_t usb_cart_byte_xchg(uint8_t c);

/// @brief Read a 32-bit value.
///
/// @details Waits for the receive buffer to be non-empty on every byte.
///
/// @returns The 32-bit read value.
extern uint32_t usb_cart_long_read(void);

/// @brief Write a 32-bit value.
///
/// @note Waits for transfer buffer to be non-full on every byte.
///
/// @param w The 32-bit value.
extern void usb_cart_long_send(uint32_t w);

/// @brief Use DMA to read.
///
/// @details This function is synchronous. CPU-DMAC is used to retrieve buffer
/// in chunks of 62 bytes. Any remaining bytes are received byte per byte.
///
/// @warning This function is rather unstable for unknown reasons.
///
/// @param buffer The buffer to write to.
/// @param len    The size of @p buffer in bytes.
extern void usb_cart_dma_read(void *buffer, uint32_t len);

/// @brief Use DMA to write.
///
/// @details This function is synchronous. CPU-DMAC is used to transfer in
/// chunks of 62 bytes. Any remaining bytes are transferred byte per byte.
///
/// @warning This function is rather unstable for unknown reasons.
///
/// @param buffer The buffer to transfer.
/// @param len    The size of @p buffer in bytes.
extern void usb_cart_dma_send(const void *buffer, uint32_t len);

/// @}

__END_DECLS

#endif /* !_YAUL_USB_CART_H_ */
