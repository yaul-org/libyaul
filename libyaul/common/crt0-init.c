/*
 * Copyright (c) 2012-2016
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 * Joe Fenton <jlfenton65@gmail.com>
 */

#include <sys/cdefs.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <cpu/intc.h>
#include <sys/init.h>

#include "exception.h"

static void __used
call_global_ctors(void)
{
        extern void (*__CTOR_LIST__[])(void);

        /* Constructors are called in reverse order of the list */
        for (int32_t i = (int32_t)__CTOR_LIST__[0]; i >= 1; i--) {
                /* Each function handles one or more destructor (within
                 * file scope) */
                __CTOR_LIST__[i]();
        }
}

/* Do all destructors */
static void __used
call_global_dtors(void)
{
        extern void (*__DTOR_LIST__[])(void);

        /* Destructors in forward order */
        for (int32_t i = 0; i < (int32_t)__DTOR_LIST__[0]; i++) {
                /* Each function handles one or more destructor (within
                 * file scope) */
                __DTOR_LIST__[i + 1]();
        }
}

/* Add function to .init section */
static void __used __section(".init")
initializer(void)
{
        void (**vbr)(void);

        /* Set hardware exception handling routines */
        vbr = cpu_intc_vector_base_get();

        /* Bypass our trampoline IHR as these IHR have to push all of
         * the CPU registers onto the stack */
        vbr[0x04] = exception_illegal_instruction;
        vbr[0x06] = exception_illegal_slot;
        vbr[0x09] = exception_cpu_address_error;
        vbr[0x0A] = exception_dma_address_error;

        call_global_ctors();

        init();
}

/* Add function to .fini section */
static void __used __section(".fini")
finalizer(void)
{
        call_global_dtors();
}
