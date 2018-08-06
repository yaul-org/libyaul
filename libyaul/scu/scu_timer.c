/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <sys/cdefs.h>

#include <scu/timer.h>
#include <scu/ic.h>

static void _timer0_handler(void);
static void _timer1_handler(void);

static void _default_ihr(void);

static void (*_timer0_ihr)(void) = _default_ihr;
static void (*_timer1_ihr)(void) = _default_ihr;

void
scu_timer_init(void)
{
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_TIMER_0 | IC_MASK_TIMER_1);

        scu_timer_disable();

        scu_timer_t0_value_set(0);
        scu_timer_t1_value_set(0);

        scu_timer_t0_clear();
        scu_timer_t1_clear();

        scu_ic_ihr_set(IC_INTERRUPT_TIMER_0, _timer0_handler);
        scu_ic_ihr_set(IC_INTERRUPT_TIMER_1, _timer1_handler);
}

void
scu_timer_t0_set(void (*ihr)(void) __unused)
{
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_TIMER_0);

        _timer0_ihr = _default_ihr;

        if (ihr != NULL) {
                scu_ic_mask_chg(~IC_MASK_TIMER_0, IC_MASK_NONE);

                _timer0_ihr = ihr;
        }
}

void
scu_timer_t1_set(void (*ihr)(void) __unused)
{
        scu_ic_mask_chg(IC_MASK_ALL, IC_MASK_TIMER_1);

        _timer1_ihr = _default_ihr;

        if (ihr != NULL) {
                scu_ic_mask_chg(~IC_MASK_TIMER_1, IC_MASK_NONE);

                _timer1_ihr = ihr;
        }
}

static void
_timer0_handler(void)
{
        _timer0_ihr();
}

static void
_timer1_handler(void)
{
        _timer1_ihr();
}

static void
_default_ihr(void)
{
}
