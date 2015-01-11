#include <sys/reent.h>

#include <common.h>

int
_pipe_r(struct _reent *r __unused, int *fd __unused)
{
        return 0;
}
