/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include <sys/cdefs.h>

#include <sys/callback-list.h>

static callback_t _callbacks[ATEXIT_MAX];

static callback_list_t _callback_list;

static void
_callback_wrapper_call(void *work)
{
        /* First cast to uintptr_t, then to a function pointer so that the
         * pedantic warning is supressed: "ISO C forbids conversion of object
         * pointer to function pointer type" */
        ((void (*)(void))(uintptr_t)work)();
}

int
atexit(void (*function)(void))
{
        if (function == NULL) {
                return -1;
        }

        callback_list_callback_add(&_callback_list, _callback_wrapper_call, (void *)(uintptr_t)function);

        return 0;
}

void
__atexit_init(void)
{
        callback_list_init(&_callback_list, _callbacks, ATEXIT_MAX);
}

void
__atexit_call(void)
{
        callback_list_rev_process(&_callback_list);
}
