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
#include "api.h"

void
console_init(void)
{
        /* Install necessary signal handlers */
}

void
console_exec(const struct device_driver *device, uint8_t command)
{
        if (command != API_CMD_LOG) {
                return;
        }

        int ret;

        uint32_t size;
        if ((ret = device->read(&size, sizeof(size))) < 0) {
                /* Error */
                goto error;
        }

        size = TO_LE(size);

        if (size == 0) {
                return;
        }

        char buffer[2];

        do {
                if ((ret = device->read(buffer, 1)) < 0) {
                        return;
                }
                buffer[1] = '\0';

                (void)printf("%s", buffer);
                (void)fflush(stdout);

                size--;
        } while (size > 0);

        return;

error:
        verbose_printf("Error reported by device: %s\n",
            device->error_stringify());
}

void
console_quit(const struct device_driver *device __unused)
{
}
