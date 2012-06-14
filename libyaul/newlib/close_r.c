#include <sys/reent.h>

int
_close_r(struct _reent *r __attribute__ ((unused)),
    int file __attribute__((unused)))
{
        return 0;
}
