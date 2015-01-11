#include <sys/reent.h>

#include <common.h>

int
_open_r(struct _reent *r __unused, const char *path __unused,
    int flags __unused)
{
        return 0;
}
