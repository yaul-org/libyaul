#include <sys/reent.h>

#include <common.h>

int
_utime_r(struct _reent *r __unused, const char *path __unused,
    char *times __unused)
{
        return 0;
}
