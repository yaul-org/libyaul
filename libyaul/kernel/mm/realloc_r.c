#include <sys/reent.h>
#include <stddef.h>

void *
_realloc_r(struct _reent *r __attribute__ ((unused)),
    void *old __attribute__ ((unused)),
    size_t new_len __attribute__ ((unused)))
{
        return NULL;
}
