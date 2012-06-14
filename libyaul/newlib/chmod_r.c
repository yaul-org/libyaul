#include <sys/reent.h>

int
_chmod_r(struct _reent *r __attribute__ ((unused)),
    const char *path __attribute__ ((unused)),
    short mode __attribute__ ((unused)))
{
        return 0;
}
