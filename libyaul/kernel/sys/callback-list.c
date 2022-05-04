/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <sys/callback-list.h>

#include "callback-list-internal.h"

static void _default_handler(void *work);

callback_list_t *
callback_list_alloc(uint32_t count)
{
        return __callback_list_request_alloc(count, malloc);
}

void
callback_list_free(callback_list_t *callback_list)
{
        __callback_list_request_free(callback_list, free);
}

void
callback_list_init(callback_list_t *callback_list, callback_t *callbacks,
    uint32_t count)
{
        assert(callback_list != NULL);
        assert(callbacks != NULL);
        assert(count > 0);

        callback_list->count = count;
        callback_list->callbacks = callbacks;

        callback_list_clear(callback_list);
}

void
callback_list_process(callback_list_t *callback_list)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);

        callback_t *callback = callback_list->callbacks;
        const callback_t * const last_callback =
            &callback_list->callbacks[callback_list->count - 1];

        for (; callback <= last_callback; callback++) {
                callback_call(callback);
        }
}

void
callback_list_rev_process(callback_list_t *callback_list)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);

        callback_t *callback =
            &callback_list->callbacks[callback_list->count - 1];
        const callback_t * const first_callback =
            callback_list->callbacks;

        for (; callback >= first_callback; callback--) {
                callback_call(callback);
        }
}

callback_id_t
callback_list_callback_add(callback_list_t *callback_list,
    callback_handler_t handler, void *work)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);

        uint32_t id;
        for (id = 0; id < callback_list->count; id++) {
                if (callback_list->callbacks[id].handler != _default_handler) {
                        continue;
                }

                callback_set(&callback_list->callbacks[id], handler, work);

                return id;
        }

        assert(id != callback_list->count);

        return -1;
}

void
callback_list_callback_remove(callback_list_t *callback_list, callback_id_t id)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);
        assert((uint32_t)id < callback_list->count);

        callback_list->callbacks[id].handler = _default_handler;
        callback_list->callbacks[id].work = NULL;
}

void
callback_list_clear(callback_list_t *callback_list)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);

        for (uint32_t i = 0; i < callback_list->count; i++) {
                callback_init(&callback_list->callbacks[i]);
        }
}

void
callback_init(callback_t *callback)
{
        assert(callback != NULL);

        callback->handler = _default_handler;
        callback->work = NULL;
}

void
callback_set(callback_t *callback, callback_handler_t handler, void *work)
{
        assert(callback != NULL);

        callback->handler = (handler != NULL) ? handler : _default_handler;
        callback->work = work;
}

static void
_default_handler(void *work __unused)
{
}
