#include <sys/reent.h>

int
_creat_r(struct _reent *r __attribute__ ((unused)),
    const char *path __attribute__ ((unused)),
    int mode __attribute__ ((unused)))
{
        return 0;
}
