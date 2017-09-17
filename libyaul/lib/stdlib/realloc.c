#include <stdint.h>
#include <stddef.h>

#include <mm/slob.h>

void *
realloc(void *old, size_t new_len)
{
        void *ret;
        ret = slob_realloc(old, new_len);

        return ret;
}
