#include <stdlib.h>

#include <sys/reent.h>

#include <common.h>

void *
_malloc_r(struct _reent *r __unused, size_t n)
{
        return malloc(n);
}
