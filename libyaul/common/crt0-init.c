/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com
 * Joe Fenton <jlfenton65@gmail.com>
 */

#include <cpu/intc.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

#include "exception.h"

typedef void (*fptr)(void);

extern void __call_exitprocs(int, void *);

extern fptr __CTOR_LIST__[];
extern fptr __DTOR_LIST__[];

/* Used by exit procs */
void *__dso_handle = NULL;

/* Do all constructors. */
static void __attribute__ ((used))
__do_global_ctors(void)
{
        /*
         * See <http://gcc.gnu.org/onlinedocs/gccint/Initialization.html>
         */

        do {
                uint32_t i;
                uint32_t n;

                n = (uint32_t)__CTOR_LIST__[0];
                /* Constructors are called in reverse order of the list */
                for (i = n; i >= 1; i--)
                        __CTOR_LIST__[i]();
        } while (false);
}

/* Do all destructors. */
static void __attribute__ ((used))
__do_global_dtors(void)
{
        uint32_t i;
        uint32_t n;

        do {
                n = (uint32_t)__DTOR_LIST__[0];
                /* Destructors in forward order */
                for (i = 0; i < n; i++)
                        __DTOR_LIST__[i + 1]();
        } while (false);
}

/* Add function to .init section */
static void __attribute__ ((used, section (".init")))
__std_startup(void)
{
        void (**vbr)(void);

        /* First added, last called */
        atexit(__do_global_dtors);

        /* Do all constructors */
        __do_global_ctors();

        /* Set hardware exception handling routines */
        vbr = cpu_intc_vector_base_get();

        vbr[0x04] = exception_illegal_instruction;
        vbr[0x06] = exception_illegal_slot;
        vbr[0x09] = exception_cpu_address_error;
        vbr[0x0A] = exception_dma_address_error;
}

/* Add function to .fini section */
static void __attribute__ ((used, section (".fini")))
__std_cleanup(void)
{

        __call_exitprocs(0, NULL);
}
