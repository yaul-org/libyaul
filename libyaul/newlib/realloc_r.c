#include <sys/reent.h>
#include <stddef.h>

#include <common.h>

void *
_realloc_r(struct _reent *r __unused, void *old __unused,
    size_t new_len __unused)
{
}
