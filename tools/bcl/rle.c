/* Copyright (c) 2003-2006 Marcus Geelnard
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not claim
 *    that you wrote the original software. If you use this software in a
 *    product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution. */

/* Note: This code is based on the code found in "codrle2.c" and "dcodrle2.c" by
 *       David Bourgin, as described in "Introduction to the losslessy
 *       compression schemes", 1994. The main differences from David's
 *       implementation are the addition of long (15-bit) run counts, the
 *       removal of file I/O (this implementation works solely with preallocated
 *       memory buffers), and that the code is now 100% reentrant. */

#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "shared.h"

#define PROGNAME "bcl_rle"

static int _rle_compress(const void *in, void *out, size_t in_size);

int
main(int argc, char *argv[])
{
        if (argc != 3) {
                print_usage(PROGNAME);

                return 0;
        }

        const char * const in_filename = argv[1];
        const char * const out_filename = argv[2];

        input_file_t input_file;

        if ((input_file_open(in_filename, &input_file)) != 0) {
                print_errno(PROGNAME);

                return 1;
        }

        /* Allocate a buffer of equal to twice the size since we don't know how
         * big the out file size will be */
        void *out_buffer;

        /* According to the original sources, the output buffer must be 0.4%
         * larger plus 1 byte */
        size_t out_size = (size_t)floor(1.004f * (float)input_file.buffer_len) + 1;

        if ((out_buffer = malloc(out_size)) == NULL) {
                print_errno(PROGNAME);

                return 1;
        }

        const uint32_t out_file_size =
                _rle_compress(input_file.buffer, out_buffer, input_file.buffer_len);

        (void)printf("%zu -> %"PRIu32"\n", input_file.buffer_len, out_file_size);

        if (out_file_size == 0) {
                fprintf(stderr, "Error: %s: RLE compression failed\n", PROGNAME);

                return 1;
        }

        if ((output_file_write(out_filename, out_buffer, out_file_size)) != 0) {
                print_errno(PROGNAME);

                return 1;
        }

        input_file_close(&input_file);

        free(out_buffer);

        return 0;
}

static void
_rle_rep_write(uint8_t *out, uint32_t *outpos, uint8_t marker, uint8_t symbol, uint32_t count)
{
        uint32_t i, idx;

        idx = *outpos;

        if (count <= 3) {
                if (symbol == marker) {
                        out[idx++] = marker;
                        out[idx++] = count - 1;
                } else {
                        for (i = 0; i < count; ++i) {
                                out[idx++] = symbol;
                        }
                }
        } else {
                out[idx++] = marker;
                -- count;

                if (count >= 128) {
                        out[idx++] = (count >> 8) | 0x80;
                }

                out[idx++] = count & 0xff;
                out[idx++] = symbol;
        }

        *outpos = idx;
}

/* Encode a non-repeating symbol, 'symbol'. 'marker' is the marker symbol, and
 * special care has to be taken for the case when 'symbol' == 'marker'. */
static void
_rle_nonrep_write(uint8_t *out, uint32_t *outpos, uint8_t marker, uint8_t symbol)
{
        uint32_t idx;

        idx = *outpos;

        if (symbol == marker) {
                out[idx++] = marker;
                out[idx++] = 0;
        } else {
                out[idx++] = symbol;
        }

        *outpos = idx;
}

static int
_rle_compress(const void *in, void *out, size_t in_size)
{
        uint8_t byte1, byte2, marker;
        uint32_t inpos;
        uint32_t outpos;
        uint32_t count;
        uint32_t i;
        uint32_t histogram[256];

        const uint8_t * const in_p = in;
        uint8_t * const out_p = out;

        /* Do we have anything to compress? */
        if (in_size < 1) {
                return 0;
        }

        /* Create histogram */
        for (i = 0; i < 256; ++i) {
                histogram[i] = 0;
        }

        for (i = 0; i < in_size; ++i) {
                ++histogram[in_p[i]];
        }

        /* Find the least common byte, and use it as the repetition marker */
        marker = 0;

        for (i = 1; i < 256; ++i) {
                if (histogram[i] < histogram[marker]) {
                        marker = i;
                }
        }

        /* Remember the repetition marker for the decoder */
        out_p[0] = marker;
        outpos = 1;

        /* Start of compression */
        byte1 = in_p[0];
        inpos = 1;
        count = 1;

        /* Are there at least two bytes? */
        if (in_size >= 2) {
                byte2 = in_p[inpos++];
                count = 2;

                /* Main compression loop */
                do {
                        if (byte1 == byte2) {
                                /* Do we meet only a sequence of identical bytes? */
                                while ((inpos < in_size) && (byte1 == byte2) &&
                                       (count < 32768)) {
                                        byte2 = in_p[inpos++];
                                        ++count;
                                }

                                if (byte1 == byte2) {
                                        _rle_rep_write(out, &outpos, marker, byte1, count);

                                        if (inpos < in_size) {
                                                byte1 = in_p[inpos++];
                                                count = 1;
                                        } else {
                                                count = 0;
                                        }
                                } else {
                                        _rle_rep_write(out, &outpos, marker, byte1, count - 1);
                                        byte1 = byte2;
                                        count = 1;
                                }
                        } else {
                                /* No, then don't handle the last byte */
                                _rle_nonrep_write(out, &outpos, marker, byte1);
                                byte1 = byte2;
                                count = 1;
                        }

                        if (inpos < in_size) {
                                byte2 = in_p[inpos++];
                                count = 2;
                        }
                } while ((inpos < in_size) || (count >= 2));
        }

        /* One byte left? */
        if (count == 1) {
                _rle_nonrep_write(out, &outpos, marker, byte1);
        }

        return outpos;
}
