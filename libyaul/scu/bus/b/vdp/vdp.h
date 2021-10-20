/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP_H_
#define _YAUL_VDP_H_

#include <vdp1.h>
#include <vdp2.h>

#include <sys/callback-list.h>

__BEGIN_DECLS

/// Not yet documented.
#define VDP1_SYNC_INTERVAL_60HZ         (0)
/// Not yet documented.
#define VDP1_SYNC_INTERVAL_VARIABLE     (-1)

typedef enum vdp_sync_mode {
        VDP1_SYNC_MODE_ERASE_CHANGE = 0x00,
        VDP1_SYNC_MODE_CHANGE_ONLY  = 0x01
} vdp_sync_mode_t;

#define vdp_sync_vblank_in_clear() do {                                        \
        vdp_sync_vblank_in_set(NULL, NULL);                                    \
} while (false)

#define vdp_sync_vblank_out_clear() do {                                       \
        vdp_sync_vblank_out_set(NULL, NULL);                                   \
} while (false)

extern void vdp1_sync(void);
extern void vdp1_sync_wait(void);

extern void vdp2_sync(void);
extern void vdp2_sync_wait(void);

extern void vdp1_sync_interval_set(const int8_t interval);
extern vdp_sync_mode_t vdp1_sync_mode_get(void);
extern void vdp1_sync_mode_set(vdp_sync_mode_t mode);

extern void vdp1_sync_cmdt_put(const vdp1_cmdt_t *cmdts, const uint16_t count,
    const uint16_t index, callback_handler_t callback, void *work);

extern void vdp1_sync_cmdt_list_put(const vdp1_cmdt_list_t *cmdt_list,
    const uint16_t index, callback_handler_t callback, void *work);

extern void vdp1_sync_cmdt_orderlist_put(const vdp1_cmdt_orderlist_t *cmdt_orderlist,
    callback_handler_t callback, void *work);

extern void vdp2_sync_commit(void);
extern void vdp2_sync_commit_wait(void);

extern void vdp_sync_vblank_in_set(callback_handler_t callback_handler, void *work);
extern void vdp_sync_vblank_out_set(callback_handler_t callback_handler, void *work);

__END_DECLS

#endif /* !_YAUL_VDP_H_ */
