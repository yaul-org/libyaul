/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP_H_
#define _VDP_H_

#include <vdp1.h>
#include <vdp2.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define vdp_sync_vblank_in_clear() do {                                        \
        vdp_sync_vblank_in_set(NULL);                                          \
} while (false)

#define vdp_sync_vblank_out_clear() do {                                       \
        vdp_sync_vblank_out_set(NULL);                                         \
} while (false)

extern void vdp_init(void);

extern void vdp_sync_init(void);
extern void vdp_sync(int16_t);

extern void vdp1_sync_draw(const struct vdp1_cmdt_list *);
extern void vdp1_sync_draw_wait(void);
extern uint16_t vdp1_sync_last_command_get(void);

extern void vdp2_sync_commit(void);

extern void vdp_sync_vblank_in_set(void (*)(void));
extern void vdp_sync_vblank_out_set(void (*)(void));

extern int8_t vdp_sync_user_callback_add(void (*)(void *), void *);
extern void vdp_sync_user_callback_remove(int8_t);
extern void vdp_sync_user_callback_clear(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_VDP_H_ */
