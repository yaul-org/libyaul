/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

/*
 * Copyright (c) 2012, 2013, 2015, Anders Montonen
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "driver.h"
#include "shared.h"
#include "crc.h"

#define RX_TIMEOUT      5000
#define TX_TIMEOUT      1000

#define I_VENDOR        0x0403
#define I_PRODUCT       0x6001
#define I_SERIAL        "AL00P4JX"

#define CMD_DOWNLOAD            (1)
#define CMD_UPLOAD              (2)
#define CMD_EXECUTE             (3)
#define CMD_GET_BUFFER_ADDRESS  (4)
#define CMD_COPY_EXECUTE        (5)
#define CMD_EXECUTE_EXT         (6)

enum {
        USB_CARTRIDGE_OK,
        USB_CARTRIDGE_DEVICE_ERROR,
        USB_CARTRIDGE_DEVICE_NOT_FOUND,
        USB_CARTRIDGE_DEVICE_NOT_OPENED,
        USB_CARTRIDGE_IO_ERROR,
        USB_CARTRIDGE_INSUFFICIENT_READ_DATA,
        USB_CARTRIDGE_INSUFFICIENT_WRITE_DATA,
        USB_CARTRIDGE_CORRUPTED_DATA,
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
        "USB_CARTRIDGE_CORRUPTED_DATA",
        "USB_CARTRIDGE_FILE_NOT_FOUND",
        "USB_CARTRIDGE_FILE_IO_ERROR",
        "USB_CARTRIDGE_BAD_REQUEST",
        "USB_CARTRIDGE_INSUFFICIENT_MEMORY"
};

static int _dev_init(void);
static int _dev_shutdown(void);

static int _usb_cart_read(void *, uint32_t);
static int _usb_cart_send(void *, uint32_t);

static int _download_buffer(void *, uint32_t, uint32_t);
static int _upload_buffer(void *, uint32_t, uint32_t);
static int _execute_buffer(void *, uint32_t, uint32_t);

static int _download_file(const char *, uint32_t, uint32_t);
static int _upload_file(const char *, uint32_t);
static int _execute_file(const char *, uint32_t);

static int _upload_execute_buffer(void *, uint32_t, uint32_t, bool);
static int _upload_execute_file(const char *, uint32_t, bool);

static int _device_read(uint8_t *, uint32_t);
static int _device_write(uint8_t *, uint32_t);

/* Helpers */
static int _send_command(uint32_t, uint32_t, size_t);
static int _receive_checksum(const uint8_t *, size_t);
static int _send_checksum(const uint8_t *, size_t);

static int
_dev_init(void)
{
        DEBUG_PRINTF("Enter\n");

#define USB_READ_PACKET_SIZE    (64 * 1024)
#define USB_WRITE_PACKET_SIZE   (4 * 1024)
#define USB_PAYLOAD(x)          ((x) - (((x) / 64) * 2))
#define READ_PAYLOAD_SIZE       (USB_PAYLOAD(USB_READ_PACKET_SIZE))
#define WRITE_PAYLOAD_SIZE      (USB_PAYLOAD(USB_WRITE_PACKET_SIZE))

        if ((ftdi_error = ftdi_init(&ftdi_ctx)) < 0) {
                DEBUG_PRINTF("ftdi_init()\n");
                goto error;
        }
        ftdi_error = ftdi_usb_open(&ftdi_ctx, I_VENDOR, I_PRODUCT);
        if ((ftdi_error < 0) && (ftdi_error != -5)) {
                DEBUG_PRINTF("ftdi_usb_open()\n");
                goto error;
        }
        if ((ftdi_error = ftdi_tcioflush(&ftdi_ctx)) < 0) {
                DEBUG_PRINTF("ftdi_tcioflush()\n");
                goto error;
        }
        if ((ftdi_error = ftdi_read_data_set_chunksize(&ftdi_ctx,
                    USB_READ_PACKET_SIZE)) < 0) {
                DEBUG_PRINTF("ftdi_read_data_set_chunksize()\n");
                goto error;
        }
        if ((ftdi_error = ftdi_write_data_set_chunksize(&ftdi_ctx,
                    USB_WRITE_PACKET_SIZE)) < 0) {
                DEBUG_PRINTF("ftdi_write_data_set_chunksize()\n");
                goto error;
        }
        if ((ftdi_error = ftdi_set_bitmode(&ftdi_ctx, 0x00, BITMODE_RESET)) < 0) {
                DEBUG_PRINTF("ftdi_set_bitmode()\n");
                goto error;
        }

exit:
        return 0;

error:
        DEBUG_PRINTF("ftdi_error: %i\n", ftdi_error);

        ftdi_usb_close(&ftdi_ctx);

        return -1;
}

