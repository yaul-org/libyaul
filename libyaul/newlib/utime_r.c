#include <sys/reent.h>

int
_utime_r(struct _reent *r __attribute__ ((unused)),
    const char *path __attribute__ ((unused)),
    char *times __attribute__ ((unused)))
{
        return 0;
}
