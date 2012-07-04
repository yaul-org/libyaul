/*
 * Copyright (c) 2012 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _DMAC_H_
#define _DMAC_H_

#include <inttypes.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct cpu_channel_cfg {
#define CPU_DMAC_CHANNEL(x)     ((x))
        uint32_t ch;

        struct {
#define CPU_DMAC_DESTINATION_FIXED 0x00000000
#define CPU_DMAC_DESTINATION_INCREMENT 0x00004000
#define CPU_DMAC_DESTINATION_DECREMENT 0x00008000
                uint32_t mode;
                void *ptr;
        } dst;

        struct {
#define CPU_DMAC_SOURCE_FIXED 0x00000000
#define CPU_DMAC_SOURCE_INCREMENT 0x00001000
#define CPU_DMAC_SOURCE_DECREMENT 0x00002000
                uint32_t mode;
                const void *ptr;
        } src;

        size_t len;
        uint32_t xfer_size;

        uint8_t vector;
        uint8_t priority;
        void (*ihr)(void);
};

extern void cpu_dmac_channel_set(struct cpu_channel_cfg *);
extern void cpu_dmac_channel_start(uint8_t);
extern void cpu_dmac_channel_stop(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_DMAC_H */
