#include <sys/reent.h>

#include <common.h>

int
_wait_r(struct _reent *r __unused, int *statusp __unused)
{
        return 0;
}
