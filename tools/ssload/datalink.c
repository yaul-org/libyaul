/*
 * Copyright (c) 2012-2016 Israel Jacquez
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

#include "debug.h"
#include "driver.h"
#include "shared.h"

#define RX_TIMEOUT      5000
#define TX_TIMEOUT      1000

#define I_VENDOR        0x0403
#define I_PRODUCT       0x6001
#define I_SERIAL        "FTS12NIT"

/* Revision (green LED) */
#define REV_GREEN_BAUD_RATE             288000

#define PACKET_REV_GREEN_HEADER_SIZE    7
#define PACKET_REV_GREEN_DATA_SIZE      79
#define PACKET_REV_GREEN_TOTAL_SIZE     (PACKET_REV_GREEN_HEADER_SIZE +        \
    PACKET_REV_GREEN_DATA_SIZE)

/* Revision (red LED) */
#define REV_RED_BAUD_RATE               375000

#define PACKET_REV_RED_HEADER_SIZE      9
#define PACKET_REV_RED_DATA_SIZE        191
#define PACKET_REV_RED_TOTAL_SIZE       (PACKET_REV_RED_HEADER_SIZE +          \
    PACKET_REV_RED_DATA_SIZE)

#define PACKET_TYPE_RECEIVE_FIRST       0x01
#define PACKET_TYPE_RECEIVE_MIDDLE      0x11
#define PACKET_TYPE_RECEIVE_FINAL       0x21
#define PACKET_TYPE_SEND                0x09
#define PACKET_TYPE_SEND_EXECUTE        0x19

#define PACKET_RESPONSE_TYPE_RECEIVE    0x00
#define PACKET_RESPONSE_TYPE_SEND       0x01

enum {
        DATALINK_OK,
        DATALINK_DEVICE_ERROR,
        DATALINK_DEVICE_NOT_FOUND,
        DATALINK_DEVICE_NOT_OPENED,
        DATALINK_IO_ERROR,
        DATALINK_INSUFFICIENT_READ_DATA,
        DATALINK_INSUFFICIENT_WRITE_DATA,
        DATALINK_ERROR_PACKET,
        DATALINK_CORRUPTED_PACKET,
        DATALINK_FILE_NOT_FOUND,
        DATALINK_FILE_IO_ERROR,
        DATALINK_BAD_REQUEST,
        DATALINK_INSUFFICIENT_MEMORY
};

static uint32_t datalink_error = DATALINK_OK;

static const char *datalink_error_strings[] = {
        "DATALINK_OK",
        "DATALINK_DEVICE_ERROR",
        "DATALINK_DEVICE_NOT_FOUND",
        "DATALINK_DEVICE_NOT_OPENED",
        "DATALINK_IO_ERROR",
        "DATALINK_INSUFFICIENT_READ_DATA",
        "DATALINK_INSUFFICIENT_WRITE_DATA",
        "DATALINK_ERROR_PACKET",
        "DATALINK_CORRUPTED_PACKET",
        "DATALINK_FILE_NOT_FOUND",
        "DATALINK_FILE_IO_ERROR",
        "DATALINK_BAD_REQUEST",
        "DATALINK_INSUFFICIENT_MEMORY"
};

static struct {
        DWORD baud_rate;

