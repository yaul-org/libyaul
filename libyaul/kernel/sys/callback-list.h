/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CALLBACK_LIST_H_
#define _CALLBACK_LIST_H_

#include <stdint.h>
#include <stdbool.h>

__BEGIN_DECLS

struct callback {
        void (*callback)(void *);
        void *work;
} __packed;

struct callback_list {
        struct callback *callbacks;
        uint8_t count;
};

static inline void __always_inline
callback_call(struct callback *callback)
{
        assert(callback != NULL);
        assert(callback->callback != NULL);

        callback->callback(callback->work);
}

extern struct callback_list *callback_list_alloc(const uint8_t);
extern void callback_list_free(struct callback_list *);
extern void callback_list_init(struct callback_list *, struct callback *, const uint8_t);

extern void callback_list_process(struct callback_list *, bool);

extern uint8_t callback_list_callback_add(struct callback_list *, void (*)(void *), void *);
extern void callback_list_callback_remove(struct callback_list *, const uint8_t);
extern void callback_list_clear(struct callback_list *);

extern void callback_init(struct callback *);
extern void callback_set(struct callback *, void (*)(void *), void *);

__END_DECLS

#endif /* !_CALLBACK_LIST_H_ */
