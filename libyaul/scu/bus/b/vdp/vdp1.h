/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VDP1_H_
#define _VDP1_H_

#include <vdp1/cmdt.h>
#include <vdp1/env.h>
#include <vdp1/map.h>
#include <vdp1/vram.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct vdp1_transfer_status {
        union {
                struct {
                        unsigned :14;
                        unsigned vte_cef:1;
                        unsigned vte_bef:1;
                } __packed;

                uint16_t raw;
        };
} __packed __aligned(2);

struct vdp1_mode_status {
        union {
                struct {
                        unsigned vms_version:4;
                        unsigned :3;
                        unsigned vms_ptm1:1;
                        unsigned vms_eos:1;
                        unsigned vms_die:1;
                        unsigned vms_dil:1;
                        unsigned vms_fcm:1;
                        unsigned vms_vbe:1;
                        unsigned vms_tvm:3;
                } __packed;

                uint16_t raw;
        };
} __packed __aligned(2);

static inline void __always_inline
vdp1_mode_status_get(struct vdp1_mode_status *status)
{
        /* If the structure isn't aligned on a 2-byte boundary, GCC will
         * attempt to invoke memcpy() */
        status->raw = MEMORY_READ(16, VDP1(MODR));
}

static inline void __always_inline
vdp1_transfer_status_get(struct vdp1_transfer_status *status)
{
        /* If the structure isn't aligned on a 2-byte boundary, GCC will
         * attempt to invoke memcpy() */
        status->raw = MEMORY_READ(16, VDP1(EDSR));
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_VDP1_H_ */
