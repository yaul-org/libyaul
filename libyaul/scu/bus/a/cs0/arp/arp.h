/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_ARP_H_
#define _YAUL_ARP_H_

#include <sys/cdefs.h>

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

/// @defgroup ARP Cartridge ARP

/// @addtogroup ARP
/// @{

/// @brief ARP Function types.
typedef enum arp_function_type {
        /// Invalid function.
        ARP_FUNCTION_TYPE_NONE     = 0,
        /// Download function.
        ARP_FUNCTION_TYPE_DOWNLOAD = 1,
        /// Upload function.
        ARP_FUNCTION_TYPE_UPLOAD   = 2,
        /// Upload and execute function.
        ARP_FUNCTION_TYPE_EXEC     = 3
} arp_function_type_t;

/// @brief Callback type.
typedef struct arp_callback arp_callback_t;

/// @brief Callback handler.
typedef void (*arp_callback_handler_t)(const arp_callback_t *);

/// @brief Not yet documented.
struct arp_callback {
        /// Function recently executed.
        arp_function_type_t function_type;
        /// Address.
        void *ptr;
        /// Length of transfer.
        size_t len;
} __aligned(4);

/// @brief Determine if the ARP is busy.
/// @returns `true` if ARP is busy. Otherwise, `false`.
extern bool arp_busy_status(void);

/// @brief Perform the ARP handshake.
///
/// @details This function blocks until the handshake is successful.
///
/// @returns `true` if the handshake is successful. Otherwise, `false`.
extern bool arp_sync_nonblock(void);

/// @brief Obtain a copy of the ARP cartridge version string.
///
/// @note This function uses @ref malloc.
///
/// @returns A copy of the ARP cartridge version string.
extern char *arp_version_get(void);

/// @brief Read a 32-bit value via the parallel port.
///
/// @note Does not wait for ARP.
///
/// @returns The 32-bit read value.
extern uint32_t arp_long_read(void);

/// @brief Read a 8-bit value via the parallel port.
///
/// @note Does not wait for ARP.
///
/// @returns The 8-bit read value.
extern uint8_t arp_byte_read(void);

/// @brief Reads an 8-bit value and writes an 8-bit value via the parallel port.
///
/// @details The function polls using @ref arp_busy_status until ARP is no
/// longer busy.
///
/// Once the ARP is free, a byte is read and stored, Then immediately, @p c is
/// written (sent).
///
/// @param c The 8-bit value to exchange.
///
/// @returns The 8-bit read value.
extern uint8_t arp_byte_xchg(uint8_t c);

/// @brief Set the ARP function callback handler.
///
/// @details The callback is invoked depending on the function invoked within
/// @ref arp_function_nonblock.
///
/// @param handler The handler.
extern void arp_function_callback_set(arp_callback_handler_t handler);

/// @brief Perform ARP functions.
///
/// @details This function performs the following ARP functions:
///  - Download memory from Saturn to host.
///  - Upload binary to Saturn from host and jump (if requested).
///
/// @warning The function still currently blocks.
extern void arp_function_nonblock(void);

/// @brief Return to the ARP menu.
///
/// @warning This forcefully jumps to the ARP entry point.
extern void arp_return(void) __noreturn;

/// @brief Write a 32-bit value via the parallel port.
///
/// @note Does not wait for ARP.
///
/// @param w The 32-bit value.
extern void arp_long_send(uint32_t w);

/// @brief Perform the ARP handshake.
///
/// @details This function blocks until the handshake is successful.
extern void arp_sync(void);

/// @}

__END_DECLS

#endif /* !_YAUL_ARP_H_ */
