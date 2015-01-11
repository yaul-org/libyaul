#include <sys/reent.h>

#include <common.h>

int
_chown(struct _reent *r __unused, const char *path __unused,
    short owner __unused, short group __unused)
{
        return 0;
}
