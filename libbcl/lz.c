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

#include <stdint.h>

/* Read uint32_teger with variable number of bytes depending on value */
static int _var_size_read(uint32_t *x, uint8_t *buf);

void
bcl_lz_decompress(uint8_t *in, uint8_t *out, uint32_t in_size)
{
        uint8_t marker, symbol;
        uint32_t  i, inpos, outpos, length, offset;

        /* Do we have anything to uncompress? */
        if (in_size < 1) {
                return;
        }

        /* Get marker symbol from input stream */
        marker = in[ 0 ];
        inpos = 1;

        /* Main decompression loop */
        outpos = 0;

        do {
                symbol = in[ inpos ++ ];

                if (symbol == marker) {
                        /* We had a marker byte */
                        if (in[ inpos ] == 0) {
                                /* It was a single occurrence of the marker byte */
                                out[ outpos ++ ] = marker;
                                ++ inpos;
                        } else {
                                /* Extract true length and offset */
                                inpos += _var_size_read(&length, &in[ inpos ]);
                                inpos += _var_size_read(&offset, &in[ inpos ]);

                                /* Copy corresponding data from history window */
                                for (i = 0; i < length; ++ i) {
                                        out[ outpos ] = out[ outpos - offset ];
                                        ++ outpos;
                                }
                        }
                } else {
                        /* No marker, plain copy */
                        out[ outpos ++ ] = symbol;
                }
        } while (inpos < in_size);
}

static int
_var_size_read(uint32_t *x, uint8_t *buf)
{
        uint32_t y, b, num_bytes;

        /* Read complete value (stop when byte contains zero in 8:th bit) */
        y = 0;
        num_bytes = 0;

        do {
                b = (uint32_t)(*buf ++);
                y = (y << 7) | (b & 0x0000007F);

                ++num_bytes;
        } while ((b & 0x00000080) != 0x00000000);

        /* Store value in x */
        *x = y;

        /* Return number of bytes read */
        return num_bytes;
}
