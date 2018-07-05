/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <cpu/registers.h>

#include "stack.h"

static char _buffer[256];

/*
 * Return the previous stack frame from the frame pointer FPTR
 */
#define FPTR_NEXT_GET(fptr)             (*((volatile uint32_t *)(((fptr) + 4))))

/*
 * Return the return address (the caller) of the frame pointer FPTR
 */
#define FPTR_RET_ADDRESS_GET(fptr)      (*((volatile uint32_t *)(fptr)))

char *
stack_backtrace(void)
{
        extern void *_text_start;
        extern void *_text_end;

        /* Return the return address (the caller) of the current
         * function */
        register uintptr_t pr;

        /* Return the current stack frame of the current
         * function. Remember that GCC must not omit the frame
         * pointer! */
        register uintptr_t fp;

        /* Obtain address of the caller and its frame pointer before it
         * is clobbered by any subsequent calls */
        pr = cpu_reg_pr_get();
        fp = cpu_reg_fp_get();

        int32_t level;
        level = 0;

        *_buffer = '\0';

        do {
                (void)snprintf(_buffer, 256, "%s #%li 0x%08X in ??? ()\n",
                    _buffer, level, (uintptr_t)pr);

                pr = FPTR_RET_ADDRESS_GET(fp);
                fp = FPTR_NEXT_GET(fp);

                level++;
        } while (((pr >= (uint32_t)&_text_start)) && (pr < ((uint32_t)&_text_end)));

        return _buffer;
}
