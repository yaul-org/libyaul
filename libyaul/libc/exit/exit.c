/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdlib.h>

#include <sys/cdefs.h>

void __noreturn
exit(int code __unused)
{
    extern void __atexit_call(void);

    __atexit_call();

    abort();
}
