#include <sys/reent.h>

int
_fork_r(struct _reent *r __attribute__ ((unused)))
{
        return 0;
}
