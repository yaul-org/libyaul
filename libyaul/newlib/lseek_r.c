#include <sys/reent.h>

int
_lseek_r(struct _reent *r __attribute__ ((unused)),
    int file __attribute__ ((unused)),
    int ptr __attribute__ ((unused)),
    int dir __attribute__ ((unused)))
{
        return 0;
}
