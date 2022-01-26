#include <errno.h>
#include <stddef.h>
#include <string.h>

static const struct errmsgstr_t {
#define E(n, s) char str##n[sizeof(s)];
#include "strerror.h"
#undef E
} _errmsgstr = {
#define E(n, s) s,
#include "strerror.h"
#undef E
};

static const unsigned short _errmsgidx[] = {
#define E(n, s) [n] = offsetof(struct errmsgstr_t, str##n),
#include "strerror.h"
#undef E
};

char *
strerror(int e)
{
        return ((char *)&_errmsgstr + _errmsgidx[e]);
}
