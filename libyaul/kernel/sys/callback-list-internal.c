/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include "callback-list-internal.h"

callback_list_t *
__callback_list_alloc(uint32_t count)
{
        return __callback_list_request_alloc(count, __malloc);
}

void
__callback_list_free(callback_list_t *callback_list)
{
        __callback_list_request_free(callback_list, __free);
}

callback_list_t *
__callback_list_request_alloc(uint32_t count, malloc_func_t malloc_func)
{
        assert(count > 0);

        callback_list_t *callback_list;
        callback_list = malloc_func(sizeof(callback_list_t));
        assert(callback_list != NULL);

        callback_t *callbacks;
        callbacks = malloc_func(count * sizeof(callback_t));
        assert(callbacks != NULL);

        callback_list_init(callback_list, callbacks, count);
        callback_list_clear(callback_list);

        return callback_list;
}

void
__callback_list_request_free(callback_list_t *callback_list, free_func_t free_func)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);

        callback_list->count = 0;

        free_func(callback_list->callbacks);
        free_func(callback_list);
}
