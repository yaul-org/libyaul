#include <sys/reent.h>

#include <common.h>

int
_fork_r(struct _reent *r __unused)
{
        return 0;
}
