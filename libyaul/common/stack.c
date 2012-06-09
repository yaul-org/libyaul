/*
 * Copyright (c) 2012 Israel Jacques
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu>
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <cons/vdp2.h>

#include "stack.h"

char *
stack_backtrace(void)
{
        extern void start(void);

        static char buf[1024];

        uintptr_t fp;
        uintptr_t pr;
        int call;

        memset(buf, '\0', sizeof(buf));

        call = 0;
        STACK_FPTR(fp);
        do {
                pr = STACK_FPTR_RET_ADDRESS_GET(fp);
                if (pr < (uint32_t)start)
                        return buf;

                (void)sprintf(buf, "%s #%i 0x%08X in ??? ()\n", buf, call, (uintptr_t)pr);
                fp = STACK_FPTR_NEXT_GET(fp);
                call++;
        } while (true);

        return buf;
}
