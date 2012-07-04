/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <cons/vdp2.h>

#include "stack.h"

char *
stack_backtrace(void)
{
        extern void *_text_start;
        extern void *_text_end;

        static char buf[1024];

        uintptr_t fp;
        uintptr_t pr;

        int level;

        /* Obtain address of the caller and its frame pointer before it
         * is clobbered by any subsequent calls */
        STACK_RET_ADDRESS(pr);
        STACK_FPTR(fp);

        level = 0;
        *buf = '\0';
        do {
                (void)sprintf(buf, "%s #%i 0x%08X in ??? ()\n",
                    buf, level, (uintptr_t)pr);

                pr = STACK_FPTR_RET_ADDRESS_GET(fp);
                fp = STACK_FPTR_NEXT_GET(fp);

                level++;
        } while (((pr >= (uint32_t)&_text_start)) && (pr < ((uint32_t)&_text_end)));

        return buf;
}
