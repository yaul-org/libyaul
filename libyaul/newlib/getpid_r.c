#include <sys/reent.h>

#include <common.h>

int
_getpid_r(struct _reent *r __unused, int n __unused)
{
        return 1;
}
