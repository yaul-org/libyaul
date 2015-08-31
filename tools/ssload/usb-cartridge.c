/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ftd2xx.h>

#include "debug.h"
#include "driver.h"
#include "shared.h"

enum {
        USB_CARTRIDGE_OK,
        USB_CARTRIDGE_DEVICE_ERROR,
        USB_CARTRIDGE_DEVICE_NOT_FOUND,
        USB_CARTRIDGE_DEVICE_NOT_OPENED,
        USB_CARTRIDGE_IO_ERROR,
        USB_CARTRIDGE_INSUFFICIENT_READ_DATA,
        USB_CARTRIDGE_INSUFFICIENT_WRITE_DATA,
        USB_CARTRIDGE_ERROR_PACKET,
        USB_CARTRIDGE_CORRUPTED_PACKET,
        USB_CARTRIDGE_FILE_NOT_FOUND,
        USB_CARTRIDGE_FILE_IO_ERROR,
        USB_CARTRIDGE_BAD_REQUEST,
        USB_CARTRIDGE_INSUFFICIENT_MEMORY
};

static uint32_t usb_cartridge_error = USB_CARTRIDGE_OK;

static const char *usb_cartridge_error_strings[] = {
        "USB_CARTRIDGE_OK",
        "USB_CARTRIDGE_DEVICE_ERROR",
        "USB_CARTRIDGE_DEVICE_NOT_FOUND",
        "USB_CARTRIDGE_DEVICE_NOT_OPENED",
        "USB_CARTRIDGE_IO_ERROR",
        "USB_CARTRIDGE_INSUFFICIENT_READ_DATA",
        "USB_CARTRIDGE_INSUFFICIENT_WRITE_DATA",
        "USB_CARTRIDGE_ERROR_PACKET",
        "USB_CARTRIDGE_CORRUPTED_PACKET",
        "USB_CARTRIDGE_FILE_NOT_FOUND",
        "USB_CARTRIDGE_FILE_IO_ERROR",
        "USB_CARTRIDGE_BAD_REQUEST",
        "USB_CARTRIDGE_INSUFFICIENT_MEMORY"
};

static FT_HANDLE ft_handle = NULL;
static FT_STATUS ft_error = FT_OK;

static const char *ft_error_strings[] = {
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

static int init(void);
static int shutdown(void);

static int download_buffer(void *, uint32_t, uint32_t);
static int upload_buffer(void *, uint32_t, uint32_t);
static int execute_buffer(void *, uint32_t, uint32_t);

static int download_file(const char *, uint32_t, uint32_t);
static int upload_file(const char *, uint32_t);
static int execute_file(const char *, uint32_t);

static int upload_execute_buffer(void *, uint32_t, uint32_t, bool);
static int upload_execute_file(const char *, uint32_t, bool);

static int usb_cartridge_read(uint8_t *, uint32_t);
static int usb_cartridge_write(uint8_t *, uint32_t);
static int usb_cartridge_packet_check(const uint8_t *, uint32_t);

/*
 * USB Cartridge
 */
static int
init(void)
{
#define MAX_DEVICES 10

        DEBUG_PRINTF("Enter\n");

exit:
        return 0;

error:
        DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);

        return -1;
}

/*
 * USB Cartridge
 */
static int
shutdown(void)
{
        if (ft_handle != NULL) {
                FT_Close(ft_handle);
        }

        return 0;
}

/*
 * USB Cartridge
 */
static int
usb_cartridge_read(uint8_t *read_buffer, uint32_t len)
{
        return 0;
}

/*
 * USB Cartridge
 */
static int
usb_cartridge_write(uint8_t *write_buffer, uint32_t len)
{
        return 0;
}

/*
 * USB Cartridge
 */
static int
upload_file(const char *input_file, uint32_t base_address)
{
        DEBUG_PRINTF("Enter\n");

        int ret;
        ret = upload_execute_file(input_file, base_address,
            /* execute = */ false);

        DEBUG_PRINTF("Exit\n");

        return ret;
}

/*
 * USB Cartridge
 */
