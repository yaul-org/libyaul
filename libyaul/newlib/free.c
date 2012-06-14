#include <inttypes.h>
#include <stddef.h>

#include <tlsf/tlsf.h>

void
free(void *addr)
{
        extern tlsf_pool heap;

        if (heap == NULL)
                return;
        tlsf_free(heap, addr);
}
