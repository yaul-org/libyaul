#include <stdlib.h>

extern "C"
void
__cxa_pure_virtual(void)
{
    /* Pure C++ virtual call; abort! */
    abort();
}

extern "C"
int
__cxa_guard_acquire(int *gv __unused)
{
        return 1;
}

extern "C"
void
__cxa_guard_release(int *gv __unused)
{
}

extern "C"
void
__cxa_guard_abort(int *gv __unused)
{
}