static int
download_file(const char *output_file, uint32_t base_address,
    uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        usb_cartridge_error = USB_CARTRIDGE_OK;

        if (output_file == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                return -1;
        }
        if (*output_file == '\0') {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                return -1;
        }

        FILE *offp;
        offp = NULL;
        if ((offp = fopen(output_file, "wb+")) == NULL) {
                return -1;
        }

        uint8_t *buffer;
        buffer = NULL;
        if ((buffer = (uint8_t *)malloc(len)) == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_INSUFFICIENT_MEMORY;
                return -1;
        }
        memset(buffer, 0x00, len);

        int ret;
        if ((ret = download_buffer(buffer, base_address, len)) < 0) {
                goto error;
        }

        (void)fwrite(buffer, 1, len, offp);

        goto exit;

error:
        exit_code = -1;

        (void)printf("ERROR: %s\n", usb_cartridge_error_strings[usb_cartridge_error]);

exit:
        if (buffer != NULL) {
                free(buffer);
        }

        if (offp != NULL) {
                fclose(offp);
        }

        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

/*
 * USB Cartridge
 */
static int
execute_file(const char *input_file, uint32_t base_address)
{
        DEBUG_PRINTF("Enter\n");

        int ret;
        ret = upload_execute_file(input_file, base_address,
            /* execute = */ true);

        DEBUG_PRINTF("Exit\n");

        return ret;
}

/*
 * USB Cartridge
 */
static int
upload_execute_file(const char *input_file, uint32_t base_address, bool execute)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        usb_cartridge_error = USB_CARTRIDGE_OK;

        /* Sanity check */
        if (input_file == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                return -1;
        }
        if (*input_file == '\0') {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                return -1;
        }

        FILE *iffp;
        iffp = NULL;

        uint8_t *buffer;
        buffer = NULL;

        if ((iffp = fopen(input_file, "rb+")) == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_FILE_NOT_FOUND;
                goto error;
        }

        /* Determine the size of file */
        int32_t len;
        if ((fseek(iffp, 0, SEEK_END)) < 0) {
                usb_cartridge_error = USB_CARTRIDGE_FILE_IO_ERROR;
                goto error;
        }
        long tell;
        if ((tell = ftell(iffp)) < 0) {
                usb_cartridge_error = USB_CARTRIDGE_FILE_IO_ERROR;
                goto error;
        }
        rewind(iffp);

        if ((len = (int32_t)tell) < 2) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                goto error;
        }

        if ((buffer = (uint8_t *)malloc(len)) == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_INSUFFICIENT_MEMORY;
                goto error;
        }
        memset(buffer, 0x00, len);

        size_t read;
        if ((read = fread(buffer, 1, len, iffp)) != (size_t)len) {
                usb_cartridge_error = USB_CARTRIDGE_FILE_IO_ERROR;
                goto error;
        }

        int ret;
        if ((ret = upload_execute_buffer(buffer, base_address, len, execute)) < 0) {
                goto error;
        }

        goto exit;

error:
        exit_code = -1;

        (void)printf("ERROR: %s\n", usb_cartridge_error_strings[usb_cartridge_error]);

exit:
        if (buffer != NULL) {
                free(buffer);
        }

        if (iffp != NULL) {
                fclose(iffp);
        }

        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

/*
 * USB Cartridge
 */
static int
upload_buffer(void *buffer, uint32_t base_address, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int ret;
        ret = upload_execute_buffer(buffer, base_address, len,
            /* execute = */ false);

        DEBUG_PRINTF("Exit\n");

        return ret;
}

/*
 * USB Cartridge
 */
static int
download_buffer(void *buffer, uint32_t base_address, uint32_t len)
{
        int exit_code;
        exit_code = 0;

        goto exit;

error:
        exit_code = -1;

        (void)printf("ERROR: %s\n", usb_cartridge_error_strings[usb_cartridge_error]);

exit:
        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

/*
 * USB Cartridge
 */
static int
execute_buffer(void *buffer, uint32_t base_address, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int ret = upload_execute_buffer(buffer, base_address, len,
            /* execute = */ true);

        DEBUG_PRINTF("Exit\n");

        return ret;
}

/*
 * USB Cartridge
 */
static int
upload_execute_buffer(void *buffer, uint32_t base_address,
    uint32_t len, bool execute)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        usb_cartridge_error = USB_CARTRIDGE_OK;

        /* Sanity check */
        if (buffer == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                return -1;
        }

        if (base_address == 0x00000000) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                return -1;
        }

        goto exit;

error:
        exit_code = -1;

        (void)printf("ERROR: %s\n", usb_cartridge_error_strings[usb_cartridge_error]);

exit:
        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

/*
 *
 */
static void
convert_error(void)
{
        switch (ft_error) {
        case FT_OK:
                usb_cartridge_error = USB_CARTRIDGE_OK;
                break;
        case FT_DEVICE_NOT_FOUND:
                usb_cartridge_error = USB_CARTRIDGE_DEVICE_NOT_FOUND;
                break;
        case FT_DEVICE_NOT_OPENED:
                usb_cartridge_error = USB_CARTRIDGE_DEVICE_NOT_OPENED;
                break;
        case FT_IO_ERROR:
                usb_cartridge_error = USB_CARTRIDGE_IO_ERROR;
                break;
        default:
                usb_cartridge_error = USB_CARTRIDGE_DEVICE_ERROR;
                break;
        }
}

const struct device_driver device_usb_cartridge = {
        .name = "USB Flash Cartridge",
        .init = init,
        .shutdown = shutdown,
        .error_stringify = NULL,
        .download_buffer = download_buffer,
        .download_file = download_file,
        .upload_buffer = upload_buffer,
        .upload_file = upload_file,
        .execute_buffer = execute_buffer,
        .execute_file = execute_file
};
