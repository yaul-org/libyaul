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

#define DEBUG

#define BAUD            375000
#define PARITY          FT_PARITY_NONE
#define DATA_BITS       FT_BITS_8
#define STOP_BITS       FT_STOP_BITS_2
#define RX_TIMEOUT      5000
#define TX_TIMEOUT      1000

#define PACKET_HEADER_SIZE      9
#define PACKET_DATA_SIZE        191
#define PACKET_TOTAL_SIZE       (PACKET_HEADER_SIZE + PACKET_DATA_SIZE)

#define PACKET_TYPE_RECEIVE_FIRST       0x01
#define PACKET_TYPE_RECEIVE_MIDDLE      0x11
#define PACKET_TYPE_RECEIVE_FINAL       0x21
#define PACKET_TYPE_SEND                0x09
#define PACKET_TYPE_SEND_EXECUTE        0x19

#define PACKET_RESPONSE_TYPE_RECEIVE    0x00
#define PACKET_RESPONSE_TYPE_SEND       0x01

#define ADDRESS_MSB(x)  ((uint8_t)((x) >> 24) & 0xFF)
#define ADDRESS_02(x)   ((uint8_t)((x) >> 16) & 0xFF)
#define ADDRESS_01(x)   ((uint8_t)((x) >> 8) & 0xFF)
#define ADDRESS_LSB(x)  ((uint8_t)(x) & 0xFF)

#ifdef DEBUG
#define CC_CONCAT_S(s1, s2)     s1 ## s2
#define CC_CONCAT(s1, s2)       CC_CONCAT_S(s1, s2)
#define DEBUG_PRINTF(fmt, ...) do {                                             \
        (void)fprintf(stderr, "%s():L%i:" " " fmt, __FUNCTION__, __LINE__,      \
            ##__VA_ARGS__);                                                     \
} while(false)
#else
#define DEBUG_PRINTF(x...)
#endif /* DEBUG */

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
};

static uint32_t datalink_error = DATALINK_OK;

static FT_HANDLE ft_handle = NULL;
static FT_STATUS ft_error = FT_OK;

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
        "DATALINK_BAD_REQUEST"
};

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

static int datalink_init(void);
static void datalink_close(void);
static int datalink_read(uint8_t *, uint32_t);
static int datalink_write(uint8_t *, uint32_t);
static int datalink_packet_check(const uint8_t *, uint32_t);

/* Helpers */
static void convert_error(void);
static uint8_t packet_checksum(const uint8_t *, uint32_t);

/* Actions */
static int action_download(const char *, uint32_t, uint32_t);
static int action_upload(const char *, uint32_t, bool);
static int action_execute(const char *, uint32_t);

/*
 *
 */
static int
datalink_init(void)
{
#define MAX_DEVICES 10

        DEBUG_PRINTF("Enter\n");

        char *devices_ptr_list[MAX_DEVICES + 1];
        char devices_list[MAX_DEVICES][64];

        int devices_cnt;
        int idx;

        for(idx = 0; idx < MAX_DEVICES; idx++) {
                devices_ptr_list[idx] = devices_list[idx];
        }
        devices_ptr_list[MAX_DEVICES] = NULL;

        if ((ft_error = FT_ListDevices(devices_ptr_list, &devices_cnt,
                    FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER)) != FT_OK) {
                return -1;
        }

        if (devices_cnt == 0) {
                ft_error = FT_DEVICE_NOT_FOUND;
                return -1;
        }

        for(idx = 0; ( (idx < MAX_DEVICES) && (idx < devices_cnt) ); idx++) {
                DEBUG_PRINTF("Device #%2d SN: %s\n", idx, devices_list[idx]);
        }

        if((ft_error = FT_OpenEx(devices_list[0], FT_OPEN_BY_SERIAL_NUMBER, &ft_handle)) != FT_OK) {
                return -1;
        }
        if((ft_error = FT_SetBaudRate(ft_handle, BAUD)) != FT_OK) {
                return -1;
        }
        if((ft_error = FT_SetDataCharacteristics(ft_handle, DATA_BITS, STOP_BITS, PARITY)) != FT_OK) {
                return -1;
        }
        if ((ft_error = FT_SetTimeouts(ft_handle, RX_TIMEOUT, TX_TIMEOUT)) != FT_OK) {
                return -1;
        }

        return 0;
}

