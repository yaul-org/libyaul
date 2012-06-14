#include <sys/reent.h>

int
_isatty_r(struct _reent *r __attribute__ ((unused)),
    int fd __attribute__ ((unused)))
{
        return -1;
}