        struct {
                uint32_t header_size;
                uint32_t data_size;
                uint32_t total_size;
        } packet;
} datalink_device = {
        .baud_rate = REV_RED_BAUD_RATE,
        .packet = {
                .header_size = PACKET_REV_RED_HEADER_SIZE,
                .data_size = PACKET_REV_RED_DATA_SIZE,
                .total_size = PACKET_REV_RED_TOTAL_SIZE
        }
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
static int device_packet_check(const uint8_t *, uint32_t);

/* Helpers */
static void convert_error(void);
static uint8_t packet_checksum(const uint8_t *, uint32_t);

/*
 *
 */
static int
init(void)
{
        DEBUG_PRINTF("Enter\n");

        /* XXX: To detect later */
        datalink_device.baud_rate = REV_RED_BAUD_RATE;
        datalink_device.packet.header_size = PACKET_REV_RED_HEADER_SIZE;
        datalink_device.packet.data_size = PACKET_REV_RED_DATA_SIZE;
        datalink_device.packet.total_size = PACKET_REV_RED_TOTAL_SIZE;

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
        if ((ft_error = FT_SetBaudRate(ft_handle,
                    datalink_device.baud_rate)) != FT_OK) {
                goto error;
        }
        if ((ft_error = FT_SetDataCharacteristics(ft_handle, FT_BITS_8,
                    FT_STOP_BITS_2, FT_PARITY_NONE)) != FT_OK) {
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
 *
 */
static int
shutdown(void)
{
        if (ft_handle != NULL) {
                FT_Close(ft_handle);
        }

        return 0;
}

const char *
error_stringify(void)
{
        static char buffer[1024];

        memset(buffer, '\0', sizeof(buffer));

        (void)sprintf(buffer, "%s (%s)",
            datalink_error_strings[datalink_error],
            ft_error_strings[ft_error]);

        return buffer;
}

/*
 *
 */
static int
device_read(uint8_t *read_buffer, uint32_t len)
{
#define MAX_TRIES (128 * 1024)

        DEBUG_PRINTF("Enter\n");

        ft_error = FT_OK;
        datalink_error = DATALINK_OK;

        if (len > datalink_device.packet.total_size) {
                datalink_error = DATALINK_IO_ERROR;
                return -1;
        }

        DWORD queued_amount;
        queued_amount = 0;

        uint32_t timeout;
        timeout = 0;

        DEBUG_PRINTF("Reading %iB\n", len);

        DEBUG_PRINTF("Checking Queue status\n");
        while ((queued_amount < len) && (ft_error == FT_OK)) {
                ft_error = FT_GetQueueStatus(ft_handle, &queued_amount);
                if ((queued_amount == 0) && (ft_error == FT_OK)) {
                        timeout++;
                }
                if (timeout > MAX_TRIES) {
                        DEBUG_PRINTF("Exceeded number of tries (%i tries))\n",
                            MAX_TRIES);
                        datalink_error = DATALINK_INSUFFICIENT_READ_DATA;
                        return -1;
                }
        }

        if (ft_error != FT_OK) {
                convert_error();
                DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);
                return -1;
        }

        if (queued_amount != len) {
                datalink_error = DATALINK_INSUFFICIENT_READ_DATA;
                DEBUG_PRINTF("Queued RX amount (%iB) doesn't match length request (%iB)\n",
                    queued_amount, len);
                return -1;
        }

        if (queued_amount > 0) {
                read_buffer[0] = 0x00;
                read_buffer[1] = 0x00;

                DWORD read;
                ft_error = FT_Read(ft_handle, read_buffer, queued_amount, &read);
                if (ft_error != FT_OK) {
                        convert_error();
                        DEBUG_PRINTF("FT API error: %s\n",
                            ft_error_strings[ft_error]);
                        return -1;
                } else if (read != len) {
                        datalink_error = DATALINK_INSUFFICIENT_READ_DATA;
                        DEBUG_PRINTF("Read (%iB) amount doesn't match length request (%iB)\n",
                            read, len);
                        return -1;
                }

                if (read_buffer[0] != 0xA5) {
                        datalink_error = DATALINK_CORRUPTED_PACKET;
                        return -1;
                }

#ifdef DEBUG
                uint32_t buffer_idx;
                for (buffer_idx = 0;
                     buffer_idx < datalink_device.packet.header_size;
                     buffer_idx++) {
                        DEBUG_PRINTF("B[%i]:0x%02X (%3u)\n",
                            buffer_idx,
                            read_buffer[buffer_idx],
                            read_buffer[buffer_idx]);
                }
#endif /* DEBUG */
        }

        datalink_error = DATALINK_OK;

        return 0;
}

/*
 *
 */
static int
device_write(uint8_t *write_buffer, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");
        DEBUG_PRINTF("Writing %iB\n", len);

        datalink_error = DATALINK_OK;

        if (len > datalink_device.packet.total_size) {
                datalink_error = DATALINK_IO_ERROR;
                return -1;
        }

#ifdef DEBUG
        uint32_t buffer_idx;
        for (buffer_idx = 0;
             buffer_idx < datalink_device.packet.header_size;
             buffer_idx++) {
                DEBUG_PRINTF("B[%i]:0x%02X (%3u)\n",
                    buffer_idx,
                    write_buffer[buffer_idx],
                    write_buffer[buffer_idx]);
        }
#endif /* DEBUG */

        ft_error = FT_Purge(ft_handle, FT_PURGE_RX | FT_PURGE_TX);
        if (ft_error != FT_OK) {
                convert_error();
                DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);
                return -1;
        }

        DWORD written;
        written = 0;

        ft_error = FT_Write(ft_handle, write_buffer, len, &written);
        if (ft_error != FT_OK) {
                convert_error();
                DEBUG_PRINTF("FT API error: %s\n", ft_error_strings[ft_error]);
                return -1;
        }
        if (written != len) {
                datalink_error = DATALINK_INSUFFICIENT_WRITE_DATA;
                return -1;
        }

        DEBUG_PRINTF("%iB written\n", written);

        return 0;
}

/*
 *
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
 *
 */
static int
download_file(const char *output_file, uint32_t base_address,
    uint32_t len)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        datalink_error = DATALINK_OK;

        if (output_file == NULL) {
                datalink_error = DATALINK_BAD_REQUEST;
                return -1;
        }
        if (*output_file == '\0') {
                datalink_error = DATALINK_BAD_REQUEST;
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
                datalink_error = DATALINK_INSUFFICIENT_MEMORY;
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
 *
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
 *
 */
static int
upload_execute_file(const char *input_file, uint32_t base_address, bool execute)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        datalink_error = DATALINK_OK;

        /* Sanity check */
        if (input_file == NULL) {
                datalink_error = DATALINK_BAD_REQUEST;
                return -1;
        }
        if (*input_file == '\0') {
                datalink_error = DATALINK_BAD_REQUEST;
                return -1;
        }

        FILE *iffp;
        iffp = NULL;

        uint8_t *buffer;
        buffer = NULL;

        if ((iffp = fopen(input_file, "rb+")) == NULL) {
                datalink_error = DATALINK_FILE_NOT_FOUND;
                goto error;
        }

        /* Determine the size of file */
        int32_t len;
        if ((fseek(iffp, 0, SEEK_END)) < 0) {
                datalink_error = DATALINK_FILE_IO_ERROR;
                goto error;
        }
        long tell;
        if ((tell = ftell(iffp)) < 0) {
                datalink_error = DATALINK_FILE_IO_ERROR;
                goto error;
        }
        rewind(iffp);

        if ((len = (int32_t)tell) < 2) {
                datalink_error = DATALINK_BAD_REQUEST;
                goto error;
        }

        if ((buffer = (uint8_t *)malloc(len)) == NULL) {
                datalink_error = DATALINK_INSUFFICIENT_MEMORY;
                goto error;
        }
        memset(buffer, 0x00, len);

        size_t read;
        if ((read = fread(buffer, 1, len, iffp)) != (size_t)len) {
                datalink_error = DATALINK_FILE_IO_ERROR;
                goto error;
        }

        int ret;
        if ((ret = upload_execute_buffer(buffer, base_address, len, execute)) < 0) {
                goto error;
        }

        goto exit;

error:
        exit_code = -1;

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
 *
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
 *
 */
static int
download_buffer(void *buffer, uint32_t base_address, uint32_t len)
{
#define STATE_RECEIVE_FIRST     0
#define STATE_RECEIVE_MIDDLE    1
#define STATE_RECEIVE_FINAL     2
#define STATE_FINISH            3

        DEBUG_PRINTF("Enter\n");

        /* Sanity check */
        if (buffer == NULL) {
                datalink_error = DATALINK_BAD_REQUEST;
                return -1;
        }

        if (base_address == 0x00000000) {
                datalink_error = DATALINK_BAD_REQUEST;
                return -1;
        }

        if (len <= 1) {
                datalink_error = DATALINK_BAD_REQUEST;
                return -1;
        }

        int exit_code;
        exit_code = 0;

        datalink_error = DATALINK_OK;

        uint8_t buffer_first[] = {
                0x5A,
                0x07,
                PACKET_TYPE_RECEIVE_FIRST,
                0, /* MSB address */
                0, /* 02 address */
                0, /* 01 address */
                0, /* LSB address */
                0, /* Datalink */
                0 /* Checksum */
        };

        uint8_t buffer_middle[] = {
                0x5A,
                0x07,
                PACKET_TYPE_RECEIVE_MIDDLE,
                0, /* MSB address */
                0, /* 02 address */
                0, /* 01 address */
                0, /* LSB address */
                0, /* Datalink */
                0 /* Checksum */
        };

        uint8_t buffer_final[] = {
                0x5A,
                0x07,
                PACKET_TYPE_RECEIVE_FINAL,
                0, /* MSB address */
                0, /* 02 address */
                0, /* 01 address */
                0, /* LSB address */
                0, /* Length */
                0 /* Checksum */
        };

        uint8_t *read_buffer;
        read_buffer = (uint8_t *)malloc(datalink_device.packet.total_size);
        assert(read_buffer != NULL);

        int32_t state;
        state = STATE_RECEIVE_FIRST;
        uint32_t count;
        count = 0;
        uint32_t address;
        address = base_address;

        uint8_t *write_buffer;
        write_buffer = NULL;

        bool tier_1;
        tier_1 = len <= datalink_device.packet.data_size;

        bool tier_2;
        tier_2 = (len > datalink_device.packet.data_size) &&
            (len <= (2 * datalink_device.packet.data_size));

        bool tier_3;
        tier_3 = len > (2 * datalink_device.packet.data_size);

        while (true) {
                switch (state) {
                case STATE_RECEIVE_FIRST:
                        DEBUG_PRINTF("%s", "State STATE_RECEIVE_FIRST\n");

                        write_buffer = &buffer_first[0];

                        if (tier_1) {
                                write_buffer[7] = len - 1;
                                state = STATE_RECEIVE_FINAL;
                        } else if (tier_2) {
                                write_buffer[7] = datalink_device.packet.data_size;
                                state = STATE_RECEIVE_FINAL;
                        } else if (tier_3) {
                                write_buffer[7] = datalink_device.packet.data_size;
                                /* The number of middle packets to send */
                                count = (len - datalink_device.packet.data_size - 1) /
                                    datalink_device.packet.data_size;

                                state = STATE_RECEIVE_MIDDLE;
                        }
                        break;
                case STATE_RECEIVE_MIDDLE:
                        write_buffer = &buffer_middle[0];

                        DEBUG_PRINTF("State STATE_RECEIVE_MIDDLE (%i)\n", count);

                        write_buffer[7] = datalink_device.packet.data_size;

                        count--;

                        if (count == 0) {
                                state = STATE_RECEIVE_FINAL;
                        }
                        break;
                case STATE_RECEIVE_FINAL:
                        DEBUG_PRINTF("State STATE_RECEIVE_FINAL\n");

                        write_buffer = &buffer_final[0];

                        if (tier_1) {
                                write_buffer[7] = 1;
                        } else if (tier_2) {
                                write_buffer[7] = len - datalink_device.packet.data_size;
                        } else if (tier_3) {
                                write_buffer[7] = (len - datalink_device.packet.data_size) %
                                    datalink_device.packet.data_size;
                                write_buffer[7] = (write_buffer[7] == 0)
                                    ? datalink_device.packet.data_size
                                    : write_buffer[7];
                        }

                        state = STATE_FINISH;
                        break;
                case STATE_FINISH:
                        goto exit;
                }

                write_buffer[3] = ADDRESS_MSB(address);
                write_buffer[4] = ADDRESS_02(address);
                write_buffer[5] = ADDRESS_01(address);
                write_buffer[6] = ADDRESS_LSB(address);

                write_buffer[8] = packet_checksum(write_buffer,
                    datalink_device.packet.header_size - 1);

                if ((device_write(write_buffer, datalink_device.packet.header_size)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                memset(read_buffer, 0x00, datalink_device.packet.total_size);
                if ((device_read(read_buffer, datalink_device.packet.header_size + write_buffer[7])) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                if ((device_packet_check(read_buffer, PACKET_RESPONSE_TYPE_RECEIVE)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                address += read_buffer[7] + 1;

                (void)memcpy(buffer, &read_buffer[datalink_device.packet.header_size - 1],
                    read_buffer[7]);
        }

        goto exit;

error:
        exit_code = -1;

exit:
        if (read_buffer != NULL) {
                free(read_buffer);
        }

        DEBUG_PRINTF("Exit\n");

        return exit_code;
}

/*
 *
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

static int
upload_execute_buffer(void *buffer, uint32_t base_address,
    uint32_t len, bool execute)
{
        DEBUG_PRINTF("Enter\n");

        int exit_code;
        exit_code = 0;

        datalink_error = DATALINK_OK;

        /* Sanity check */
        if (buffer == NULL) {
                datalink_error = DATALINK_BAD_REQUEST;
                return -1;
        }

        if (base_address == 0x00000000) {
                datalink_error = DATALINK_BAD_REQUEST;
                return -1;
        }

        void *base_buffer;
        base_buffer = buffer;
        uint32_t base_len;
        base_len = len;

        size_t read;

        uint32_t address;
        address = base_address;

        uint8_t execute_buffer[datalink_device.packet.total_size];
        memset(execute_buffer, 0x00, datalink_device.packet.total_size);
        execute_buffer[0] = 0x5A;
        execute_buffer[1] = 7 + 2;
        execute_buffer[2] = execute
            ? PACKET_TYPE_SEND_EXECUTE
            : PACKET_TYPE_SEND;
        execute_buffer[3] = ADDRESS_MSB(base_address);
        execute_buffer[4] = ADDRESS_02(base_address);
        execute_buffer[5] = ADDRESS_01(base_address);
        execute_buffer[6] = ADDRESS_LSB(base_address);
        execute_buffer[7] = 2;
        (void)memcpy(&execute_buffer[8], buffer, 2);
        buffer += 2;
        execute_buffer[datalink_device.packet.header_size + 2 - 1] =
            packet_checksum(execute_buffer, (datalink_device.packet.header_size + 2) - 1);

        len -= 2;
        address += 2;

        uint8_t read_buffer[datalink_device.packet.total_size];
        uint8_t tmp_buffer[datalink_device.packet.total_size];

        do {
                uint8_t transfer_len;
                transfer_len = (((int32_t)len - (int32_t)datalink_device.packet.data_size) < 0)
                    ? len % datalink_device.packet.data_size
                    : datalink_device.packet.data_size;

                uint8_t *write_buffer;

                DEBUG_PRINTF("Transferring %iB (%iB) to 0x%08X\n",
                    transfer_len, len, address);

                if (len == 0) {
                        write_buffer = execute_buffer;
                        transfer_len = 2;
                } else {
                        write_buffer = tmp_buffer;

                        memset(write_buffer, 0x00, datalink_device.packet.total_size);
                        write_buffer[0] = 0x5A;
                        write_buffer[1] = (7 + transfer_len) > ((int32_t)datalink_device.packet.total_size - 2)
                            ? (int32_t)datalink_device.packet.total_size - 2
                            : 7 + transfer_len;
                        write_buffer[2] = PACKET_TYPE_SEND;
                        write_buffer[3] = ADDRESS_MSB(address);
                        write_buffer[4] = ADDRESS_02(address);
                        write_buffer[5] = ADDRESS_01(address);
                        write_buffer[6] = ADDRESS_LSB(address);
                        write_buffer[7] = transfer_len;
                        (void)memcpy(&write_buffer[8], buffer, transfer_len);
                        buffer += transfer_len;
                        write_buffer[datalink_device.packet.header_size + transfer_len - 1] =
                            packet_checksum(write_buffer,
                                (datalink_device.packet.header_size + transfer_len) - 1);
                }

                if ((device_write(write_buffer, datalink_device.packet.header_size + transfer_len)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                memset(read_buffer, 0x00, datalink_device.packet.total_size);
                if ((device_read(read_buffer, datalink_device.packet.header_size)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                if ((device_packet_check(read_buffer, PACKET_RESPONSE_TYPE_SEND)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                len -= transfer_len;
                address += transfer_len;
        } while ((int32_t)len >= 0);

        if ((uintptr_t)base_buffer != (uintptr_t)(buffer - base_len)) {
                DEBUG_PRINTF("base_buffer=%p != buffer-len=%p\n",
                    (void *)base_buffer,
                    (void *)(buffer - base_len));

                datalink_error = DATALINK_INSUFFICIENT_READ_DATA;
                goto error;
        }

        goto exit;

error:
        exit_code = -1;

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
                datalink_error = DATALINK_OK;
                break;
        case FT_DEVICE_NOT_FOUND:
                datalink_error = DATALINK_DEVICE_NOT_FOUND;
                break;
        case FT_DEVICE_NOT_OPENED:
                datalink_error = DATALINK_DEVICE_NOT_OPENED;
                break;
        case FT_IO_ERROR:
                datalink_error = DATALINK_IO_ERROR;
                break;
        default:
                datalink_error = DATALINK_DEVICE_ERROR;
                break;
        }
}

/*
 *
 */
static uint8_t
packet_checksum(const uint8_t *buffer, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");
        DEBUG_PRINTF("Reading %iB from buffer\n", len);

        uint32_t checksum;
        checksum = 0;

        uint32_t buffer_idx;

#ifdef DEBUG
        static char output_buffer[2048];

        uint32_t output_idx;
        output_idx = 0;
        uint32_t fold_x;
        fold_x = 0;

        /* Skip 1st line */
        (void)sprintf(output_buffer, "\nHH HH HH HH HH HH HH HH\n");
        output_idx += strlen(output_buffer);

        for (buffer_idx = 0; buffer_idx < len; buffer_idx++) {
                (void)sprintf(&output_buffer[output_idx],
                    "%02X", buffer[buffer_idx]);
                output_idx += 2;
                if (((fold_x + 1) % 20) == 0) {
                        output_buffer[output_idx] = '\n';
                        output_idx++;
                } else if ((buffer_idx + 1) != len) {
                        output_buffer[output_idx] = ' ';
                        output_idx++;
                }

                fold_x++;
        }
        if (output_buffer[output_idx] != '\n') {
                output_buffer[output_idx] = '\n';
                output_idx++;
        }
        output_buffer[output_idx] = '\0';

        DEBUG_PRINTF("%s", output_buffer);
#endif /* DEBUG */

        for (buffer_idx = 1; buffer_idx < len; buffer_idx++) {
#ifdef DEBUG
                if (buffer_idx < datalink_device.packet.header_size) {
                        DEBUG_PRINTF("B[%02i]\n", buffer_idx);
                }
#endif /* DEBUG */

                checksum += (uint32_t)buffer[buffer_idx];
        }

        DEBUG_PRINTF("8-bit value: 0x%02X (%i)\n", (uint8_t)checksum,
            (uint8_t)checksum);
        DEBUG_PRINTF("32-bit value: 0x%02X (%i)\n", checksum, checksum);

        return (uint8_t)checksum;
}

/*
 *
 */
static int
device_packet_check(const uint8_t *buffer, uint32_t response_type)
{
        static const uint8_t packet_response_error[] = {
                0xA5,
                0x07,
                0x00,
                0x00,
                0x00,
                0x00,
                0x00,
                0x00,
                0x07
        };

        static const uint8_t packet_response_send[] = {
                0xA5,
                0x07,
                0xFF,
                0x00,
                0x00,
                0x00,
                0x00,
                0x00,
                0x06
        };

        datalink_error = DATALINK_OK;

        /* Check if the response packet is an error packet */
        uint32_t buffer_idx;
        bool bad_packet;

        for (buffer_idx = 0;
             ((buffer_idx < datalink_device.packet.header_size) &&
                 (buffer[buffer_idx] == packet_response_error[buffer_idx]));
             buffer_idx++);

        bad_packet = buffer_idx == datalink_device.packet.header_size;
        if (bad_packet) {
                datalink_error = DATALINK_ERROR_PACKET;
                return -1;
        }

        uint8_t calc_checksum;
        uint8_t checksum;

        switch (response_type) {
        case PACKET_RESPONSE_TYPE_RECEIVE:
                calc_checksum = packet_checksum(buffer,
                    (datalink_device.packet.header_size - 1) + buffer[7]);
                checksum = buffer[(datalink_device.packet.header_size) + buffer[7] - 1];

                if (checksum != calc_checksum) {
                        DEBUG_PRINTF("Checksum mismatch (0x%02X, 0x%02X)\n",
                            calc_checksum,
                            checksum);
                        datalink_error = DATALINK_CORRUPTED_PACKET;
                        return -1;
                }
                break;
        case PACKET_RESPONSE_TYPE_SEND:
                for (buffer_idx = 0;
                     ((buffer_idx < datalink_device.packet.header_size) &&
                         (buffer[buffer_idx] == packet_response_send[buffer_idx]));
                     buffer_idx++);
                bad_packet = buffer_idx != datalink_device.packet.header_size;
                if (bad_packet) {
                        datalink_error = DATALINK_ERROR_PACKET;
                        return -1;
                }
        }

        datalink_error = DATALINK_OK;

        return 0;
}

const struct device_driver device_datalink = {
        .name = "USB DataLink Red/Green LED",
        .init = init,
        .shutdown = shutdown,
        .error_stringify = error_stringify,
        .download_buffer = download_buffer,
        .download_file = download_file,
        .upload_buffer = upload_buffer,
        .upload_file = upload_file,
        .execute_buffer = execute_buffer,
        .execute_file = execute_file
};
