/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the University of
 *    California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
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

#include <ctype.h>

#define _CTYPE_LOOKUP(_c) ((_ctype_bitmap + 1)[(int)(_c) & 0xFF])

#define _U (0x01)
#define _L (0x02)
#define _N (0x04)
#define _S (0x08)
#define _P (0x10)
#define _C (0x20)
#define _X (0x40)
#define _B (0x80)

static const uint8_t _ctype_bitmap[1 + 256] __aligned(4) = {
          0,
         _C,      _C,      _C,      _C,      _C,      _C,      _C, _C,
         _C, _C | _S, _C | _S, _C | _S, _C | _S, _C | _S,      _C, _C,
         _C,      _C,      _C,      _C,      _C,      _C,      _C, _C,
         _C,      _C,      _C,      _C,      _C,      _C,      _C, _C,
    _S | _B,      _P,      _P,      _P,      _P,      _P,      _P, _P,
         _P,      _P,      _P,      _P,      _P,      _P,      _P, _P,
         _N,      _N,      _N,      _N,      _N,      _N,      _N, _N,
         _N,      _N,      _P,      _P,      _P,      _P,      _P, _P,
         _P, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U | _X, _U,
         _U,      _U,      _U,      _U,      _U,      _U,      _U, _U,
         _U,      _U,      _U,      _U,      _U,      _U,      _U, _U,
         _U,      _U,      _U,      _P,      _P,      _P,      _P, _P,
         _P, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L | _X, _L,
         _L,      _L,      _L,      _L,      _L,      _L,      _L, _L,
         _L,      _L,      _L,      _L,      _L,      _L,      _L, _L,
         _L,      _L,      _L,      _P,      _P,      _P,      _P, _C,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0,
          0,       0,       0,       0,       0,       0,       0,  0
};

int
isalnum(int c)
{
    return ((_CTYPE_LOOKUP(c) & (_U | _L | _N)) != 0);
}

int
isalpha(int c)
{
    return ((_CTYPE_LOOKUP(c) & (_U | _L)) != 0);
}

int
isascii(int c)
{
    return (((uint32_t)c) <= 0x7F);
}

int
isblank(int c)
{
    return ((_CTYPE_LOOKUP(c) & _B) == _B) || (c == '\t');
}

int
iscntrl(int c)
{
    return ((_CTYPE_LOOKUP(c) & _C) == _C);
}

int
isdigit(int c)
{
    return ((_CTYPE_LOOKUP(c) & _N) == _N);
}

int
isgraph(int c)
{
    return ((_CTYPE_LOOKUP(c) & (_P | _U | _L | _N)) != 0);
}

int
islower(int c)
{
    return ((_CTYPE_LOOKUP(c) & (_U | _L)) == _L);
}

int
isprint(int c)
{
    return ((_CTYPE_LOOKUP(c) & (_P | _U | _L | _N | _B)) != 0);
}

int
ispunct(int c)
{
    return ((_CTYPE_LOOKUP(c) & _P) == _P);
}

int
isspace(int c)
{
    return ((_CTYPE_LOOKUP(c) & _S) == _S);
}

int
isupper(int c)
{
    return ((_CTYPE_LOOKUP(c) & (_U | _L)) == _U);
}

int
isxdigit(int c)
{
    return ((_CTYPE_LOOKUP(c) & (_X | _N)) != 0);
}

int
tolower(int c)
{
    if (isupper(c)) {
        return (c | _C);
    }

    return c;
}

int
toupper(int c)
{
    if (islower(c)) {
        return (c & 0x5F);
    }

    return c;
}
