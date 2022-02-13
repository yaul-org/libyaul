/*
 * Copyright (c) 2012-2022 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cart.h>

#include <crc.h>
#include <ssload.h>

#include "iso9660.h"

void
iso9660_sector_usb_cart_read(sector_t sector, void *ptr)
{
        usb_cart_byte_send(SSLOAD_COMM_CMD_ISO9660);

        while (true) {
                usb_cart_long_send(sector);

                uint32_t *dst_p;
                dst_p = (uint32_t *)ptr;

                for (uint32_t i = 0; i < (ISO9660_SECTOR_SIZE / 16); i++) {
                        *dst_p++ = usb_cart_long_read();
                        *dst_p++ = usb_cart_long_read();
                        *dst_p++ = usb_cart_long_read();
                        *dst_p++ = usb_cart_long_read();
                }

                const crc_t crc = crc_calculate(ptr, ISO9660_SECTOR_SIZE);

                usb_cart_byte_send(crc);

                const crc_t read_crc = usb_cart_byte_read();

                if (read_crc == crc) {
                        usb_cart_byte_send(SSLOAD_COMM_RET_OK);
                        usb_cart_byte_read();
                        break;
                }

                usb_cart_byte_send(SSLOAD_COMM_RET_ERROR);
        }
}
