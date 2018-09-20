/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <ctype.h>
#include <libgen.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "drivers.h"

void
console(const struct device_driver *device)
{
        uint8_t term_byte;
        term_byte = 0xFF;

        while (true) {
                char byte;
                byte = '\0';

                int ret;

                if ((ret = device->read_byte(&byte)) != 0) {
                        break;
                }

                switch (byte) {
                case '\0':
                        /* Send termination byte */
                        device->send_byte(&term_byte);
                        break;
                default:
                        (void)putchar(byte);
                        break;
                }

                fflush(stdout);
        }

        (void)putchar('\n');
}
