/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_SYS_CALLBACK_LIST_H_
#define _YAUL_KERNEL_SYS_CALLBACK_LIST_H_

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

typedef int32_t callback_id_t;

typedef void (*callback_handler_t)(void *work);

typedef struct callback {
        callback_handler_t handler;
        void *work;
} callback_t;

typedef struct callback_list {
        callback_t *callbacks;
        uint32_t count;
} callback_list_t;

static inline void __always_inline
callback_call(const callback_t *callback)
{
        assert(callback != NULL);
        assert(callback->handler != NULL);

        callback->handler(callback->work);
}

extern callback_list_t *callback_list_alloc(uint32_t count);
extern void callback_list_free(callback_list_t *callback_list);
extern void callback_list_init(callback_list_t *callback_list,
    callback_t *callbacks, uint32_t count);

extern void callback_list_process(callback_list_t *callback_list);
extern void callback_list_rev_process(callback_list_t *callback_list);

extern callback_id_t callback_list_callback_add(callback_list_t *callback_list,
    callback_handler_t handler, void *work);
extern void callback_list_callback_remove(callback_list_t *callback_list,
    callback_id_t id);
extern void callback_list_clear(callback_list_t *callback_list);

extern void callback_init(callback_t *callback);
extern void callback_set(callback_t *callback, callback_handler_t handler,
    void *work);

__END_DECLS

#endif /* !_YAUL_KERNEL_SYS_CALLBACK_LIST_H_ */
