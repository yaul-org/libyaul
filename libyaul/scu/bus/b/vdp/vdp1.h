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

__BEGIN_DECLS

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

static inline void __always_inline
vdp1_mode_status_get(vdp1_mode_status_t *status)
{
        /* If the structure isn't aligned on a 2-byte boundary, GCC will
         * attempt to invoke memcpy() */
        status->raw = MEMORY_READ(16, VDP1(MODR));
}

static inline void __always_inline
vdp1_transfer_status_get(vdp1_transfer_status_t *status)
{
        /* If the structure isn't aligned on a 2-byte boundary, GCC will
         * attempt to invoke memcpy() */
        status->raw = MEMORY_READ(16, VDP1(EDSR));
}

__END_DECLS

#endif /* !_YAUL_VDP1_H_ */
