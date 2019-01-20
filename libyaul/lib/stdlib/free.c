#include <sys/cdefs.h>

#include <mm/slob.h>

void __weak
free(void *addr)
{
        slob_free(addr);
}