static int
_dev_shutdown(void)
{
        DEBUG_PRINTF("Enter\n");

        if ((ftdi_error = ftdi_tcioflush(&ftdi_ctx)) < 0) {
                return -1;
        }

        ftdi_usb_close(&ftdi_ctx);

        return 0;
}

static int
_device_read(uint8_t *read_buffer, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");
        DEBUG_PRINTF("Request read of %iB\n", len);

        usb_cartridge_error = USB_CARTRIDGE_OK;

        uint32_t read;
        read = 0;

        while ((len - read) > 0) {
                DEBUG_PRINTF("Call to ftdi_read_data(%i)\n", len);
                DEBUG_PRINTF("Read %iB\n", read);

                DEBUG_HEXDUMP(read_buffer, read);

                int amount;
                if ((amount = ftdi_read_data(&ftdi_ctx, read_buffer, len)) < 0) {
                        ftdi_error = amount;
                        return -1;
                }
                read += amount;
        }

        DEBUG_PRINTF("%iB read\n", read);

        return 0;
}

static int
_device_write(uint8_t *write_buffer, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");
        DEBUG_PRINTF("Writing %iB\n", len);

        usb_cartridge_error = USB_CARTRIDGE_OK;

        uint32_t written;
        written = 0;

        while ((len - written) > 0) {
                int amount;
                if ((amount = ftdi_write_data(&ftdi_ctx, write_buffer, len)) < 0) {
                        ftdi_error = amount;
                        return -1;
                }
                written += amount;
        }

        DEBUG_PRINTF("%iB written\n", len);

        return 0;
}

static int
_upload_file(const char *input_file, uint32_t base_address)
{
        DEBUG_PRINTF("Enter\n");

        int ret;
        ret = _upload_execute_file(input_file, base_address,
            /* execute = */ false);

        DEBUG_PRINTF("Exit\n");

        return ret;
}

