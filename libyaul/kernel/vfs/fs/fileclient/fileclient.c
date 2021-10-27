#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "fileclient.h"

#include <ssload.h>
#include <crc.h>

static char _filename_buffer[32];

static void _filename_send(const char *filename);

void
fileclient_sector_request(const char *filename, const uint32_t sector_offset,
    void *dst)
{
        usb_cart_byte_send(SSLOAD_API_CMD_FILE);

        _filename_send(filename);

        usb_cart_long_send(sector_offset);

        while (true) {
                uint8_t *byte_buffer;
                byte_buffer = (uint8_t *)dst;

                for (uint32_t i = 0; i < FILECLIENT_SECTOR_SIZE; i++) {
                        byte_buffer[i] = usb_cart_byte_read();
                }

                const crc_t crc = crc_calculate(dst, FILECLIENT_SECTOR_SIZE);

                usb_cart_byte_send(crc);

                const crc_t read_crc = usb_cart_byte_read();

                if (read_crc == crc) {
                        break;
                }
        }
}

uint32_t
fileclient_sector_count_request(const char *filename)
{
        usb_cart_byte_send(SSLOAD_API_CMD_SECTOR_COUNT);

        _filename_send(filename);

        uint32_t sector_count;
        sector_count = usb_cart_long_read();

        return sector_count;
}

uint32_t
fileclient_byte_size_request(const char *filename)
{
        usb_cart_byte_send(SSLOAD_API_CMD_BYTE_SIZE);

        _filename_send(filename);

        const uint32_t sector_count = usb_cart_long_read();

        return sector_count;
}

static void
_filename_send(const char *filename)
{
        (void)strcpy(_filename_buffer, filename);

        for (uint32_t i = 0; i < (sizeof(_filename_buffer) - 1); i++) {
                usb_cart_byte_send(_filename_buffer[i]);
        }

        usb_cart_byte_send('\0');
}
