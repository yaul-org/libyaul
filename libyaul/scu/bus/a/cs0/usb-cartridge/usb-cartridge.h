/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _USB_CARTRIDGE_H_
#define _USB_CARTRIDGE_H_

#include <sys/cdefs.h>

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern uint32_t usb_cartridge_long_read(void);
extern uint8_t usb_cartridge_byte_read(void);
extern uint8_t usb_cartridge_byte_xchg(uint8_t);
extern void usb_cartridge_byte_send(uint8_t);
extern void usb_cartridge_init(void);
extern void usb_cartridge_long_send(uint32_t);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_USB_CARTRIDGE_H_ */
