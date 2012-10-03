#include <sys/reent.h>

#include <stdlib.h>

void *
_malloc_r(struct _reent *r __attribute__ ((unused)), size_t n)
{
        return malloc(n);
}
