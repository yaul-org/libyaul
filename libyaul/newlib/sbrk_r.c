#include <sys/types.h>
#include <sys/reent.h>

caddr_t
_sbrk_r(struct _reent *r __attribute__ ((unused)),
    int incr __attribute__ ((unused)))
{
        return 0;
}
