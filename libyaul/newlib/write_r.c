#include <sys/reent.h>

int
_write_r(struct _reent *r __attribute__ ((unused)),
    int file __attribute__ ((unused)),
    char *ptr __attribute__ ((unused)),
    int len __attribute__ ((unused)))
{
        return len;
}
