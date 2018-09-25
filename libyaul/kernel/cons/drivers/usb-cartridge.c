/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <bus/a/cs0/usb-cartridge/usb-cartridge.h>

#include "../drivers.h"

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
static void _write(struct cons *);
#endif /* HAVE_DEV_CARTRIDGE */

void
cons_usb_cartridge_init(struct cons *cons)
{
        cons->write = _write;

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
        usb_cartridge_init();
#endif /* HAVE_DEV_CARTRIDGE */
}

#if HAVE_DEV_CARTRIDGE == 1 /* USB flash cartridge */
static void
_write(struct cons *cons)
{
        uint32_t char_count;
        char_count = 0;

        bool send_newline;
        send_newline = false;

        uint32_t row;
        for (row = 0; row < cons->rows; row++) {
                uint32_t prev_char_count;
                prev_char_count = char_count;

                uint32_t col;
                for (col = 0; col < cons->cols; col++) {
                        struct cons_buffer *cb;
                        cb = &cons->buffer[col + (row * cons->cols)];

                        if (cb->glyph == '\0') {
                                break;
                        }

                        usb_cartridge_byte_send(cb->glyph);

                        char_count++;
                        send_newline = false;
                }

                /* Force clear the row. Ideally, we don't want to do
                 * this here */
                cons->buffer[row * cons->cols].glyph = '\0';

                /* Send a newline iff we've sent characters and we
                 * didn't reach the end of the row */
                bool sent_chars;
                sent_chars = (char_count - prev_char_count) > 0;

                if (send_newline || (sent_chars && (col < cons->cols))) {
                        usb_cartridge_byte_send('\n');
                }

                /* If we've reached the end of the row and sent characters,
                 * then if the next row is empty, it's because the row
                 * has a newline */
                send_newline = sent_chars && (col == cons->cols);
        }

        /* If we've sent at least one character, wait for the
         * response */
        if (char_count > 0) {
                usb_cartridge_byte_send('\0');
        }
}
#endif /* HAVE_DEV_CARTRIDGE */
