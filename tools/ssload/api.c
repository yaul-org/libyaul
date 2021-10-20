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
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "debug.h"
#include "shared.h"
#include "drivers.h"
#include "api.h"

static volatile struct {
        bool running;
} _state;

void fileserver_exec(const struct device_driver *, uint8_t);
void fileserver_quit(const struct device_driver *);

void console_exec(const struct device_driver *, uint8_t);
void console_quit(const struct device_driver *);

static void _catch_signals(void);

static void _signal_handler(int signum, siginfo_t *info, void *ptr);

void
api_handle(const struct device_driver *device)
{
        _catch_signals();

        _state.running = true;

        while (_state.running) {
                uint8_t command;

                int ret;
                if ((ret = device->read(&command, 1)) >= 0) {
                        DEBUG_PRINTF("Command: 0x%02X\n", command);

                        switch (command) {
                        case API_CMD_FILE:
                        case API_CMD_BYTE_SIZE:
                        case API_CMD_SECTOR_COUNT:
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

bool
api_byte_read(const struct device_driver *device, uint8_t *value)
{
        *value = 0;

        return api_variable_read(device, value, sizeof(uint8_t));
}

bool
api_long_read(const struct device_driver *device, uint32_t *value)
{
        *value = 0;

        bool read;
        read = api_variable_read(device, value, sizeof(uint32_t));

        *value = TO_LE(*value);

        return read;
}

bool
api_variable_read(const struct device_driver *device, void *buffer, uint32_t buffer_len)
{
        (void)memset(buffer, 0x00, buffer_len);

        for (uint32_t i = 0; i < API_RETRIES_COUNT; i++) {
                int ret;
                if ((ret = device->read(buffer, buffer_len)) >= 0) {
                        return true;
                }

                usleep(API_USLEEP);
        }

        return false;
}

static void
_catch_signals(void)
{
        struct sigaction act;

        act.sa_flags = 0;

        sigemptyset(&act.sa_mask);

        act.sa_sigaction = _signal_handler;

        if ((sigaction(SIGINT, &act, NULL)) < 0) {
        }
}

static void
_signal_handler(int signum, siginfo_t *info, void *ptr)
{
        _state.running = false;
}
