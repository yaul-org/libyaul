/*
 * Copyright (c) 2012-2014 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef COMMON_H
#define COMMON_H

#define RGB888_TO_RGB555(r, g, b) (0x8000 | (((b) >> 3) << 10) |               \
    (((g) >> 3) << 5) | ((r) >> 3))

#define MATH_SIN(x)     (lut_sin[(x) & 0x1FF])
#define MATH_COS(x)     (lut_cos[(x) & 0x1FF])

#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   224

#define __unused__ __attribute__ ((unused))

void init(void);

void sleep(fix16_t);

void *fs_open(const char *);
void fs_close(void *);
void fs_read(void *, void *);
size_t fs_size(void *);

extern struct cons cons;
extern struct smpc_peripheral_digital digital_pad;

extern char *text;
extern size_t text_len;

extern int32_t lut_cos[];
extern int32_t lut_sin[];

#endif /* !COMMON_H */
