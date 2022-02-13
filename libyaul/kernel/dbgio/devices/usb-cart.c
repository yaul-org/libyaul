/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <usb-cart.h>

#include <ssload.h>
#include <internal.h>

#define STATE_IDLE              0x00
#define STATE_INITIALIZED       0x01
#define STATE_BUFFER_DIRTY      0x02
#define STATE_BUFFER_FLUSHING   0x04

#define BUFFER_FLUSH_POW        (5)
#define BUFFER_FLUSH_REM_MASK   (0x1F)

static void _init(const dbgio_usb_cart_t *);
static void _deinit(void);
static void _puts(const char *);
static void _flush(void);
static void _font_load(void);

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

const struct dbgio_dev_ops __dev_ops_usb_cart = {
        .dev            = DBGIO_DEV_USB_CART,
        .default_params = &_default_params,
        .init           = (dev_ops_init_t)_init,
        .deinit         = _deinit,
        .font_load      = _font_load,
        .puts           = _puts,
        .flush          = _flush
};

static void
_init(const dbgio_usb_cart_t *params)
{
        assert(params != NULL);

        if (_dev_state == NULL) {
                _dev_state = __malloc(sizeof(dev_state_t));

                (void)memset(_dev_state, 0x00, sizeof(dev_state_t));
        }
        assert(_dev_state != NULL);

        /* Resize the buffer if needed */
        if ((_dev_state->buffer != NULL) &&
            (_dev_state->buffer_size < params->buffer_size)) {
                __free(_dev_state->buffer);

                _dev_state->buffer = NULL;
        }

        if (_dev_state->buffer == NULL) {
                _dev_state->buffer = __malloc(params->buffer_size);

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

        __free(_dev_state->buffer);
        __free(_dev_state);

        _dev_state = NULL;
}

static void
_puts(const char *buffer)
{
        size_t len;
        len = strlen(buffer);

        uint32_t current_len;
        current_len = (uint32_t)(_dev_state->buffer_p - _dev_state->buffer);

        uint32_t new_len;
        new_len = current_len + len;

        if (new_len >= _dev_state->buffer_size) {
                return;
        }

        (void)memcpy(_dev_state->buffer_p, buffer, len);

        _dev_state->buffer_p += len;

        _dev_state->state |= STATE_BUFFER_DIRTY;
}

static void
_buffer_partial_flush(const uint8_t *buffer, uint32_t len)
{
        if (len == 0) {
                return;
        }

        usb_cart_byte_send(SSLOAD_COMM_CMD_LOG);
        usb_cart_long_send(len);

        for (uint32_t i = 0; i < len; i++) {
                usb_cart_byte_send(buffer[i]);
        }
}

static void
_flush(void)
{
        if ((_dev_state->state & STATE_BUFFER_DIRTY) != STATE_BUFFER_DIRTY) {
                return;
        }

        _dev_state->state |= STATE_BUFFER_FLUSHING;

        const uint32_t len = _dev_state->buffer_p - _dev_state->buffer;

        _buffer_partial_flush(&_dev_state->buffer[0], len);

        _dev_state->buffer_p = _dev_state->buffer;

        _dev_state->state &= ~(STATE_BUFFER_DIRTY | STATE_BUFFER_FLUSHING);
}

static void
_font_load(void)
{
}
