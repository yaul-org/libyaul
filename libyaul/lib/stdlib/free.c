#include <mm/slob.h>

void __attribute__ ((weak, alias("free")))
override_free(void *addr)
{
        slob_free(addr);
}

void
free(void *addr)
{
        override_free(addr);
}
