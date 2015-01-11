#include <sys/reent.h>

#include <common.h>

int
_lseek_r(struct _reent *r __unused, int file __unused, int ptr __unused,
    int dir __unused)
{
        return 0;
}
