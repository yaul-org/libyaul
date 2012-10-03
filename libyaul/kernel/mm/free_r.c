#include <sys/reent.h>

#include <stdlib.h>

void
_free_r(struct _reent *r __attribute__ ((unused)), void *addr)
{
        free(addr);
}
