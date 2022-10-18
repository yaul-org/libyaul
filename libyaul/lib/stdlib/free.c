#include <sys/cdefs.h>

#include <internal.h>

void __weak
free(void *addr)
{
        void __user_free(void *addr);

        __user_free(addr);
}
