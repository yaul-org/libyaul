#include <sys/reent.h>
#include <stddef.h>

#include <tlsf/tlsf.h>

void *
_realloc_r(struct _reent *r __attribute__ ((unused)),
    void *old __attribute__ ((unused)),
    size_t new_len __attribute__ ((unused)))
{
        extern tlsf_pool heap;

        return tlsf_realloc(heap, old, new_len);
}
