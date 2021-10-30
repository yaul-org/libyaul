/*-
 * Copyright (c) Authors of libfixmath
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "uint32.h"

/* Performs an unsigned log-base2 on the specified unsigned integer and returns
 * the result */
uint32_t
uint32_log2(uint32_t value)
{
        if (value == 0) {
                return 0;
        }

        uint32_t result;
        result = 0;

        if (value >= (1 << 16)) {
                value >>= 16;
                result += 16;
        }

        if (value >= (1 << 8)) {
                value >>= 8;
                result += 8;
        }

        if (value >= (1 << 4)) {
                value >>= 4;
                result += 4;
        }

        if (value >= (1 << 2)) {
                value >>= 2;
                result += 2;
        }

        if (value >= (1 << 1)) {
                result += 1;
        }

        return result;
}

uint32_t
uint32_loop_log2(uint32_t value)
{
        uint32_t l;
        l = 0;

        while ((value >> l) > 1) {
                l++;
        }

        return l;
}

uint32_t
uint32_pow2_round_next(uint32_t value)
{
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value++;

        return value;
}
