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

#include "fix16.h"

fix16_t
fix16_overflow_add(fix16_t a, fix16_t b)
{
        /* Use unsigned integers because overflow with signed integers is an
         * undefined operation <http://www.airs.com/blog/archives/120> */
        uint32_t _a = a, _b = b;
        uint32_t sum = _a + _b;

        /* Overflow can only happen if sign of a == sign of b, and then it
         * causes sign of sum != sign of a */
        if ((((_a ^ _b) & 0x80000000) == 0) && (((_a ^ sum) & 0x80000000) != 0)) {
                return FIX16_OVERFLOW;
        }

        return sum;
}

fix16_t
fix16_overflow_sub(fix16_t a, fix16_t b)
{
        uint32_t _a = a;
        uint32_t _b = b;
        uint32_t diff = _a - _b;

        /* Overflow can only happen if sign of a != sign of b, and then it
         * causes sign of diff != sign of a. */
        if ((((_a ^ _b) & 0x80000000) != 0) && (((_a ^ diff) & 0x80000000) != 0)) {
                return FIX16_OVERFLOW;
        }

        return diff;
}

fix16_t
fix16_lerp(fix16_t a, fix16_t b, fix16_t t)
{
        fix16_t t_clamped;
        t_clamped = fix16_clamp(FIX16(0.0f), FIX16_ONE, t);

        return (a * (FIX16_ONE - t_clamped)) + (b * t_clamped);
}

fix16_t
fix16_lerp8(fix16_t a __unused, fix16_t b __unused, uint8_t t __unused)
{
        return FIX16(0.0f);
}
