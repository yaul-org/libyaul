/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dbgio.h>

#include <usb-cart.h>

#include "../dbgio-internal.h"

#define STATE_IDLE              0x00
#define STATE_INITIALIZED       0x01
#define STATE_BUFFER_DIRTY      0x02
#define STATE_BUFFER_FLUSHING   0x04

static void _init(const dbgio_usb_cart_t *);
static void _deinit(void);
static void _buffer(const char *);
static void _flush(void);

typedef struct {
        uint8_t *buffer;
        uint8_t *buffer_p;
        uint32_t buffer_size;

        uint8_t state;
} dev_state_t;

static const dbgio_usb_cart_t _default_params = {
        .buffer_size = 4096
};

static dev_state_t *_dev_state;

const dbgio_dev_ops_t _internal_dev_ops_usb_cart = {
        .dev = DBGIO_DEV_USB_CART,
        .default_params = &_default_params,
        .init = (void (*)(const void *))_init,
        .deinit = _deinit,
        .buffer = _buffer,
        .flush = _flush
};

static void
_init(const dbgio_usb_cart_t *params)
{
        assert(params != NULL);

        if (_dev_state == NULL) {
                _dev_state = malloc(sizeof(dev_state_t));

                (void)memset(_dev_state, 0x00, sizeof(dev_state_t));
        }
        assert(_dev_state != NULL);

        /* Resize the buffer if needed */
        if ((_dev_state->buffer != NULL) &&
            (_dev_state->buffer_size < params->buffer_size)) {
                free(_dev_state->buffer);

                _dev_state->buffer = NULL;
        }

        if (_dev_state->buffer == NULL) {
                _dev_state->buffer = malloc(params->buffer_size);

                (void)memset(_dev_state->buffer, '\0', params->buffer_size);
        }
        assert(_dev_state->buffer != NULL);

        _dev_state->buffer_p = _dev_state->buffer;
        _dev_state->buffer_size = params->buffer_size;

        _dev_state->state = STATE_INITIALIZED;
}

static void
_deinit(void)
{
        if ((_dev_state->state & STATE_INITIALIZED) != STATE_INITIALIZED) {
                return;
        }

        free(_dev_state->buffer);
        free(_dev_state);

        _dev_state = NULL;
}

static void
_buffer(const char *buffer)
{
        size_t len;
        len = strlen(buffer);

        uint32_t new_len;
        new_len = (uint32_t)(_dev_state->buffer_p - _dev_state->buffer) + len;

        if (new_len >= _dev_state->buffer_size) {
                return;
        }

        (void)memcpy(_dev_state->buffer_p, buffer, len);

        _dev_state->buffer_p += len;

        _dev_state->state |= STATE_BUFFER_DIRTY;
}

static void
_flush(void)
{
        if ((_dev_state->state & STATE_BUFFER_DIRTY) != STATE_BUFFER_DIRTY) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_FLUSHING;

        uint32_t len;
        len = _dev_state->buffer_p - _dev_state->buffer;

        uint32_t i;
        for (i = 0; i < len; i++) {
                usb_cart_byte_send(_dev_state->buffer[i]);
        }

        _dev_state->buffer_p = _dev_state->buffer;

        _dev_state->state &= ~(STATE_BUFFER_DIRTY | STATE_BUFFER_FLUSHING);
}
