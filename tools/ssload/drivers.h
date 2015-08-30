/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef DRIVERS_H_
#define DRIVERS_H_

#include "driver.h"

#define DEVICE_DRIVER_DATALINK          0
#define DEVICE_DRIVER_USB_CARTRIDGE     1

#define DEVICE_DRIVER_DATALINK_STRING           "datalink"
#define DEVICE_DRIVER_USB_CARTRIDGE_STRING      "usb-cartridge"

extern const struct device_driver *device_drivers[];

#endif /* !DRIVERS_H_ */
