/*-
 * Copyright (c) 1990, 1993 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>

#include <stdint.h>
#include <string.h>

typedef int32_t word; /* "word" used for optimal copy speed */

/*
 * Copy a block of memory, handling overlap.
 * This is the routine that actually implements
 * (the portable versions of) bcopy, memcpy, and memmove.
 */

void *
memcpy(void *dst0, const void *src0, size_t length)
{
        char *dst = dst0;
        const char *src = src0;
        size_t t;

        if ((length == 0) || (dst == src)) { /* nothing to do */
                return dst0;
        }

        /*
         * Macros: loop-t-times; and loop-t-times, t>0
         */
#define TLOOP(s) if (t) TLOOP1(s)
#define TLOOP1(s) do { s; } while (--t)

        if ((uint32_t)dst < (uint32_t)src) {
                /*
                 * Copy forward.
                 */
                t = (uintptr_t)src;     /* only need low bits */
                if ((t | (uintptr_t)dst) & (sizeof(word) - 1)) {
                        /*
                         * Try to align operands. This cannot be done
                         * unless the low bits match.
                         */
                        if ((t ^ (uintptr_t)dst) & (sizeof(word) - 1) || (length < sizeof(word))) {
                                t = length;
                        } else {
                                t = sizeof(word) - (t & (sizeof(word) - 1));
                        }
                        length -= t;
                        TLOOP1(*dst++ = *src++);
                }
                /*
                 * Copy whole words, then mop up any trailing bytes.
                 */
                t = length / sizeof(word);
                TLOOP(*(word *)dst = *(word *)src; src += sizeof(word); dst += sizeof(word));
                t = length & (sizeof(word) - 1);
                TLOOP(*dst++ = *src++);
        } else {
                /*
                 * Copy backwards. Otherwise essentially the same.
                 * Alignment works as before, except that it takes
                 * (t&(sizeof(word) - 1)) bytes to align, not sizeof(word)-(t&(sizeof(word) - 1)).
                 */
                src += length;
                dst += length;
                t = (uintptr_t)src;
                if ((t | (uintptr_t)dst) & (sizeof(word) - 1)) {
                        if ((t ^ (uintptr_t)dst) & (sizeof(word) - 1) || (length <= sizeof(word))) {
                                t = length;
                        } else {
                                t &= (sizeof(word) - 1);
                        }
                        length -= t;
                        TLOOP1(*--dst = *--src);
                }
                t = length / sizeof(word);
                TLOOP(src -= sizeof(word); dst -= sizeof(word); *(word *)dst = *(word *)src);
                t = length & (sizeof(word) - 1);
                TLOOP(*--dst = *--src);
        }

        return dst0;
}
