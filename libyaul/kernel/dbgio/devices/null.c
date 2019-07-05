/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdio.h>

#include <dbgio.h>

#include "../dbgio-internal.h"

static void _init(const void *);
static void _deinit(void);
static void _buffer(const char *);
static void _flush(void);

const uint32_t _default_params;

const dbgio_dev_ops_t _internal_dev_ops_null = {
        .dev = DBGIO_DEV_NULL,
        .default_params = &_default_params,
        .init = _init,
        .deinit = _deinit,
        .buffer = _buffer,
        .flush = _flush
};

static void
_init(const void *params __unused)
{
}

static void
_deinit(void)
{
}

static void
_buffer(const char *buffer __unused)
{
}

static void
_flush(void)
{
}
