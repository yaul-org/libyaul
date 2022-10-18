#include <assert.h>
#include <stdint.h>
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

void __weak operator delete[](void* ptr __unused, unsigned int) {
    /* Not yet implemented */
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

static void __section(".ctor") __used
_global_ctors_call(void)
{
    extern void (*__CTOR_LIST__[])(void);

    /* Constructors are called in reverse order of the list */
    for (int32_t i = (int32_t)__CTOR_LIST__[0]; i >= 1; i--) {
        /* Each function handles one or more destructor (within file
         * scope) */
        __CTOR_LIST__[i]();
    }
}

static void __section(".dtor") __used
_global_dtors_call(void)
{
    extern void (*__DTOR_LIST__[])(void);

    /* Destructors in forward order */
    for (int32_t i = 0; i < (int32_t)__DTOR_LIST__[0]; i++) {
        /* Each function handles one or more destructor (within file
         * scope) */
        __DTOR_LIST__[i + 1]();
    }
}
