#include <sys/reent.h>

#include <common.h>

int
_execve_r(struct _reent *r __unused, const char *path __unused,
    char *const argv[] __unused, char *const envp[] __unused)
{
        return 0;
}
