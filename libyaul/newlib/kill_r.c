#include <sys/reent.h>

int
_kill_r(struct _reent *r __attribute__ ((unused)),
    int n __attribute__ ((unused)), int m __attribute__ ((unused)))
{
        return 0;
}
