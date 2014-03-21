/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <smpc/peripheral.h>

#include <assert.h>

#include "smpc-internal.h"

void
smpc_peripheral_digital_get(struct smpc_peripheral const *peripheral,
    struct smpc_peripheral_digital * const digital)
{
        assert(peripheral != NULL);
        assert(digital != NULL);

        digital->connected = peripheral->connected;
        digital->port = peripheral->port;
        digital->type = peripheral->type;
        digital->size = peripheral->size;

        digital->previous.pressed.raw = *(uint16_t *)&peripheral->previous_data[0];
        digital->current.pressed.raw = *(uint16_t *)&peripheral->data[0];

        uint32_t diff;

        diff = digital->current.pressed.raw ^ digital->previous.pressed.raw;

        digital->current.held.raw = diff & digital->current.pressed.raw;
        digital->current.released.raw = diff & digital->previous.pressed.raw;

        digital->parent = peripheral->parent;
}
