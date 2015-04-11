#include <stddef.h>

#include <sys/reent.h>

#include <common.h>

void *
_realloc_r(struct _reent *r __unused, void *old,
    size_t new_len)
{
        return realloc(old, new_len);
}
