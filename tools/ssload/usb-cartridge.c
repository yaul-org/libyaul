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

#define RX_TIMEOUT      5000
#define TX_TIMEOUT      1000

#define I_VENDOR        0x0403
#define I_PRODUCT       0x6001
#define I_SERIAL        "AL00P4JX"

#define CMD_DOWNLOAD    1
#define CMD_UPLOAD      2
#define CMD_EXECUTE     3

typedef uint8_t crc_t;

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

static int device_read(uint8_t *, uint32_t);
static int device_write(uint8_t *, uint32_t);

/* Helpers */
static void convert_error(void);
static int send_command(uint32_t, uint32_t, size_t);
static int receive_checksum(const uint8_t *, size_t);
static int send_checksum(const uint8_t *, size_t);
static crc_t crc_calculate(const uint8_t *, size_t);

/*
 * USB Cartridge
 */
static int
init(void)
{
        DEBUG_PRINTF("Enter\n");

        ft_error = FT_OK;

        const char **devices_list;
        devices_list = enumerate_devices();

        if (devices_list == NULL) {
                goto error;
        }

        const char *device_serial;
        device_serial = NULL;

        int device_idx;
        for (device_idx = 0; device_idx < MAX_ENUMERATE_DEVICES; device_idx++) {
                if (devices_list[device_idx] == NULL) {
                        break;
                }

                if ((strncmp(devices_list[device_idx], I_SERIAL, sizeof(I_SERIAL))) == 0) {
                        device_serial = devices_list[device_idx];
                }

                DEBUG_PRINTF("Device #%02X SN: %s\n", device_idx,
                    devices_list[device_idx]);
        }

        if (device_serial == NULL) {
                ft_error = FT_DEVICE_NOT_FOUND;
                goto error;
        }

        DEBUG_PRINTF("Opening device with serial number \"%s\"\n",
            device_serial);

        if ((ft_error = FT_OpenEx((PVOID *)device_serial, FT_OPEN_BY_SERIAL_NUMBER,
                    &ft_handle)) != FT_OK) {
                DEBUG_PRINTF("Remove Linux kernel modules, ftdi_sio, and usbserial\n");
                goto error;
        }

        if ((ft_error = FT_Purge(ft_handle, FT_PURGE_RX | FT_PURGE_TX)) != FT_OK) {
                goto error;
        }

        if ((ft_error = FT_SetBitMode(ft_handle, 0x00, 0x00)) != FT_OK) {
                goto error;
        }

        if ((ft_error = FT_SetTimeouts(ft_handle, RX_TIMEOUT, TX_TIMEOUT)) != FT_OK) {
                goto error;
        }

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
        DEBUG_PRINTF("Enter\n");

        if (ft_handle == NULL) {
                return 0;
        }

        /* USB cartridge isn't happy when the buffers aren't purged */
        DEBUG_PRINTF("Purging (RX/TX) buffers\n");
        ft_error = FT_Purge(ft_handle, FT_PURGE_RX | FT_PURGE_TX);
        if (ft_error != FT_OK) {
                convert_error();
                DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);
                return -1;
        }

        DEBUG_PRINTF("Closing\n");
        if ((ft_error = FT_Close(ft_handle)) != FT_OK) {
                return -1;
        }

        return 0;
}

/*
 * USB Cartridge
 */
static int
device_read(uint8_t *read_buffer, uint32_t len)
{
#define MAX_TRIES (128 * 1024)

        DEBUG_PRINTF("Enter\n");

        ft_error = FT_OK;
        usb_cartridge_error = USB_CARTRIDGE_OK;

        uint32_t read;
        read = 0;

        DEBUG_PRINTF("Request read of %iB\n", len);

        do {
                DEBUG_PRINTF("Checking Queue status\n");

                DWORD queued_amount;
                queued_amount = 0;

                uint32_t timeout;
                timeout = 0;

                do {
                        ft_error = FT_GetQueueStatus(ft_handle, &queued_amount);
                        if (ft_error != FT_OK) {
                                convert_error();
                                DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);
                                return -1;
                        }

                        if (queued_amount == 0) {
                                timeout++;
                        }

                        if (timeout > MAX_TRIES) {
                                DEBUG_PRINTF("Exceeded number of tries (%i tries))\n",
                                    MAX_TRIES);
                                queued_amount = 1;
                                break;
                        }
                } while (queued_amount == 0);

                DEBUG_PRINTF("Queued amount: %iB\n", queued_amount);

                DWORD size;
                ft_error = FT_Read(ft_handle, read_buffer, queued_amount, &size);
                if (ft_error != FT_OK) {
                        convert_error();
                        DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);
                        return -1;
                }

                read += size;

                DEBUG_PRINTF("read: %i, size: %i\n", read, size);
        } while (read < len);

        if (read != len) {
                DEBUG_PRINTF("Amount bytes read exceeded request length\n");
                usb_cartridge_error = USB_CARTRIDGE_INSUFFICIENT_READ_DATA;
                return -1;
        }

        DEBUG_PRINTF("%iB read\n", read);

        return 0;
}

