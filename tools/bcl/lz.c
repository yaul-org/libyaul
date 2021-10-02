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

#define PROGNAME "bcl_lz"

/* Maximum offset (can be any size < 2^31). Lower values give faster
 * compression, while higher values gives better compression. The default value
 * of 100000 is quite high. Experiment to see what works best for you. */
#define LZ_MAX_OFFSET 2147483648

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "shared.h"

static int _lz_compress(uint8_t *in, uint8_t *out, uint32_t insize, uint32_t max_offset);

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
            _lz_compress(input_file.buffer, out_buffer, input_file.buffer_len, LZ_MAX_OFFSET);

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

/* Return maximum length string match */
static uint32_t
_lz_string_compare(uint8_t *str1, uint8_t *str2, uint32_t minlen, uint32_t maxlen)
{
        uint32_t len;

        for (len = minlen; (len < maxlen) && (str1[len] == str2[len]); ++len);

        return len;
}

/* Write integer with variable number of bytes depending on value */
static int _lz_var_size_write(uint32_t x, uint8_t *buf)
{
        uint32_t y;
        int num_bytes, i, b;

        /* Determine number of bytes needed to store the number x */
        y = x >> 3;

        for (num_bytes = 5; num_bytes >= 2; --num_bytes) {
                if (y & 0xfe000000) {
                        break;
                }

                y <<= 7;
        }

        /* Write all bytes, seven bits in each, with 8:th bit set for all */
        /* but the last byte. */
        for (i = num_bytes - 1; i >= 0; --i) {
                b = (x >> (i * 7)) & 0x0000007f;

                if (i > 0) {
                        b |= 0x00000080;
                }

                *buf++ = (uint8_t) b;
        }

        /* Return number of bytes written */
        return num_bytes;
}

static int
_lz_compress(uint8_t *in, uint8_t *out, uint32_t in_size, uint32_t max_offset)
{
        uint8_t marker, symbol;
        uint32_t inpos, outpos, bytesleft, i;
        uint32_t maxoffset, offset, bestoffset;
        uint32_t maxlength, length, bestlength;
        uint32_t histogram[256];
        uint8_t *ptr1, *ptr2;

        if (max_offset >= LZ_MAX_OFFSET) {
                max_offset = LZ_MAX_OFFSET;
        }

        /* Do we have anything to compress? */
        if (in_size < 1) {
                return 0;
        }

        /* Create histogram */
        for (i = 0; i < 256; ++i) {
                histogram[i] = 0;
        }

        for (i = 0; i < in_size; ++i) {
                ++histogram[in[i]];
        }

        /* Find the least common byte, and use it as the marker symbol */
        marker = 0;

        for (i = 1; i < 256; ++i) {
                if (histogram[i ] < histogram[marker]) {
                        marker = i;
                }
        }

        /* Remember the marker symbol for the decoder */
        out[0] = marker;

        /* Start of compression */
        inpos = 0;
        outpos = 1;

        /* Main compression loop */
        bytesleft = in_size;

        do {
                /* Determine most distant position */
                if (inpos > max_offset) {
                        maxoffset = max_offset;
                } else {
                        maxoffset = inpos;
                }

                /* Get pointer to current position */
                ptr1 = &in[inpos];

                /* Search history window for maximum length string match */
                bestlength = 3;
                bestoffset = 0;

                for (offset = 3; offset <= maxoffset; ++offset) {
                        /* Get pointer to candidate string */
                        ptr2 = &ptr1[- (int) offset];

                        /* Quickly determine if this is a candidate (for speed) */
                        if ((ptr1[0 ] == ptr2[0]) &&
                                        (ptr1[bestlength ] == ptr2[bestlength])) {
                                /* Determine maximum length for this offset */
                                maxlength = (bytesleft < offset ? bytesleft : offset);

                                /* Count maximum length match at this offset */
                                length = _lz_string_compare(ptr1, ptr2, 0, maxlength);

                                /* Better match than any previous match? */
                                if (length > bestlength) {
                                        bestlength = length;
                                        bestoffset = offset;
                                }
                        }
                }

                /* Was there a good enough match? */
                if ((bestlength >= 8) ||
                    ((bestlength == 4) && (bestoffset <= 0x0000007f)) ||
                    ((bestlength == 5) && (bestoffset <= 0x00003fff)) ||
                    ((bestlength == 6) && (bestoffset <= 0x001fffff)) ||
                    ((bestlength == 7) && (bestoffset <= 0x0fffffff))) {
                        out[outpos++] = (uint8_t) marker;
                        outpos += _lz_var_size_write(bestlength, &out[outpos]);
                        outpos += _lz_var_size_write(bestoffset, &out[outpos]);
                        inpos += bestlength;
                        bytesleft -= bestlength;
                } else {
                        /* Output single byte (or two bytes if marker byte) */
                        symbol = in[inpos++];
                        out[outpos++] = symbol;

                        if (symbol == marker) {
                                out[outpos++] = 0;
                        }

                        --bytesleft;
                }
        } while (bytesleft > 3);

        /* Dump remaining bytes, if any */
        while (inpos < in_size) {
                if (in[inpos] == marker) {
                        out[outpos++] = marker;
                        out[outpos++] = 0;
                } else {
                        out[outpos++] = in[inpos];
                }

                ++inpos;
        }

        return outpos;
}
