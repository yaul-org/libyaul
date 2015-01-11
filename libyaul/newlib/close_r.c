#include <sys/reent.h>

#include <common.h>

int
_close_r(struct _reent *r __unused, int file __unused)
{
        return 0;
}
