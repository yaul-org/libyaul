#include <sys/reent.h>

#include <common.h>

int
_kill_r(struct _reent *r __unused, int n __unused, int m __unused)
{
        return 0;
}
