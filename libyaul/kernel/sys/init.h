/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_SYS_INIT_H_
#define _YAUL_KERNEL_SYS_INIT_H_

#include <sys/cdefs.h>

__BEGIN_DECLS

/* This prototype declaration is needed (only) for C++ to void name mangling. We
 * also don't want to explicitly declare the prototype for C either (hence the
 * redundant extern "C") */
#ifdef __cplusplus
extern "C" void user_init(void);
#endif /* __cplusplus */

__END_DECLS

#endif /* !_YAUL_KERNEL_SYS_INIT_H_ */
