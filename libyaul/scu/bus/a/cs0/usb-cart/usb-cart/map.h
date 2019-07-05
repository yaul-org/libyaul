/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _USB_CART_MAP_H_
#define _USB_CART_MAP_H_

#include <scu/map.h>

/* Macros specific for processor */
#define USB_CART(x) (CS0(0x01) + ((x) << 20))

#define FIFO    0x01
#define FLAGS   0x02

#endif /* !_USB_CART_MAP_H_ */
