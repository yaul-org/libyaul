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

void __weak operator delete(void* ptr) {
    free(ptr);
}

void __weak operator delete[](void* ptr) {
    free(ptr);
}

/*-
 * <https://en.cppreference.com/w/cpp/memory/new/operator_delete>
 *
 * Called if a user-defined replacement is provided, except that it's
 * unspecified whether other overloads or this overload is called when deleting
 * objects of incomplete type and arrays of non-class and trivially-destructible
 * class types.
 *
 * A memory allocator can use the given size to be more efficient */
void __weak operator delete(void* ptr, unsigned int) {
    free(ptr);
}
