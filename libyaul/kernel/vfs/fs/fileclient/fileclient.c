#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "fileclient.h"

#include <crc.h>

#define FILESERVER_CMD_FILE     (0x00)
#define FILESERVER_CMD_SIZE     (0x01)
#define FILESERVER_CMD_QUIT     (0x7F)
#define FILESERVER_CMD_INVALID  (0xFF)

static char _filename_buffer[32];

static void _filename_send(const char *filename);

void
fileclient_sector_request(const char *filename, const uint32_t sector_offset,
    void *dst)
{
        usb_cart_byte_send(FILESERVER_CMD_FILE);

        _filename_send(filename);

        usb_cart_long_send(sector_offset);

        while (true) { 
                uint8_t *byte_buffer;
                byte_buffer = (uint8_t *)dst;

                uint32_t i;

                for (i = 0; i < FILECLIENT_SECTOR_SIZE; i++) {
                        byte_buffer[i] = usb_cart_byte_read();
                }

                crc_t crc;
                crc = crc_calculate(dst, FILECLIENT_SECTOR_SIZE);

                usb_cart_byte_send(crc);

                crc_t read_crc;
                read_crc = usb_cart_byte_read();

                if (read_crc == crc) {
                        break;
                }
        }
}

uint32_t
fileclient_sector_count_request(const char *filename)
{
        usb_cart_byte_send(FILESERVER_CMD_SIZE);

        _filename_send(filename);

        uint32_t sector_count;
        sector_count = usb_cart_long_read();

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
