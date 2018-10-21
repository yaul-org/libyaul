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

extern void vdp_init(void);

extern void vdp_sync_init(void);
extern void vdp_sync(int16_t);
extern void vdp1_sync_draw(const struct vdp1_cmdt *, uint16_t);
extern void vdp1_sync_draw_wait(void);
extern void vdp2_sync_commit(void);
extern void vdp2_sync_commit_wait(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* !_VDP_H_ */
