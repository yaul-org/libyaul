#include <sys/cdefs.h>

int __weak
remove(const char *path __unused)
{
        return 0;
}
