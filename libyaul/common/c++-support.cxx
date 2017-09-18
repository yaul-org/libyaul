#include <stdlib.h>

extern "C"
void __cxa_pure_virtual(void) {
        /* Pure C++ virtual call; abort! */
        abort();
}

void* operator new(size_t size __unused) {
    return (void*)0;
}

void* operator new[](size_t size __unused) {
    return (void*)0;
}

void operator delete(void *ptr __unused) {
}

void operator delete[](void *ptr __unused) {
}
