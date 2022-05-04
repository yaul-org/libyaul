/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <internal.h>

#include "dbgio-internal.h"

/* This is enough for a 352x256 character resolution */
#define SPRINTF_BUFFER_SIZE (1408)

static struct {
        bool initialized;
        char *buffer;
        const dbgio_dev_ops_t *dev_ops;
} _dbgio_state;

static const dbgio_dev_ops_t *_dev_ops_table[] = {
        &__dev_ops_null,
        NULL,
        &__dev_ops_vdp2,
        &__dev_ops_vdp2_async,
        &__dev_ops_usb_cart
};

void
dbgio_init(void)
{
        if (_dbgio_state.initialized) {
                return;
        }

        _dbgio_state.initialized = true;

        dbgio_dev_default_init(DBGIO_DEV_NULL);

        if (_dbgio_state.buffer == NULL) {
                _dbgio_state.buffer = __malloc(SPRINTF_BUFFER_SIZE);
        }
}

void
dbgio_dev_init(dbgio_dev_t dev, const void *params)
{
        assert(_dbgio_state.initialized);

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
        assert(_dbgio_state.initialized);

        if (_dbgio_state.dev_ops == NULL) {
                return;
        }

        assert(_dbgio_state.dev_ops->deinit != NULL);

        _dbgio_state.dev_ops->deinit();

        _dbgio_state.dev_ops = NULL;
}

dbgio_dev_t
dbgio_dev_selected_get(void)
{
        assert(_dbgio_state.initialized);

        if (_dbgio_state.dev_ops == NULL) {
                return DBGIO_DEV_NULL;
        }

        return _dbgio_state.dev_ops->dev;
}

void
dbgio_dev_font_load(void)
{
        assert(_dbgio_state.initialized);

        assert(_dbgio_state.dev_ops != NULL);

        _dbgio_state.dev_ops->font_load();
}

void
dbgio_puts(const char *buffer)
{
        assert(_dbgio_state.initialized);

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
        assert(_dbgio_state.initialized);

        assert(_dbgio_state.dev_ops != NULL);

        va_list args;

        va_start(args, format);
        (void)vsprintf(_dbgio_state.buffer, format, args);
        va_end(args);

        if (*_dbgio_state.buffer == '\0') {
                return;
        }

        _dbgio_state.dev_ops->puts(_dbgio_state.buffer);
}

void
dbgio_flush(void)
{
        assert(_dbgio_state.initialized);

        assert(_dbgio_state.dev_ops != NULL);

        _dbgio_state.dev_ops->flush();
}
