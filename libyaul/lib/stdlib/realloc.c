#include <sys/cdefs.h>

#include <internal.h>

void * __weak
realloc(void *old, size_t new_len)
{
        extern void *_internal_user_realloc(void *old, size_t new_len);

        return _internal_user_realloc(old, new_len);
}
