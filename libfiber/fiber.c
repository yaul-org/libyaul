/*
 * Copyright (c) 2012-2022 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <cpu/intc.h>
#include <cpu/registers.h>

#include "fiber.h"

extern void __context_save(fiber_t *fiber);
extern void __context_switch(fiber_t *from_fiber, fiber_t *to_fiber);

static fiber_t _fiber_parent;
static fiber_t * _fiber_current = &_fiber_parent;

static fiber_stack_alloc_t _stack_alloc = memalign;
static fiber_stack_free_t _stack_free   = free;

void
fiber_init(void)
{
        _fiber_parent.reg_file.sp = (uintptr_t)cpu_dual_master_stack_get();
        _fiber_parent.reg_file.vbr = cpu_reg_vbr_get();
}

void
fiber_stack_allocator_set(fiber_stack_alloc_t stack_alloc, fiber_stack_free_t stack_free)
{
        _stack_alloc = (stack_alloc != NULL) ? stack_alloc : memalign;
        _stack_free = (stack_free != NULL) ? stack_free : free;
}

int32_t
fiber_fiber_init(fiber_t *fiber, ssize_t stack_size, fiber_entry_t entry)
{
        const ssize_t round_stack_size = uint32_pow2_round_next(stack_size);

        fiber->size = clamp(round_stack_size, 16, 4096);
        fiber->stack = _stack_alloc(fiber->size, 16);
        if (fiber->stack == NULL) {
                return -1;
        }

        /* Stack grows towards zero */
        fiber->reg_file.sp = (uintptr_t)&fiber->stack[fiber->size];
        /* When context switching function returns, PC is set to PR, which is
         * why we set PR and not PC. There are no instructions that allow us to
         * manipulate PC */
        fiber->reg_file.pr = (uintptr_t)entry;
        fiber->reg_file.vbr = cpu_reg_vbr_get();

        return 0;
}

void
fiber_fiber_deinit(fiber_t *fiber)
{
        uint32_t * const sp = &fiber->reg_file.sp;

        if (sp != NULL) {
                _stack_free(sp);

                *sp = 0x00000000;
        }
}

void
fiber_yield(fiber_t *to)
{
        const uint8_t sr_mask = cpu_intc_mask_get();
        cpu_intc_mask_set(15);

        if (to == NULL) {
                to = &_fiber_parent;
        }

        fiber_t * const fiber_previous = _fiber_current;

        _fiber_current = to;

        __context_switch(fiber_previous, _fiber_current);

        cpu_intc_mask_set(sr_mask);
}
