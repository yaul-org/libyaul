/*
 * Copyright (c) 2012
 * See LICENSE for details.
 *
 * Israel Jacques <mrko@eecs.berkeley.edu
 * Joe Fenton <jlfenton65@gmail.com>
 */

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdbool.h>

#include <cpu/intc.h>
#include <exception.h>

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

        do {
                uint32_t i, n = (uint32_t)__CTOR_LIST__[0];
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
                for (i = 0; i < n; i++)
                        __DTOR_LIST__[i + 1]();
        } while (false);
}

/* Add function to .init section */
static void __attribute__ ((used, section (".init")))
__std_startup(void)
{
        atexit(__do_global_dtors); /* First added, last called */

        __do_global_ctors(); /* Do all constructors. */

        /* Set hardware exception handlers */
        cpu_intc_vct_set(0x04, (uint32_t)&exception_illegal_instruction);
        cpu_intc_vct_set(0x06, (uint32_t)&exception_illegal_slot);
        cpu_intc_vct_set(0x09, (uint32_t)&exception_cpu_address_error);
        cpu_intc_vct_set(0x0A, (uint32_t)&exception_dma_address_error);
}

/* Add function to .fini section */
static void __attribute__ ((used, section (".fini")))
__std_cleanup(void)
{

        __call_exitprocs(0, NULL);
}
