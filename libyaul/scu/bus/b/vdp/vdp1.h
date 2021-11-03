/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_VDP1_H_
#define _YAUL_VDP1_H_

#include <vdp1/cmdt.h>
#include <vdp1/env.h>
#include <vdp1/map.h>
#include <vdp1/vram.h>

#include <sys/callback-list.h>

__BEGIN_DECLS

#define vdp1_sync_render_clear() do {                                          \
        vdp1_sync_render_set(NULL, NULL);                                      \
} while (false)

typedef struct vdp1_transfer_status {
        union {
                struct {
                        unsigned int :14;
                        unsigned int cef:1;
                        unsigned int bef:1;
                } __packed;

                uint16_t raw;
        };
} __packed __aligned(2) vdp1_transfer_status_t;

typedef struct vdp1_mode_status {
        union {
                struct {
                        unsigned int version:4;
                        unsigned int :3;
                        unsigned int ptm1:1;
                        unsigned int eos:1;
                        unsigned int die:1;
                        unsigned int dil:1;
                        unsigned int fcm:1;
                        unsigned int vbe:1;
                        unsigned int tvm:3;
                } __packed;

                uint16_t raw;
        };
} __packed __aligned(2) vdp1_mode_status_t;

typedef enum vdp_sync_mode {
        VDP1_SYNC_MODE_ERASE_CHANGE = 0x00,
        VDP1_SYNC_MODE_CHANGE_ONLY  = 0x01
} vdp_sync_mode_t;

static inline __always_inline vdp1_mode_status_t
vdp1_mode_status_get(void)
{
        /* If the structure isn't aligned on a 2-byte boundary, GCC will attempt
         * to invoke memcpy() */
        vdp1_mode_status_t status;

        status.raw = MEMORY_READ(16, VDP1(MODR));

        return status;
}

static inline __always_inline vdp1_transfer_status_t
vdp1_transfer_status_get(void)
{
        /* If the structure isn't aligned on a 2-byte boundary, GCC will
         * attempt to invoke memcpy() */
        vdp1_transfer_status_t status;

        status.raw = MEMORY_READ(16, VDP1(EDSR));

        return status;
}

extern void vdp1_sync(void);
extern bool vdp1_sync_busy(void);
extern void vdp1_sync_wait(void);

/// @htmlinclude vdp1_sync_interval_table.html
extern void vdp1_sync_interval_set(int8_t interval);
extern void vdp1_sync_mode_set(vdp_sync_mode_t mode);
extern vdp_sync_mode_t vdp1_sync_mode_get(void);

extern void vdp1_sync_cmdt_put(const vdp1_cmdt_t *cmdts, uint16_t count,
    uint16_t index);
extern void vdp1_sync_cmdt_list_put(const vdp1_cmdt_list_t *cmdt_list,
    uint16_t index);
extern void vdp1_sync_cmdt_orderlist_put(const vdp1_cmdt_orderlist_t *cmdt_orderlist);
extern void vdp1_sync_cmdt_stride_put(const void *buffer, uint16_t count,
    uint16_t cmdt_index, uint16_t index);

extern void vdp1_sync_put_wait(void);
extern void vdp1_sync_render(void);

extern void vdp1_sync_put_set(callback_handler_t callback_handler,
    void *work);
extern void vdp1_sync_render_set(callback_handler_t callback_handler,
    void *work);
extern void vdp1_sync_transfer_over_set(callback_handler_t callback_handler,
    void *work);

__END_DECLS

#endif /* !_YAUL_VDP1_H_ */
