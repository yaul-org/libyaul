/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _KERNEL_SYS_CALLBACK_LIST_INTERNAL_H_
#define _KERNEL_SYS_CALLBACK_LIST_INTERNAL_H_

#include <sys/callback-list.h>

#include <internal.h>

__BEGIN_DECLS

extern callback_list_t *__callback_list_alloc(uint32_t count);
extern void __callback_list_free(callback_list_t *callback_list);

extern callback_list_t *__callback_list_request_alloc(uint32_t count,
    malloc_func_t malloc_func);
extern void __callback_list_request_free(callback_list_t *callback_list,
    free_func_t free_func);

__END_DECLS

#endif /* !_KERNEL_SYS_CALLBACK_LIST_INTERNAL_H_ */
