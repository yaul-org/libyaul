/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <usb-cart.h>

#include "ssload.h"

void
ssload_sync(void)
{
        while (true) {
                usb_cart_byte_send(SSLOAD_COMM_SYNC);

                const uint8_t ack_xchg = usb_cart_byte_xchg(SSLOAD_COMM_SYNC);

                if (ack_xchg == SSLOAD_COMM_SYNC) {
                        return;
                }
        }
}