static int
_download_file(const char *output_file, uint32_t base_address,
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
                goto error;
        }
        memset(buffer, 0x00, len);

        int ret;
        if ((ret = _download_buffer(buffer, base_address, len)) < 0) {
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

static int
_execute_file(const char *input_file, uint32_t base_address)
{
        DEBUG_PRINTF("Enter\n");

        int ret;
        ret = _upload_execute_file(input_file, base_address,
            /* execute = */ true);

        DEBUG_PRINTF("Exit\n");

        return ret;
}

static int
_upload_execute_file(const char *input_file, uint32_t base_address, bool execute)
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
        if ((ret = _upload_execute_buffer(buffer, base_address, len, execute)) < 0) {
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

static int
_upload_buffer(void *buffer, uint32_t base_address, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int ret;
        ret = _upload_execute_buffer(buffer, base_address, len,
            /* execute = */ false);

        DEBUG_PRINTF("Exit\n");

        return ret;
}

static const char *
error_stringify(void)
{
        return ftdi_get_error_string(&ftdi_ctx);
}

static int
_usb_cart_read(void *buffer, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        /* ft_error = FT_OK; */
        usb_cartridge_error = USB_CARTRIDGE_OK;

        /* Sanity check */
        if (buffer == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                goto error;
        }

        if ((_device_read(buffer, len)) < 0) {
                goto error;
        }

        goto exit;

error:
        exit_code = -1;

        (void)printf("ERROR: %s\n", usb_cartridge_error_strings[usb_cartridge_error]);

exit:
        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

static int
_usb_cart_send(void *buffer, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        /* ft_error = FT_OK; */
        usb_cartridge_error = USB_CARTRIDGE_OK;

        /* Sanity check */
        if (buffer == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                goto error;
        }

        if ((_device_write(buffer, len)) < 0) {
                goto error;
        }

        goto exit;

error:
        exit_code = -1;

        (void)printf("ERROR: %s\n", usb_cartridge_error_strings[usb_cartridge_error]);

exit:
        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

static int
_download_buffer(void *buffer, uint32_t base_address, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        /* ft_error = FT_OK; */
        usb_cartridge_error = USB_CARTRIDGE_OK;

        /* Sanity check */
        if (buffer == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                goto error;
        }

        if (base_address == 0x00000000) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                goto error;
        }

        if ((_send_command(CMD_DOWNLOAD, base_address, len)) < 0) {
                goto error;
        }

        if ((_device_read(buffer, len)) < 0) {
                goto error;
        }

        if ((_receive_checksum(buffer, len)) < 0) {
                goto error;
        }

        goto exit;

error:
        exit_code = -1;

        (void)printf("ERROR: %s\n", usb_cartridge_error_strings[usb_cartridge_error]);

exit:
        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

static int
_execute_buffer(void *buffer, uint32_t base_address, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int ret = _upload_execute_buffer(buffer, base_address, len,
            /* execute = */ true);

        DEBUG_PRINTF("Exit\n");

        return ret;
}

static int
_upload_execute_buffer(void *buffer, uint32_t base_address,
    uint32_t len, bool execute)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        /* ft_error = FT_OK; */
        usb_cartridge_error = USB_CARTRIDGE_OK;

        /* Sanity check */
        if (buffer == NULL) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                goto error;
        }

        if (base_address == 0x00000000) {
                usb_cartridge_error = USB_CARTRIDGE_BAD_REQUEST;
                goto error;
        }

        uint8_t command;
        command = (execute) ? CMD_EXECUTE_EXT : CMD_UPLOAD;

        if ((_send_command(command, base_address, len)) < 0) {
                goto error;
        } 

        if ((_device_write(buffer, len)) < 0) {
                goto error;
        }

        if ((_send_checksum(buffer, len)) < 0) {
                goto error;
        }

        goto exit;

error:
        exit_code = -1;

        (void)printf("ERROR: %s\n", usb_cartridge_error_strings[usb_cartridge_error]);

exit:
        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

static int
_send_command(uint32_t command, uint32_t address, size_t len)
{
        static char *command2str[] = {
                NULL,
                "CMD_DOWNLOAD",     
                "CMD_UPLOAD",
                "CMD_EXECUTE",
                "CMD_GET_BUFF_ADDR",
                "CMD_COPY_EXECUTE",
                "CMD_EXECUTE_EXT"
        };

        usb_cartridge_error = USB_CARTRIDGE_OK;

        uint8_t buffer[13];
        uint8_t buffer_len;

        DEBUG_PRINTF("Command: \"%s\" (0x%02X)\n", command2str[command],
            command);
        DEBUG_PRINTF("Address: 0x%08X\n", address);
        DEBUG_PRINTF("Size: %iB (0x%08X)\n", (uint32_t)len, (uint32_t)len);

        buffer[ 0] = command;

        buffer[ 1] = ADDRESS_MSB(address);
        buffer[ 2] = ADDRESS_02(address);
        buffer[ 3] = ADDRESS_01(address);
        buffer[ 4] = ADDRESS_LSB(address);

        buffer[ 5] = LEN_MSB(len);
        buffer[ 6] = LEN_02(len);
        buffer[ 7] = LEN_01(len);
        buffer[ 8] = LEN_LSB(len);

        buffer_len = 9;

        if (command == CMD_EXECUTE_EXT) {
                buffer[ 9] = 0;
                buffer[10] = 0;
                buffer[11] = 0;
                buffer[12] = 0;

                buffer_len = 13;
        }

        return _device_write(buffer, buffer_len);
}

static int
_receive_checksum(const uint8_t *buffer, size_t buffer_len)
{
        usb_cartridge_error = USB_CARTRIDGE_OK;

        crc_t checksum;
        checksum = crc_calculate(buffer, buffer_len);

        uint8_t read_buffer[1];
        read_buffer[0] = 0x00;

        int ret;
        if ((ret = _device_read(read_buffer, sizeof(read_buffer))) < 0) {
                return ret;
        }

        if ((crc_t)read_buffer[0] != checksum) {
                DEBUG_PRINTF("Checksum received (0x%02X) does not match calculated (0x%02X)\n",
                    (uint8_t)read_buffer[0],
                    checksum);

                usb_cartridge_error = USB_CARTRIDGE_CORRUPTED_DATA;
                return -1;
        }

        return 0;
}

static int
_send_checksum(const uint8_t *buffer, size_t buffer_len)
{
        usb_cartridge_error = USB_CARTRIDGE_OK;

        int ret;

        uint8_t write_buffer[1];
        write_buffer[0] = (uint8_t)crc_calculate(buffer, buffer_len);

        if ((ret = _device_write(write_buffer, sizeof(write_buffer))) < 0) {
                return ret;
        }

        uint8_t read_buffer[1];
        read_buffer[0] = 0x00;

        if ((ret = _device_read(read_buffer, sizeof(read_buffer))) < 0) {
                return ret;
        }

        if (read_buffer[0] != 0) {
                usb_cartridge_error = USB_CARTRIDGE_CORRUPTED_DATA;
                return -1;
        }

        return 0;
}

const struct device_driver device_usb_cartridge = {
        .name = "USB Flash Cartridge",
        .init = _dev_init,
        .shutdown = _dev_shutdown,
        .error_stringify = error_stringify,
        .read = _usb_cart_read,
        .send = _usb_cart_send,
        .download_buffer = _download_buffer,
        .download_file = _download_file,
        .upload_buffer = _upload_buffer,
        .upload_file = _upload_file,
        .execute_buffer = _execute_buffer,
        .execute_file = _execute_file
};
