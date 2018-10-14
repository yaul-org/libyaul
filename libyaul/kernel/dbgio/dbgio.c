/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdio.h>

#include <dbgio.h>

#include "dbgio-internal.h"

static const dbgio_dev_ops_t *_dev_ops;

static const dbgio_dev_ops_t *_dev_ops_table[] = {
        &_internal_dev_ops_null,
        NULL,
        &_internal_dev_ops_vdp2,
        &_internal_dev_ops_usb_cart,
};

void
dbgio_init(void)
{
        _dev_ops = NULL;
}

void
dbgio_dev_init(uint8_t dev, const void *params)
{
        assert((dev == DBGIO_DEV_NULL) ||
               (dev == DBGIO_DEV_VDP2));

        assert(params != NULL);

        _dev_ops_table[dev]->init(params);
}

void
dbgio_dev_default_init(uint8_t dev)
{
        assert((dev == DBGIO_DEV_NULL) ||
               (dev == DBGIO_DEV_VDP2));

        _dev_ops_table[dev]->init(_dev_ops_table[dev]->default_params);
}

void
dbgio_dev_set(uint8_t dev)
{
        assert((dev == DBGIO_DEV_NULL) ||
               (dev == DBGIO_DEV_VDP2));

        _dev_ops = _dev_ops_table[dev];
}

void
dbgio_buffer(const char *buffer)
{
        assert(_dev_ops != NULL);

        assert(buffer != NULL);

        _dev_ops->buffer(buffer);
}

void
dbgio_flush(void)
{
        assert(_dev_ops != NULL);

        _dev_ops->flush();
}
