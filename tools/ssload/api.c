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

void fileserver_exec(const struct device_driver *, uint8_t);
void fileserver_quit(const struct device_driver *);

void console_exec(const struct device_driver *, uint8_t);
void console_quit(const struct device_driver *);

void
api_handle(const struct device_driver *device)
{
        while (true) {
                uint8_t command;

                int ret;
                if ((ret = device->read(&command, 1)) >= 0) {
                        DEBUG_PRINTF("Command: 0x%02X\n", command);

                        switch (command) {
                        case API_CMD_FILE:
                        case API_CMD_SIZE:
                                fileserver_exec(device, command);
                                break;
                        case API_CMD_LOG:
                                console_exec(device, command);
                                break;
                        case API_CMD_QUIT:
                                fileserver_quit(device);
                                console_quit(device);
                                break;
                        }
                }

                usleep(API_USLEEP);
        }
}
