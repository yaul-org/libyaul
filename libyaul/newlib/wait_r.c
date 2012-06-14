#include <sys/reent.h>

int
_wait_r(struct _reent *r __attribute__ ((unused)),
    int *statusp __attribute__ ((unused)))
{
        return 0;
}
