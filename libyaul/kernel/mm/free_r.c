#include <stdlib.h>

#include <sys/reent.h>

#include <common.h>

void
_free_r(struct _reent *r __unused, void *addr)
{
        free(addr);
}
