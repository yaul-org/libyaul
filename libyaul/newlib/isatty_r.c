#include <sys/reent.h>

#include <common.h>

int
_isatty_r(struct _reent *r __unused, int fd __unused)
{
        return -1;
}
