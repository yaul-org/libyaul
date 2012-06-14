#include <sys/reent.h>

int
_chown(struct _reent *r __attribute__ ((unused)),
    const char *path __attribute__ ((unused)),
    short owner __attribute__ ((unused)),
    short group __attribute__ ((unused)))
{
        return 0;
}
