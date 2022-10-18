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

#include <stdint.h>

void
bcl_rle_decompress(uint8_t *in, uint8_t *out, uint32_t in_size)
{
        uint8_t marker, symbol;
        uint32_t  i, inpos, outpos, count;

        /* Do we have anything to uncompress? */
        if (in_size < 1) {
                return;
        }

        /* Get marker symbol from input stream */
        inpos = 0;
        marker = in[ inpos ++ ];

        /* Main decompression loop */
        outpos = 0;

        do {
                symbol = in[ inpos ++ ];

                if (symbol == marker) {
                        /* We had a marker byte */
                        count = in[ inpos ++ ];

                        if (count <= 2) {
                                /* Counts 0, 1 and 2 are used for marker byte repetition
                                   only */
                                for (i = 0; i <= count; ++ i) {
                                        out[ outpos ++ ] = marker;
                                }
                        } else {
                                if (count & 0x80) {
                                        count = ((count & 0x7f) << 8) + in[ inpos ++ ];
                                }

                                symbol = in[ inpos ++ ];

                                for (i = 0; i <= count; ++ i) {
                                        out[ outpos ++ ] = symbol;
                                }
                        }
                } else {
                        /* No marker, plain copy */
                        out[ outpos ++ ] = symbol;
                }
        } while (inpos < in_size);
}
