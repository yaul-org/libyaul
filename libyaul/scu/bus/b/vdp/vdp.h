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

#define vdp_sync_vblank_in_clear() do {                                        \
        vdp_sync_vblank_in_set(NULL, NULL);                                    \
} while (false)

#define vdp_sync_vblank_out_clear() do {                                       \
        vdp_sync_vblank_out_set(NULL, NULL);                                   \
} while (false)

extern void vdp_sync_vblank_in_set(callback_handler_t callback_handler,
    void *work);
extern void vdp_sync_vblank_out_set(callback_handler_t callback_handler,
    void *work);

extern void vdp_dma_enqueue(void *dst, const void *src, size_t len);
extern uint32_t vdp_dma_count_get(void);

extern callback_id_t vdp_dma_callback_add(callback_handler_t callback_handler,
    void *work);
extern void vdp_dma_callback_remove(callback_id_t callback_id);

__END_DECLS

#endif /* !_YAUL_VDP_H_ */
