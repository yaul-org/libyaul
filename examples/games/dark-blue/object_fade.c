/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <yaul.h>

#include "physics.h"
#include "objects.h"

#define FADE_STATE_STOP         0
#define FADE_STATE_FADE         1

static const char *fade_state2str[] = {
        "FADE_STATE_STOP",
        "FADE_STATE_FADE"
};

static void on_init(struct object *);
static void on_update(struct object *);
static void on_draw(struct object *);

static void m_start(struct object *, bool, fix16_t);
static bool m_faded(struct object *);

struct object_fade object_fade = {
        .active = true,
        .id = OBJECT_FADE_ID,
        .rigid_body = NULL,
        .colliders = {
                NULL
        },
        .on_init = on_init,
        .on_update = on_update,
        .on_draw = on_draw,
        .on_destroy = NULL,
        .on_collision = NULL,
        .on_trigger = NULL,
        .functions = {
                .m_start = m_start,
                .m_faded = m_faded
        },
        .private_data = {
                .m_state = FADE_STATE_STOP,
                .m_fade_in = true,
                .m_speed = F16(0.5f),
                .m_fade = F16(-256.0f)
        }
};

static void
on_init(struct object *this)
{
        THIS_PRIVATE_DATA(object_fade, state) = FADE_STATE_STOP;
        THIS_PRIVATE_DATA(object_fade, fade_in) = true;
        THIS_PRIVATE_DATA(object_fade, speed) = F16(0.5f);
        THIS_PRIVATE_DATA(object_fade, fade) = F16(-256.0f);

        vdp2_scrn_color_offset_set(SCRN_NBG0, SCRN_COLOR_OFFSET_A);
        vdp2_scrn_color_offset_set(SCRN_NBG1, SCRN_COLOR_OFFSET_A);
        vdp2_scrn_color_offset_set(SCRN_NBG2, SCRN_COLOR_OFFSET_A);
        vdp2_scrn_color_offset_set(SCRN_BACK, SCRN_COLOR_OFFSET_A);
        vdp2_scrn_color_offset_set(SCRN_SPRITE, SCRN_COLOR_OFFSET_A);

        int16_t fade;
        fade = fix16_to_int(THIS_PRIVATE_DATA(object_fade, fade));

        vdp2_scrn_color_offset_rgb_set(SCRN_COLOR_OFFSET_A, fade, fade, fade);
}

static void
on_update(struct object *this)
{
        (void)sprintf(text,
            "fade state=%s\n",
            fade_state2str[THIS_PRIVATE_DATA(object_fade, state)]);
        cons_buffer(text);

        switch (THIS_PRIVATE_DATA(object_fade, state)) {
        case FADE_STATE_STOP:
                break;
        case FADE_STATE_FADE:
                /* Fade */
                THIS_PRIVATE_DATA(object_fade, fade) = fix16_add(
                    THIS_PRIVATE_DATA(object_fade, fade),
                    THIS_PRIVATE_DATA(object_fade, speed));

                /* Clamp */
                THIS_PRIVATE_DATA(object_fade, fade) = fix16_clamp(
                    THIS_PRIVATE_DATA(object_fade, fade),
                    F16(-256.0f),
                    F16(0.0f));

                /* Check if we need to stop */
                fix16_t fade;
                fade = THIS_PRIVATE_DATA(object_fade, fade);

                if ((fade >= F16(0.0f)) || (fade <= F16(-256.0f))) {
                        THIS_PRIVATE_DATA(object_fade, state) = FADE_STATE_STOP;
                }
                break;
        }
}

static void
on_draw(struct object *this)
{
        int16_t fade;
        fade = fix16_to_int(THIS_PRIVATE_DATA(object_fade, fade));

        vdp2_scrn_color_offset_rgb_set(SCRN_COLOR_OFFSET_A, fade, fade, fade);
}

static void
m_start(struct object *this, bool fade_in, fix16_t speed)
{
        if (!(THIS_CALL_PUBLIC_MEMBER(object_fade, faded))) {
                return;
        }

        THIS_PRIVATE_DATA(object_fade, fade_in) = fade_in;
        THIS_PRIVATE_DATA(object_fade, speed) = (fade_in) ? speed : -speed;

        THIS_PRIVATE_DATA(object_fade, state) = FADE_STATE_FADE;
}

static bool
m_faded(struct object *this)
{
        return THIS_PRIVATE_DATA(object_fade, state) == FADE_STATE_STOP;
}
