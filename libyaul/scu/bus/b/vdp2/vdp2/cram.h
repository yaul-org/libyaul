/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _CRAM_H_
#define _CRAM_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Color RAM (CRAM) stores color data of sprites and scroll screens.
 *
 * R/W access from the CPU or DMA controller(s) is possible, but the
 * image may be disturbed by the access timing. Access through CPU or
 * DMA controller(s) is possible only in word units and long word
 * units. Access in bytes is now allowed. */

#define CRAM_OFFSET(x, y, z)    (0x25F00000 + ((x) << 9) +                     \
            ((y) << 5) + ((z) << 1))

#define CRAM_SIZE               0x1000

/*
 * Mode 0: RGB 555 1,024 colors mirrored
 * Mode 1: RGB 555 2,048 colors
 * Mode 2: RGB 888 1,024 colors
 */

#define CRAM_NBG0_OFFSET(x, y, z) CRAM_OFFSET(x, y, z)
#define CRAM_RBG1_OFFSET(x, y, z) CRAM_NBG0_OFFSET(x, y, z)
#define CRAM_NBG1_OFFSET(x, y, z) CRAM_OFFSET(x, y, z)
#define CRAM_NBG2_OFFSET(x, y, z) CRAM_OFFSET(x, y, z)
#define CRAM_NBG3_OFFSET(x, y, z) CRAM_OFFSET(x, y, z)
#define CRAM_RBG0_OFFSET(x, y, z) CRAM_OFFSET(x, y, z)
#define CRAM_VDP1_OFFSET(x, y, z) CRAM_OFFSET(x, y, z)

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* !_CRAM_H_ */
