#include <sys/reent.h>

#include <common.h>

int
_creat_r(struct _reent *r __unused, const char *path __unused,
    int mode __unused)
{
        return 0;
}
