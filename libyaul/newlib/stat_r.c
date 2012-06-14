#include <sys/stat.h>
#include <sys/reent.h>

int
_stat_r(struct _reent *r __attribute__ ((unused)),
    const char *path __attribute__ ((unused)),
    struct stat *st __attribute__ ((unused)))
{
        return 0;
}
