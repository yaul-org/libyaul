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
fix16_sin(fix16_t radians __unused)
{
        return FIX16(0.0f);
}

fix16_t
fix16_cos(fix16_t radians)
{
        return fix16_sin(radians + (FIX16_PI >> 1));
}

fix16_t
fix16_tan(fix16_t radians __unused)
{
        return FIX16(0.0f);
}

fix16_t
fix16_asin(fix16_t x __unused)
{
        return FIX16(0.0f);
}

fix16_t
fix16_acos(fix16_t x)
{
        return ((FIX16_PI >> 1) - fix16_asin(x));
}

fix16_t
fix16_atan(fix16_t x)
{
        return fix16_atan2(x, FIX16_ONE);
}

fix16_t
fix16_atan2(fix16_t x __unused, fix16_t y __unused)
{
        return FIX16(0.0f);
}
