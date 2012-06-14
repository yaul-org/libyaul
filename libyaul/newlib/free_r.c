#include <stdlib.h>
#include <sys/reent.h>

void
_free_r(struct _reent *r __attribute__ ((unused)),
    void *addr)
{
        free(addr);
}
