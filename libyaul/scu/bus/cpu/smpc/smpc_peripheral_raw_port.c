/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/peripheral.h>

#include "smpc-internal.h"

smpc_peripheral_port_t *
smpc_peripheral_raw_port(uint8_t port)
{

        switch (port) {
        case 1:
                return &smpc_peripheral_port_1;
        case 2:
                return &smpc_peripheral_port_2;
        default:
                return NULL;
        }
}
