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

#include <internal.h>
#include <dbgio/dbgio-internal.h>

typedef enum state_flags {
    STATE_IDLE            = 0,
    STATE_INITIALIZED     = 1 << 0,
    STATE_BUFFER_DIRTY    = 1 << 1,
    STATE_BUFFER_FLUSHING = 1 << 2
} state_flags_t;

#define BUFFER_FLUSH_POW      5
#define BUFFER_FLUSH_REM_MASK 0x1F

static void _init(const void *params);
static void _deinit(void);
static void _puts(const char *buffer);
static void _printf(const char *format, va_list ap);
static void _flush(void);

static struct {
    FILE file;
} _cookie;

const dbgio_dev_ops_t __dbgio_dev_ops_usb_cart = {
    .dev            = DBGIO_DEV_USB_CART,
    .default_params = NULL,
    .init           = _init,
    .deinit         = _deinit,
    .font_load      = NULL,
    .display_set    = NULL,
    .puts           = _puts,
    .printf         = _printf,
    .flush          = _flush
};

static size_t
_file_write(FILE *f __unused, const uint8_t *s, size_t l)
{
    if (l == 0) {
        return 0;
    }

    /* XXX: Needs to be reworked */
    /* usb_cart_byte_send(SSLOAD_COMM_CMD_LOG); */
    usb_cart_long_send(l);

    for (uint32_t i = 0; i < l; i++) {
        usb_cart_byte_send(*s);
        s++;
    }

    return l;
}

static int
_file_close(FILE *file)
{
    __free(file->buf);

    return 0;
}

static void
_init(const void *params __unused)
{
    void * const buffer = malloc(USB_CART_OUT_EP_SIZE);
    assert(buffer != NULL);

    _cookie.file.write = _file_write;
    _cookie.file.close = _file_close;
    _cookie.file.buf = buffer;
    _cookie.file.buf_size = USB_CART_OUT_EP_SIZE;
}

static void
_deinit(void)
{
    fclose(&_cookie.file);
}

static void
_puts(const char *buffer)
{
    (void)fputs(buffer, &_cookie.file);
}

static void
_printf(const char *format, va_list ap)
{
    (void)vfprintf(&_cookie.file, format, ap);
}

static void
_flush(void)
{
    fflush(&_cookie.file);
}
