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
#include "dbgio.h"

#include <internal.h>
#include <dbgio/dbgio-internal.h>

#define WRITE_ADDRESS CS0(0x00100001)

static struct {
    FILE file;
} _cookie;

static void _init(const void *params);
static void _deinit(void);
static void _puts(const char *buffer);
static void _printf(const char *format, va_list ap);

const dbgio_dev_ops_t __dbgio_dev_ops_mednafen_debug = {
    .dev            = DBGIO_DEV_MEDNAFEN_DEBUG,
    .default_params = NULL,
    .init           = _init,
    .deinit         = _deinit,
    .font_load      = NULL,
    .puts           = _puts,
    .printf         = _printf,
    .flush          = NULL
};

static size_t
_file_write(FILE *f __unused, const uint8_t *s, size_t l)
{
    for (uint32_t i = 0; i < l; i++) {
        MEMORY_WRITE(8, WRITE_ADDRESS, s[i]);
    }

    return l;
}

static void
_init(const void *params __unused)
{
    /* Nothing else is needed. Use a FILE to direct all writes via vfprintf() */
    _cookie.file.write = _file_write;
}

static void
_deinit(void)
{
    /* No need to close the "file" */
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
