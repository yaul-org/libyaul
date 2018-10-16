/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <ctype.h>
#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "shared.h"
#include "drivers.h"

void
console(const struct device_driver *device)
{
        int ret;

        uint32_t size;
        size = 0;

        while (true) {
                uint8_t buffer[62];

                memset(buffer, '\0', sizeof(buffer));

                if ((ret = device->read(buffer, sizeof(buffer))) < 0) {
                        goto error;
                }

                size += strlen((const char *)buffer);

                DEBUG_PRINTF("\"%.*s\" (size: %u bytes)\n", size, buffer, size);

                (void)printf("%.*s", size, buffer);
                (void)fflush(stdout);
        }

error:
        verbose_printf("Error reported by device: %s\n",
            device->error_stringify());

        return;
}
