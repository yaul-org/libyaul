/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include "debug.h"
#include "shared.h"

FT_HANDLE ft_handle = NULL;
FT_STATUS ft_error = FT_OK;

const char *ft_error_strings[] = {
        "FT_OK",
        "FT_INVALID_HANDLE",
        "FT_DEVICE_NOT_FOUND",
        "FT_DEVICE_NOT_OPENED",
        "FT_IO_ERROR",
        "FT_INSUFFICIENT_RESOURCES",
        "FT_INVALID_PARAMETER",
        "FT_INVALID_BAUD_RATE",
        "FT_DEVICE_NOT_OPENED_FOR_ERASE",
        "FT_DEVICE_NOT_OPENED_FOR_WRITE",
        "FT_FAILED_TO_WRITE_DEVICE",
        "FT_EEPROM_READ_FAILED",
        "FT_EEPROM_WRITE_FAILED",
        "FT_EEPROM_ERASE_FAILED",
        "FT_EEPROM_NOT_PRESENT",
        "FT_EEPROM_NOT_PROGRAMMED",
        "FT_INVALID_ARGS",
        "FT_NOT_SUPPORTED",
        "FT_OTHER_ERROR",
        "FT_DEVICE_LIST_NOT_READY",
};

const char **
enumerate_devices(void)
{
        static char *devices_ptr_list[MAX_ENUMERATE_DEVICES + 1];
        static char devices_list[MAX_ENUMERATE_DEVICES][64];

        ft_error = FT_OK;

        int device_idx;

        for (device_idx = 0; device_idx < MAX_ENUMERATE_DEVICES; device_idx++) {
                devices_ptr_list[device_idx] = devices_list[device_idx];
        }
        devices_ptr_list[MAX_ENUMERATE_DEVICES] = NULL;

        int devices_cnt;

        if ((ft_error = FT_ListDevices(devices_ptr_list, &devices_cnt,
                    FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER)) != FT_OK) {
                return NULL;
        }

        if (devices_cnt == 0) {
                ft_error = FT_DEVICE_NOT_FOUND;
                return NULL;
        }

        /* Clear the rest */
        for (device_idx = devices_cnt; device_idx < MAX_ENUMERATE_DEVICES;
             device_idx++) {
                devices_ptr_list[device_idx] = NULL;
        }

        return (const char **)devices_ptr_list;
}
