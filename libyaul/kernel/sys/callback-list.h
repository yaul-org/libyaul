/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_SYS_CALLBACK_LIST_H_
#define _YAUL_KERNEL_SYS_CALLBACK_LIST_H_

#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

typedef uint8_t callback_id_t;

typedef void (*callback_handler)(void *);

typedef struct callback {
        callback_handler handler;
        void *work;
} __packed callback_t;

typedef struct callback_list {
        callback_t *callbacks;
        uint8_t count;
} callback_list_t;

static inline void __always_inline
callback_call(callback_t *callback)
{
        assert(callback != NULL);
        assert(callback->handler != NULL);

        callback->handler(callback->work);
}

extern callback_list_t *callback_list_alloc(uint8_t);
extern void callback_list_free(callback_list_t *);
extern void callback_list_init(callback_list_t *, callback_t *, uint8_t);

extern void callback_list_process(callback_list_t *, bool);

extern callback_id_t callback_list_callback_add(callback_list_t *, callback_handler, void *);
extern void callback_list_callback_remove(callback_list_t *, callback_id_t);
extern void callback_list_clear(callback_list_t *);

extern void callback_init(callback_t *);
extern void callback_set(callback_t *, callback_handler, void *);

__END_DECLS

#endif /* !_YAUL_KERNEL_SYS_CALLBACK_LIST_H_ */
