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

#include <assert.h>
#include <stdbool.h>

#include <cpu/divu.h>

#include "fix16.h"

fix16_t
fix16_overflow_add(fix16_t a, fix16_t b)
{
        /* Use unsigned integers because overflow with signed integers is an
         * undefined operation <http://www.airs.com/blog/archives/120> */
        uint32_t ta = a;
        uint32_t tb = b;
        uint32_t sum = ta + tb;

        /* Overflow can only happen if sign of a == sign of b, and then it
         * causes sign of sum != sign of a */
        if ((((ta ^ tb) & 0x80000000) == 0) && (((ta ^ sum) & 0x80000000) != 0)) {
                return FIX16_OVERFLOW;
        }

        return sum;
}

fix16_t
fix16_overflow_sub(fix16_t a, fix16_t b)
{
        uint32_t ta = a;
        uint32_t tb = b;
        uint32_t diff = ta - tb;

        /* Overflow can only happen if sign of a != sign of b, and then it
         * causes sign of diff != sign of a. */
        if ((((ta ^ tb) & 0x80000000) != 0) && (((ta ^ diff) & 0x80000000) != 0)) {
                return FIX16_OVERFLOW;
        }

        return diff;
}

fix16_t
fix16_div(fix16_t dividend, fix16_t divisor)
{
        cpu_divu_fix16_set(dividend, divisor);

        return cpu_divu_quotient_get();
}

fix16_t
fix16_lerp(fix16_t a, fix16_t b, fix16_t t)
{
        return (fix16_mul(a, (FIX16_ONE - t)) + fix16_mul(b, t));
}

fix16_t
fix16_lerp8(fix16_t a, fix16_t b, const uint8_t t)
{
        /* Make sure when t =   0 => 0.0f
         *                t = 255 => 1.0f */
        const fix16_t fixed_t = fix16_int32_from((t + 1) >> 8);

        return (fix16_mul(a, (FIX16_ONE - t)) + fix16_mul(b, fixed_t));
}
