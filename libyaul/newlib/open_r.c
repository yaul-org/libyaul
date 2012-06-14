#include <sys/reent.h>

int
_open_r(struct _reent *r __attribute__ ((unused)),
    const char *path __attribute__ ((unused)),
    int flags __attribute__ ((unused)))
{
        return 0;
}
