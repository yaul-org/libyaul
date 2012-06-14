#include <inttypes.h>
#include <stddef.h>

#include <tlsf/tlsf.h>

tlsf_pool heap = NULL;

void *
malloc(size_t n)
{
        extern void *_stack_end;
        extern void *_end;

        if (heap == NULL) {
                /* Create a heap at the end of the 'SBSS' section */
                heap = tlsf_create((void *)&_stack_end,
                    (uint32_t)&_stack_end - (uint32_t)&_end);
        }

        return tlsf_malloc(heap, n);
}
