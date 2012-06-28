/*
 * Copyright (c) 2001 Dan Potter
 * See LICENSE for details.
 *
 * Dan Potter
 */

#ifndef _STACK_H_
#define _STACK_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * Return the return address (the caller) of the current function
 */
#define STACK_RET_ADDRESS(pr) do {                                            \
        __asm__ __volatile__ ("STS pr,%0\n"                                   \
                : "=&z" (pr)                                                  \
                : /* No inputs */                                             \
                : "memory");                                                  \
} while (false)

/*
 * Return the current stack frame of the current function. Remember that
 * GCC must not omit the frame pointer!
 */
#define STACK_FPTR(fp) do {                                                   \
        __asm__ __volatile__ ("MOV r14,%0\n"                                  \
                : "=&z" (fp)                                                  \
                : /* No inputs */                                             \
                : "memory");                                                  \
} while (false)

/*
 * Return the previous stack frame from the frame pointer FPTR
 */
#define STACK_FPTR_NEXT_GET(fptr) (*((volatile uint32_t *)(((fptr) + 4))))

/*
 * Return the return address (the caller) of the frame pointer FPTR
 */
#define STACK_FPTR_RET_ADDRESS_GET(fptr) (*((volatile uint32_t *)(fptr)))

char *stack_backtrace(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_STACK_H_ */
