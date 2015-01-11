#include <sys/reent.h>

#include <common.h>

int
_unlink_r(struct _reent *r __unused)
{
        return -1;
}
