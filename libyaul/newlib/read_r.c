#include <sys/reent.h>

#include <common.h>

int
_read_r(struct _reent *r __unused, int file __unused, char *ptr __unused,
    int len __unused)
{
        return 0;
}
