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

#ifndef _YAUL_MATH_UINT32_H_
#define _YAUL_MATH_UINT32_H_

#include <sys/cdefs.h>

#include <stdint.h>

__BEGIN_DECLS

extern uint32_t uint32_log2(uint32_t value);
extern uint32_t uint32_loop_log2(uint32_t value);
extern uint32_t uint32_pow2_round_next(uint32_t value);

static inline uint32_t __always_inline
uint32_pow2_round(uint32_t value, uint32_t pow)
{
        return (((value + ((1 << pow) - 1)) >> pow) << pow);
}

__END_DECLS

#endif /* !_YAUL_MATH_UINT32_H_ */
