/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _USB_CARTRIDGE_MAP_H_
#define _USB_CARTRIDGE_MAP_H_

#include <scu/map.h>

/* Macros specific for processor */
#define USB_CARTRIDGE(x) (CS0(0x01) + ((x) << 20))

#define FIFO            0x01
#define FLAG            0x02

#endif /* !_USB_CARTRIDGE_INTERNAL_H_ */
