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
smpc_peripheral_analog_get(struct smpc_peripheral const *peripheral,
    struct smpc_peripheral_analog * const analog)
{
        assert(peripheral != NULL);
        assert(analog != NULL);

        analog->connected = peripheral->connected;
        analog->port = peripheral->port;
        analog->type = peripheral->type;
        analog->size = peripheral->size;

        memset(&analog->previous.pressed.raw[0], &peripheral->previous_data[0],
            (sizeof(analog->previous.pressed.raw) / sizeof(analog->previous.pressed.raw[0])));
        memset(&analog->pressed.raw[0], &peripheral->data[0],
            (sizeof(analog->pressed.raw) / sizeof(analog->pressed.raw[0])));

        uint16_t diff;
        uint16_t raw;
        uint16_t previous_raw;

        raw = *(uint16_t *)&analog->pressed.raw[0];
        previous_raw = *(uint16_t *)&analog->previous.pressed.raw[0];

        diff = raw ^ previous_raw;

        analog->held.raw = diff & raw;
        analog->released.raw = diff & previous_raw;

        analog->parent = peripheral->parent;
}
