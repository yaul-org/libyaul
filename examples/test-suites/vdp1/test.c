/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#define FIXMATH_NO_OVERFLOW 1
#define FIXMATH_NO_ROUNDING 1
#include <fixmath.h>

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>

#include "globals.h"
#include "test.h"
#include "state.h"

void
test_init(void)
{
        static bool initialized = false;

        static uint16_t blcs_color[] = {
                0x9C00
        };

        vdp2_scrn_back_screen_color_set(VRAM_ADDR_4MBIT(2, 0x01FFFE), 0x9C00);

        if (initialized) {
                return;
        }
        initialized = true;

        text_len = common_round_pow2(CONS_COLS * CONS_ROWS);
        text = (char *)malloc(text_len);
        assert(text != NULL);
        memset(text, '\0', text_len);
}

void
test_exit(void)
{
        state_machine_transition(&state_vdp1, STATE_VDP1_MENU);
}
