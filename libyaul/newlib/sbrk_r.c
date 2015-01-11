#include <sys/types.h>
#include <sys/reent.h>

#include <common.h>

caddr_t
_sbrk_r(struct _reent *r __unused, int incr __unused)
{
        return 0;
}
