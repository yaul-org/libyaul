#include <inttypes.h>
#include <stddef.h>

#include <tlsf/tlsf.h>

tlsf_pool heap = NULL;

void *
malloc(size_t n)
{
        extern void *_end;
        extern void *_text_start;

        uint32_t ram_end;

        if (heap == NULL) {
                ram_end = 0x000FBFFF + (uint32_t)&_text_start;

                /* Create a heap at the end of the 'SBSS' section */
                heap = tlsf_create((void *)&_end, ram_end - (uint32_t)&_end);
        }

        return tlsf_malloc(heap, n);
}
