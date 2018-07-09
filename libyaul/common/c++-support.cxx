#include <assert.h>
#include <stdlib.h>

extern "C"
void __cxa_pure_virtual(void) {
        /* Pure C++ virtual call; abort! */
    assert("Pure C++ virtual call");
}

void* __attribute__ ((weak)) operator new(size_t size) {
    return malloc(size);
}

void* __attribute__ ((weak)) operator new[](size_t size) {
    return malloc(size);
}

void __attribute__ ((weak)) operator delete(void *ptr) {
    free(ptr);
}

void __attribute__ ((weak)) operator delete[](void *ptr) {
    free(ptr);
}
