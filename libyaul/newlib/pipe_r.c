#include <sys/reent.h>

int
_pipe_r(struct _reent *r __attribute__ ((unused)),
    int *fd __attribute__ ((unused)))
{
        return 0;
}
