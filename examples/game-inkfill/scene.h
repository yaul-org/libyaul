/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef SCENE_H
#define SCENE_H

void scene_nop(void);
void scene_init(void);
int scene_register(const char *, void (*)(void), void (*)(void), void (*)(void), void (*)(void));
void scene_load(const char *);

extern void (*scene_update)(void);
extern void (*scene_draw)(void);
extern void (*scene_exit)(void);

#endif /* !SCENE_H */
