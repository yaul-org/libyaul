#include <sys/reent.h>

int
_unlink_r(struct _reent *r __attribute__ ((unused)))
{
        return -1;
}
