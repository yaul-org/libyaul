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

static void _default_callback(void *);

struct callback_list *
callback_list_alloc(const uint8_t count)
{
        assert(count > 0);

        struct callback_list *callback_list;
        callback_list = _internal_malloc(sizeof(struct callback_list));
        assert(callback_list != NULL);

        struct callback *callbacks;
        callbacks = _internal_malloc(count * sizeof(struct callback));
        assert(callbacks != NULL);

        callback_list_init(callback_list, callbacks, count);
        callback_list_clear(callback_list);

        return callback_list;
}

void
callback_list_free(struct callback_list *callback_list)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);

        callback_list->count = 0;

        _internal_free(callback_list->callbacks);
        _internal_free(callback_list);
}

void
callback_list_init(struct callback_list *callback_list, struct callback *callbacks, const uint8_t count)
{
        assert(callback_list != NULL);
        assert(callbacks != NULL);
        assert(count > 0);

        callback_list->count = count;
        callback_list->callbacks = callbacks;
}

void
callback_list_process(struct callback_list *callback_list, bool clear)
{
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);

        uint8_t id;
        for (id = 0; id < callback_list->count; id++) {
                void (*callback)(void *);
                callback = callback_list->callbacks[id].callback;

                void *work;
                work = callback_list->callbacks[id].work;

                if (clear) {
                        callback_init(&callback_list->callbacks[id]);
                }

                callback(work);
        }
}

uint8_t
callback_list_callback_add(struct callback_list *callback_list, void (*callback)(void *), void *work)
{
#ifdef DEBUG
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);
#endif /* DEBUG */

        uint8_t id;
        for (id = 0; id < callback_list->count; id++) {
                if (callback_list->callbacks[id].callback != _default_callback) {
                        continue;
                }

                callback_set(&callback_list->callbacks[id], callback, work);

                return id;
        }

#ifdef DEBUG
        assert(id != callback_list->count);
#endif /* DEBUG */

        return -1;
}

void
callback_list_callback_remove(struct callback_list *callback_list, const uint8_t id)
{
#ifdef DEBUG
        assert(callback_list != NULL);
        assert(callback_list->callbacks != NULL);
        assert(callback_list->count > 0);
        assert(id < callback_list->count);
#endif /* DEBUG */

        callback_list->callbacks[id].callback = _default_callback;
        callback_list->callbacks[id].work = NULL;
}

void
callback_list_clear(struct callback_list *callback_list)
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
callback_init(struct callback *callback)
{
        assert(callback != NULL);

        callback->callback = _default_callback;
        callback->work = NULL;
}

void
callback_set(struct callback *callback, void (*callback_func)(void *), void *work)
{
        assert(callback != NULL);

        callback->callback = (callback_func != NULL) ? callback_func : _default_callback;
        callback->work = work;
}

static void
_default_callback(void *work __unused)
{
}
