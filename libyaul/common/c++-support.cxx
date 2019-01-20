#include <assert.h>
#include <stdlib.h>

extern "C"
void __cxa_pure_virtual(void) {
    /* Pure C++ virtual call; abort! */
    assert(false);
}

void* __weak operator new(size_t size) {
    return malloc(size);
}

void* __weak operator new[](size_t size) {
    return malloc(size);
}

void __weak operator delete(void *ptr) {
    free(ptr);
}

void __weak operator delete[](void *ptr) {
    free(ptr);
}