/*
 *
 */
static void
datalink_close(void)
{
        FT_Close(ft_handle);
}

/*
 *
 */
static int
datalink_read(uint8_t *read_buffer, uint32_t len)
{
#define MAX_TRIES (128 * 1024)

        DEBUG_PRINTF("Enter\n");

        if (len > PACKET_TOTAL_SIZE) {
                datalink_error = DATALINK_IO_ERROR;
                return -1;
        }

        DWORD queued_amount;
        queued_amount = 0;

        uint32_t timeout;
        timeout = 0;

        ft_error = FT_OK;
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

        if(queued_amount > 0) {
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
                for (buffer_idx = 0; buffer_idx < PACKET_HEADER_SIZE; buffer_idx++) {
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
datalink_write(uint8_t *write_buffer, uint32_t len)
{
        DEBUG_PRINTF("Enter\n");
        DEBUG_PRINTF("Writing %iB\n", len);

        if (len > PACKET_TOTAL_SIZE) {
                datalink_error = DATALINK_IO_ERROR;
                return -1;
        }

#ifdef DEBUG
        uint32_t buffer_idx;
        for (buffer_idx = 0; buffer_idx < PACKET_HEADER_SIZE; buffer_idx++) {
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
action_upload(const char *input_file, uint32_t base_address, bool execute)
{
        DEBUG_PRINTF("Enter\n");

        FILE *iffp;
        iffp = NULL;

        /* Sanity check */
        if (input_file == NULL) {
                datalink_error = DATALINK_BAD_REQUEST;
                goto error;
        }
        if (*input_file == '\0') {
                datalink_error = DATALINK_BAD_REQUEST;
                goto error;
        }

        if (base_address == 0x00000000) {
                datalink_error = DATALINK_BAD_REQUEST;
                goto error;
        }

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

        size_t read;

        uint32_t address;
        address = base_address;

        uint8_t execute_buffer[PACKET_TOTAL_SIZE];
        memset(execute_buffer, 0x00, PACKET_TOTAL_SIZE);
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
        if ((read = fread(&execute_buffer[8], 1, 2, iffp)) != 2) {
                datalink_error = DATALINK_FILE_IO_ERROR;
                goto error;
        }
        execute_buffer[PACKET_HEADER_SIZE + 2 - 1] =
            packet_checksum(execute_buffer, (PACKET_HEADER_SIZE + 2) - 1);

        len -= 2;
        address += 2;

        uint8_t read_buffer[PACKET_TOTAL_SIZE];
        uint8_t buffer[PACKET_TOTAL_SIZE];

        do {
                uint8_t transfer_len;
                transfer_len = ((len - PACKET_DATA_SIZE) < 0)
                    ? len % PACKET_DATA_SIZE
                    : PACKET_DATA_SIZE;

                uint8_t *write_buffer;

                DEBUG_PRINTF("Transferring %iB (%iB) to 0x%08X\n",
                    transfer_len, len, address);

                if (len == 0) {
                        write_buffer = execute_buffer;
                        transfer_len = 2;
                } else {
                        write_buffer = buffer;

                        memset(buffer, 0x00, PACKET_TOTAL_SIZE);
                        buffer[0] = 0x5A;
                        buffer[1] = (7 + transfer_len) > (PACKET_TOTAL_SIZE - 2)
                            ? PACKET_TOTAL_SIZE - 2
                            : 7 + transfer_len;
                        buffer[2] = PACKET_TYPE_SEND;
                        buffer[3] = ADDRESS_MSB(address);
                        buffer[4] = ADDRESS_02(address);
                        buffer[5] = ADDRESS_01(address);
                        buffer[6] = ADDRESS_LSB(address);
                        buffer[7] = transfer_len;
                        if ((read = fread(&buffer[8], 1, transfer_len, iffp)) != transfer_len) {
                                datalink_error = DATALINK_FILE_IO_ERROR;
                                DEBUG_PRINTF("fread() %zu, %i\n", read, transfer_len);
                                goto error;
                        }
                        buffer[PACKET_HEADER_SIZE + transfer_len - 1] =
                            packet_checksum(buffer, (PACKET_HEADER_SIZE + transfer_len) - 1);
                }

                if ((datalink_write(write_buffer, PACKET_HEADER_SIZE + transfer_len)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                memset(read_buffer, 0x00, PACKET_TOTAL_SIZE);
                if ((datalink_read(read_buffer, PACKET_HEADER_SIZE)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                if ((datalink_packet_check(read_buffer, PACKET_RESPONSE_TYPE_SEND)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }
                /* if (read_buffer[0] != 0xA5) { */
                /*         DEBUG_PRINTF("ERROR\n"); */
                /*         goto error; */
                /* } */

                len -= transfer_len;
                address += transfer_len;
        } while (len >= 0);

        goto exit;

        int error;

error:
        error = -1;

        (void)printf("ERROR\n");

exit:
        error = 0;

        if (iffp != NULL) {
                fclose(iffp);
        }

        datalink_error = DATALINK_OK;

        return error;
}

/*
 *
 */
static int
action_execute(const char *input_file, uint32_t base_address)
{
        DEBUG_PRINTF("Enter\n");

        return action_upload(input_file, base_address, /* execute = */ true);
}

/*
 *
 */
static int
action_download(const char *output_file, uint32_t base_address, uint32_t len)
{
#define STATE_RECEIVE_FIRST     0
#define STATE_RECEIVE_MIDDLE    1
#define STATE_RECEIVE_FINAL     2
#define STATE_FINISH            3

        DEBUG_PRINTF("Enter\n");

        /* Sanity check */
        if (output_file == NULL) {
                datalink_error = DATALINK_BAD_REQUEST;
        }
        if (*output_file == '\0') {
                datalink_error = DATALINK_BAD_REQUEST;
        }

        if (base_address == 0x00000000) {
                datalink_error = DATALINK_BAD_REQUEST;
        }

        if (len <= 1) {
                datalink_error = DATALINK_BAD_REQUEST;
        }

        uint8_t buffer_first[PACKET_HEADER_SIZE] = {
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

        uint8_t buffer_middle[PACKET_HEADER_SIZE] = {
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

        uint8_t buffer_final[PACKET_HEADER_SIZE] = {
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

        FILE *offp;
        offp = NULL;
        if ((offp = fopen(output_file, "wb+")) == NULL) {
                return -1;
        }

        uint8_t *read_buffer;
        read_buffer = (uint8_t *)malloc(PACKET_TOTAL_SIZE);
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
        tier_1 = len <= PACKET_DATA_SIZE;

        bool tier_2;
        tier_2 = (len > PACKET_DATA_SIZE) && (len <= (2 * PACKET_DATA_SIZE));

        bool tier_3;
        tier_3 = len > (2 * PACKET_DATA_SIZE);

        while (true) {
                switch (state) {
                case STATE_RECEIVE_FIRST:
                        DEBUG_PRINTF("%s", "State STATE_RECEIVE_FIRST\n");

                        write_buffer = &buffer_first[0];

                        if (tier_1) {
                                write_buffer[7] = len - 1;
                                state = STATE_RECEIVE_FINAL;
                        } else if (tier_2) {
                                write_buffer[7] = PACKET_DATA_SIZE;
                                state = STATE_RECEIVE_FINAL;
                        } else if (tier_3) {
                                write_buffer[7] = PACKET_DATA_SIZE;
                                /* The number of middle packets to send */
                                count = (len - PACKET_DATA_SIZE - 1) / PACKET_DATA_SIZE;

                                state = STATE_RECEIVE_MIDDLE;
                        }
                        break;
                case STATE_RECEIVE_MIDDLE:
                        write_buffer = &buffer_middle[0];

                        DEBUG_PRINTF("State STATE_RECEIVE_MIDDLE (%i)\n", count);

                        write_buffer[7] = PACKET_DATA_SIZE;

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
                                write_buffer[7] = len - PACKET_DATA_SIZE;
                        } else if (tier_3) {
                                write_buffer[7] = (len - PACKET_DATA_SIZE) % PACKET_DATA_SIZE;
                                write_buffer[7] = (write_buffer[7] == 0)
                                    ? PACKET_DATA_SIZE
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
                    PACKET_HEADER_SIZE - 1);

                if ((datalink_write(write_buffer, PACKET_HEADER_SIZE)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                memset(read_buffer, 0x00, PACKET_TOTAL_SIZE);
                if ((datalink_read(read_buffer, PACKET_HEADER_SIZE + write_buffer[7])) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                if ((datalink_packet_check(read_buffer, PACKET_RESPONSE_TYPE_RECEIVE)) < 0) {
                        DEBUG_PRINTF("datalink_error = %s\n",
                            datalink_error_strings[datalink_error]);
                        goto error;
                }

                address += read_buffer[7] + 1;

                (void)fwrite(&read_buffer[PACKET_HEADER_SIZE - 1],
                    1,
                    read_buffer[7],
                    offp);
        }

        goto exit;

        int error;
error:
        error = -1;

        (void)printf("ERROR\n");

exit:
        error = 0;

        if (offp != NULL) {
                fclose(offp);
        }

        if (read_buffer != NULL) {
                free(read_buffer);
        }

        datalink_error = DATALINK_OK;

        return error;
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
                if (buffer_idx < PACKET_HEADER_SIZE) {
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
datalink_packet_check(const uint8_t *buffer, uint32_t response_type)
{
        static const uint8_t packet_response_error[PACKET_HEADER_SIZE] = {
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

        static const uint8_t packet_response_send[PACKET_HEADER_SIZE] = {
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

        /* Check if the response packet is an error packet */
        uint32_t buffer_idx;
        bool bad_packet;

        for (buffer_idx = 0;
             ((buffer_idx < PACKET_HEADER_SIZE) &&
                 (buffer[buffer_idx] == packet_response_error[buffer_idx]));
             buffer_idx++);

        bad_packet = buffer_idx == PACKET_HEADER_SIZE;
        if (bad_packet) {
                datalink_error = DATALINK_ERROR_PACKET;
                return -1;
        }

        uint8_t calc_checksum;
        uint8_t checksum;

        switch (response_type) {
        case PACKET_RESPONSE_TYPE_RECEIVE:
                calc_checksum = packet_checksum(buffer,
                    (PACKET_HEADER_SIZE - 1) + buffer[7]);
                checksum = buffer[(PACKET_HEADER_SIZE) + buffer[7] - 1];

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
                     ((buffer_idx < PACKET_HEADER_SIZE) &&
                         (buffer[buffer_idx] == packet_response_send[buffer_idx]));
                     buffer_idx++);
                bad_packet = buffer_idx != PACKET_HEADER_SIZE;
                if (bad_packet) {
                        datalink_error = DATALINK_ERROR_PACKET;
                        return -1;
                }
        }

        datalink_error = DATALINK_OK;

        return 0;
}

int
main(int argc, char **argv)
{
        if ((datalink_init()) < 0) {
                return -1;
        }

        if (argc == 1) {
                (void)fprintf(stderr, "data-link binary\n");
                return 1;
        }

        action_execute(argv[1], 0x26004000);

        datalink_close();

        return 0;
}
