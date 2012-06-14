#include <sys/reent.h>

int
_getpid_r(struct _reent *r __attribute__ ((unused)),
    int n __attribute__ ((unused)))
{
        return 1;
}
