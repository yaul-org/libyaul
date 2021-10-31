/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <dbgio.h>

#include <internal.h>

#include "dbgio-internal.h"

/* This is enough for a 352x256 character resolution */
#define SPRINTF_BUFFER_SIZE (1408)

static struct {
        volatile uint32_t state;
        const struct dbgio_dev_ops *dev_ops;
} _dbgio_state;

static const struct dbgio_dev_ops *_dev_ops_table[] = {
        &_internal_dev_ops_null,
        NULL,
        &_internal_dev_ops_vdp2,
        &_internal_dev_ops_vdp2_async,
#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        &_internal_dev_ops_usb_cart,
#else
        NULL,
#endif /* HAVE_DEV_CARTRIDGE */
};

static char *_sprintf_buffer;

void
_internal_dbgio_init(void)
{
        dbgio_dev_default_init(DBGIO_DEV_NULL);

        if (_sprintf_buffer == NULL) {
                _sprintf_buffer = _internal_malloc(SPRINTF_BUFFER_SIZE);
        }
}

void
dbgio_dev_init(dbgio_dev_t dev, const void *params)
{
        assert(params != NULL);
        assert(_dev_ops_table[dev]->init != NULL);

        if (_dbgio_state.dev_ops != _dev_ops_table[dev]) {
                dbgio_dev_deinit();
        }

        _dbgio_state.dev_ops = _dev_ops_table[dev];

        _dbgio_state.dev_ops->init(params);
}

void
dbgio_dev_default_init(dbgio_dev_t dev)
{
        dbgio_dev_init(dev, _dev_ops_table[dev]->default_params);
}

void
dbgio_dev_deinit(void)
{
        if (_dbgio_state.dev_ops == NULL) {
                return;
        }

        assert(_dbgio_state.dev_ops->deinit != NULL);

        _dbgio_state.dev_ops->deinit();

        _dbgio_state.dev_ops = NULL;
}

void
dbgio_dev_font_load(void)
{
        assert(_dbgio_state.dev_ops != NULL);

        _dbgio_state.dev_ops->font_load();
}

void
dbgio_puts(const char *buffer)
{
        assert(_dbgio_state.dev_ops != NULL);

        assert(buffer != NULL);

        if (*buffer == '\0') {
                return;
        }

        _dbgio_state.dev_ops->puts(buffer);
}

void
dbgio_printf(const char *format, ...)
{
        assert(_dbgio_state.dev_ops != NULL);

        va_list args;

        va_start(args, format);
        (void)vsprintf(_sprintf_buffer, format, args);
        va_end(args);

        if (*_sprintf_buffer == '\0') {
                return;
        }

        _dbgio_state.dev_ops->puts(_sprintf_buffer);
}

void
dbgio_flush(void)
{
        assert(_dbgio_state.dev_ops != NULL);

        _dbgio_state.dev_ops->flush();
}
