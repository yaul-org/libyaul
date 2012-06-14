#include <sys/reent.h>

int
_execve_r(struct _reent *r __attribute__ ((unused)),
    const char *path __attribute__ ((unused)),
    char *const argv[] __attribute__ ((unused)),
    char *const envp[] __attribute__ ((unused)))
{
        return 0;
}
