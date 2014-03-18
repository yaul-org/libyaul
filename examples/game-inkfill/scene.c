/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#include <assert.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include <sys/queue.h>

#include "globals.h"
#include "scene.h"

void (*scene_update)(void) = scene_nop;
void (*scene_draw)(void) = scene_nop;
void (*scene_exit)(void) = scene_nop;

TAILQ_HEAD(scenes, scene);

struct scene {
        const char *name;
        void (*init)(void);
        void (*update)(void);
        void (*draw)(void);
        void (*exit)(void);

        TAILQ_ENTRY(scene) entries;
};

static struct scenes scenes;

void
scene_init(void)
{
        TAILQ_INIT(&scenes);

        scene_update = scene_nop;
        scene_draw = scene_nop;
        scene_exit = scene_nop;
}

int
scene_register(const char *name, void (*init)(void), void (*update)(void),
    void (*draw)(void), void (*exit)(void))
{
        struct scene *scene;

        TAILQ_FOREACH (scene, &scenes, entries) {
                if ((strcmp(scene->name, name)) == 0) {
                        /* Duplicate exists */
                        return -1;
                }
        }

        scene = (struct scene *)malloc(sizeof(struct scene));
        assert(scene != NULL);

        scene->name = name;
        scene->init = init;
        scene->update = update;
        scene->draw = draw;
        scene->exit = exit;

        TAILQ_INSERT_TAIL(&scenes, scene, entries);

        return 0;
}

void
scene_load(const char *name)
{
        struct scene *scene;

        TAILQ_FOREACH (scene, &scenes, entries) {
                if ((strcmp(scene->name, name)) == 0) {
                        scene_exit();

                        scene->init();

                        scene_update = scene->update;
                        scene_draw = scene->draw;
                        scene_exit = scene->exit;
                        return;
                }
        }
}

void
scene_nop(void)
{
        /* Do nothing */
}
