#include "slob.h"

void
free(void *addr)
{
        slob_free(addr);
}