/*
 * USB Cartridge
 */
static int
device_write(uint8_t *write_buffer, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");
        DEBUG_PRINTF("Writing %iB\n", len);

        usb_cartridge_error = USB_CARTRIDGE_OK;

        ft_error = FT_Purge(ft_handle, FT_PURGE_RX | FT_PURGE_TX);
        if (ft_error != FT_OK) {
                convert_error();
                DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);
                return -1;
        }

        uint32_t written;
        written = 0;

        do {
                DWORD size;
                ft_error = FT_Write(ft_handle, write_buffer, len, &size);
                if (ft_error != FT_OK) {
                        convert_error();
                        DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);

                        return -1;
                }

                written += size;
        } while (written < len);

        DEBUG_PRINTF("%iB written\n", len);

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
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        ft_error = FT_OK;
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

        if ((send_command(CMD_DOWNLOAD, base_address, len)) < 0) {
                goto error;
        }

        if ((device_read(buffer, len)) < 0) {
                goto error;
        }

        if ((receive_checksum(buffer, len)) < 0) {
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

        ft_error = FT_OK;
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

        if ((send_command(CMD_UPLOAD, base_address, len)) < 0) {
                goto error;
        }

        if ((device_write(buffer, len)) < 0) {
                goto error;
        }

        if ((send_checksum(buffer, len)) < 0) {
                goto error;
        }

        if (execute) {
                if ((send_command(CMD_EXECUTE, base_address, len)) < 0) {
                        goto error;
                }
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

static int
send_command(uint32_t command, uint32_t address, size_t len)
{
        static char *command2str[] = {
                NULL,
                "DOWNLOAD",
                "UPLOAD",
                "EXECUTE"
        };

        usb_cartridge_error = USB_CARTRIDGE_OK;

        uint8_t buffer[9];

        DEBUG_PRINTF("Command: \"%s\" (0x%02X)\n", command2str[command],
            command);
        DEBUG_PRINTF("Address: 0x%08X\n", address);
        DEBUG_PRINTF("Size: %iB (0x%08X)\n", (uint32_t)len, (uint32_t)len);

        buffer[0] = command;
        buffer[1] = ADDRESS_MSB(address);
        buffer[2] = ADDRESS_02(address);
        buffer[3] = ADDRESS_01(address);
        buffer[4] = ADDRESS_LSB(address);
        buffer[5] = LEN_MSB(len);
        buffer[6] = LEN_02(len);
        buffer[7] = LEN_01(len);
        buffer[8] = LEN_LSB(len);

        return device_write(buffer, sizeof(buffer));
}

static int
receive_checksum(const uint8_t *buffer, size_t buffer_len)
{
        usb_cartridge_error = USB_CARTRIDGE_OK;

        crc_t checksum;
        checksum = crc_calculate(buffer, buffer_len);

        uint8_t read_buffer[1];
        read_buffer[0] = 0x00;

        int ret;
        if ((ret = device_read(read_buffer, sizeof(read_buffer))) < 0) {
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
send_checksum(const uint8_t *buffer, size_t buffer_len)
{
        usb_cartridge_error = USB_CARTRIDGE_OK;

        int ret;

        uint8_t write_buffer[1];
        write_buffer[0] = (uint8_t)crc_calculate(buffer, buffer_len);

        if ((ret = device_write(write_buffer, sizeof(write_buffer))) < 0) {
                return ret;
        }

        uint8_t read_buffer[1];
        read_buffer[0] = 0x00;

        if ((ret = device_read(read_buffer, sizeof(read_buffer))) < 0) {
                return ret;
        }

        if (read_buffer[0] != 0) {
                usb_cartridge_error = USB_CARTRIDGE_CORRUPTED_DATA;
                return -1;
        }

        return 0;
}

static crc_t
crc_calculate(const uint8_t *buffer, size_t buffer_len)
{
        /* Generated by pycrc v0.7.10. <http://www.tty1.net/pycrc> */

        static const crc_t crc_table[] = {
                0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
                0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
                0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
                0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
                0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
                0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
                0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
                0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
                0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
                0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
                0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
                0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
                0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
                0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
                0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
                0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
                0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
                0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
                0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
                0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
                0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
                0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
                0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
                0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
                0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
                0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
                0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
                0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
                0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
                0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
                0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
                0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
        };

        uint32_t tbl_idx;

        crc_t crc_value;
        crc_value = 0x00;

        DEBUG_PRINTF("Calculating checksum\n");

        while (buffer_len--) {
                tbl_idx = ((crc_value >> 0) ^ *buffer) & 0xFF;
                crc_value = (crc_table[tbl_idx] ^ (crc_value << 8)) & 0xFF;

                buffer++;
        }

        crc_t crc;
        crc = (crc_value & 0xFF) ^ 0x00;

        DEBUG_PRINTF("Calculated checksum: 0x%02X\n", (uint8_t)crc);

        return crc;
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
