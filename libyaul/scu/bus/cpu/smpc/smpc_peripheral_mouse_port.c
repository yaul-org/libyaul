/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/peripheral.h>

#include "smpc-internal.h"

struct smpc_peripheral_mouse *
smpc_peripheral_mouse_port(uint8_t port)
{
        struct smpc_peripheral_mouse *port1;
        struct smpc_peripheral_mouse *port2;

        port1 = (struct smpc_peripheral_mouse *)&smpc_peripheral_port1.info;
        port2 = (struct smpc_peripheral_mouse *)&smpc_peripheral_port2.info;

        switch (port) {
        case 1:
                return port1;
        case 2:
                return port2;
        default:
                return NULL;
        }
}
