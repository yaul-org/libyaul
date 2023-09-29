/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <stdio.h>

#include <dbgio/dbgio-internal.h>

const dbgio_dev_ops_t __dbgio_dev_ops_null = {
    .dev            = DBGIO_DEV_NULL,
    .default_params = NULL,
    .init           = NULL,
    .deinit         = NULL,
    .display_set    = NULL,
    .puts           = NULL,
    .printf         = NULL,
    .flush          = NULL
};
