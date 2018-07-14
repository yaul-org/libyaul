#include <mm/slob.h>

void __attribute__ ((weak))
free(void *addr)
{
        slob_free(addr);
}
