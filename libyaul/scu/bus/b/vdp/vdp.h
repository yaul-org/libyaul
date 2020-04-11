/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP_H_
#define _VDP_H_

#include <vdp1.h>
#include <vdp2.h>

__BEGIN_DECLS

#define VDP1_SYNC_INTERVAL_60HZ         (0)
#define VDP1_SYNC_INTERVAL_VARIABLE     (-1)

#define vdp_sync_vblank_in_clear() do {                                        \
        vdp_sync_vblank_in_set(NULL);                                          \
} while (false)

#define vdp_sync_vblank_out_clear() do {                                       \
        vdp_sync_vblank_out_set(NULL);                                         \
} while (false)

extern void vdp_init(void);

extern void vdp_sync_init(void);
extern void vdp_sync(void);

extern bool vdp1_sync_rendering(void);
extern void vdp1_sync_interval_set(const int8_t);
extern void vdp1_sync_cmdt_put(const struct vdp1_cmdt *, const uint16_t,
    void (*)(void *), void *);
extern void vdp1_sync_cmdt_list_put(const struct vdp1_cmdt_list *,
    void (*)(void *), void *);
extern void vdp1_sync_cmdt_orderlist_put(const struct vdp1_cmdt_orderlist *,
    void (*callback)(void *), void *work);
extern uint16_t vdp1_sync_last_command_get(void);
extern void vdp1_sync_last_command_set(const uint16_t);

extern void vdp2_sync_commit(void);

extern void vdp_sync_vblank_in_set(void (*)(void));
extern void vdp_sync_vblank_out_set(void (*)(void));

extern int8_t vdp_sync_user_callback_add(void (*)(void *), void *);
extern void vdp_sync_user_callback_remove(const uint8_t);
extern void vdp_sync_user_callback_clear(void);

__END_DECLS

#endif /* !_VDP_H_ */
