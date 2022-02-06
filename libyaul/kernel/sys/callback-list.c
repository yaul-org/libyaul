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

#include <internal.h>

static void _default_handler(void *);

callback_list_t *
callback_list_alloc(uint8_t count)
{
        assert(count > 0);

        callback_list_t *callback_list;
        callback_list = __malloc(sizeof(callback_list_t));
        assert(callback_list != NULL);

        callback_t *callbacks;
        callbacks = __malloc(count * sizeof(callback_t));
        assert(callbacks != NULL);

        callback_list_init(callback_list, callbacks, count);
        callback_list_clear(callback_list);

        return callback_list;
}

void
callback_list_free(callback_list_t *callback_list)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);

        callback_list->count = 0;

        __free(callback_list->callbacks);
        __free(callback_list);
}

void
callback_list_init(callback_list_t *callback_list, callback_t *callbacks, uint8_t count)
{
        assert(callback_list != NULL);
        assert(callbacks != NULL);
        assert(count > 0);

        callback_list->count = count;
        callback_list->callbacks = callbacks;
}

void
callback_list_process(callback_list_t *callback_list, bool clear)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);

        uint8_t id;
        for (id = 0; id < callback_list->count; id++) {
                callback_handler_t handler;
                handler = callback_list->callbacks[id].handler;

                void *work;
                work = callback_list->callbacks[id].work;

                if (clear) {
                        callback_init(&callback_list->callbacks[id]);
                }

                handler(work);
        }
}

callback_id_t
callback_list_callback_add(callback_list_t *callback_list, callback_handler_t handler, void *work)
{
#ifdef DEBUG
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);
#endif /* DEBUG */

        callback_id_t id;
        for (id = 0; id < callback_list->count; id++) {
                if (callback_list->callbacks[id].handler != _default_handler) {
                        continue;
                }

                callback_set(&callback_list->callbacks[id], handler, work);

                return id;
        }

#ifdef DEBUG
        assert(id != callback_list->count);
#endif /* DEBUG */

        return -1;
}

void
callback_list_callback_remove(callback_list_t *callback_list, callback_id_t id)
{
#ifdef DEBUG
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);
        assert(id < callback_list->count);
#endif /* DEBUG */

        callback_list->callbacks[id].handler = _default_handler;
        callback_list->callbacks[id].work = NULL;
}

void
callback_list_clear(callback_list_t *callback_list)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);

        uint8_t i;
        for (i = 0; i < callback_list->count; i++) {
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
