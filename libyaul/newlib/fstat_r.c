#include <sys/stat.h>
#include <sys/reent.h>

#include <common.h>

int
_fstat_r(struct _reent *r __unused, int file __unused, struct stat *st __unused)
{
        return 0;
}
