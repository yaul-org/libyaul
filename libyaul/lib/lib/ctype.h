#ifndef _CTYPE_H_
#define _CTYPE_H_

#include "_ansi.h"

#include <sys/cdefs.h>

_BEGIN_STD_C

int isalnum(int);
int isalpha(int);
int isblank(int);
int iscntrl(int);
int isdigit(int);
int isgraph(int);
int islower(int);
int isprint(int);
int ispunct(int);
int isspace(int);
int isupper(int);
int isxdigit(int);

#define _U 0x01
#define _L 0x02
#define _N 0x04
#define _S 0x08
#define _P 0x10
#define _C 0x20
#define _X 0x40
#define _B 0x80

#define _ctype_lookup(_c) ((_ctype_bitmap + 1)[(int)(_c) & 0xFFl])

#define isalnum(_c)     (_ctype_lookup(_c) & (_U | _L | _N))
#define isalpha(_c)     (_ctype_lookup(_c) & (_U | _L))
#define isascii(_c)     ((unsigned)(_c) <= 0x7F)
#define isblank(_c)     ((_ctype_lookup(_c) & _B) || (c == '\t'))
#define iscntrl(_c)     (_ctype_lookup(_c) & _C)
#define isdigit(_c)     (_ctype_lookup(_c) & _N)
#define isgraph(_c)     (_ctype_lookup(_c) & (_P | _U | _L | _N))
#define islower(_c)     ((_ctype_lookup(_c) & (_U | _L)) == _L)
#define isprint(_c)     (_ctype_lookup(_c) & (_P | _U | _L | _N | _B))
#define ispunct(_c)     (_ctype_lookup(_c) & _P)
#define isspace(_c)     (_ctype_lookup(_c) & _S)
#define isupper(_c)     ((_ctype_lookup(_c) & (_U | _L)) == _U)
#define isxdigit(_c)    (_ctype_lookup(_c) & (_X | _N))

extern const char _ctype_bitmap[];

_END_STD_C

#endif /* _CTYPE_H_ */
