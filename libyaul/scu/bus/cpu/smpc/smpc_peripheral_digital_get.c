/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>

#include <smpc/peripheral.h>

#include "smpc-internal.h"

void
smpc_peripheral_digital_get(smpc_peripheral_t *peripheral,
    smpc_peripheral_digital_t *digital)
{
        assert(peripheral != NULL);
        assert(digital != NULL);

        digital->connected = peripheral->connected;
        digital->port = peripheral->port;
        digital->type = peripheral->type;
        digital->size = peripheral->size;

        digital->previous.pressed.raw = *(uint16_t *)&peripheral->previous_data[0];
        digital->pressed.raw = *(uint16_t *)&peripheral->data[0];

        const uint32_t diff = digital->pressed.raw ^ digital->previous.pressed.raw;

        digital->held.raw = diff & digital->pressed.raw;
        digital->released.raw = diff & digital->previous.pressed.raw;

        digital->parent = peripheral->parent;
}
