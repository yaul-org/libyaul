#include <sys/cdefs.h>

char * __weak
tmpnam(char *buf __unused)
{
        return 0;
}
