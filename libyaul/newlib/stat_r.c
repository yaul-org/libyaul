#include <sys/stat.h>
#include <sys/reent.h>

#include <common.h>

int
_stat_r(struct _reent *r __unused, const char *path __unused,
    struct stat *st __unused)
{
        return 0;
}
