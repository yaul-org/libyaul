/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_KERNEL_SSLOAD_H_
#define _YAUL_KERNEL_SSLOAD_H_

#define SSLOAD_COMM_CMD_ISO9660 (0xEE)
#define SSLOAD_COMM_CMD_LOG     (0xEF)
#define SSLOAD_COMM_CMD_QUIT    (0xF0)

#define SSLOAD_COMM_SYNC (0xF1)

#define SSLOAD_COMM_RET_UNUSED1  (0xF2)
#define SSLOAD_COMM_RET_UNUSED2  (0xF3)
#define SSLOAD_COMM_RET_UNUSED3  (0xF4)
#define SSLOAD_COMM_RET_UNUSED4  (0xF5)
#define SSLOAD_COMM_RET_UNUSED5  (0xF6)
#define SSLOAD_COMM_RET_UNUSED6  (0xF7)
#define SSLOAD_COMM_RET_UNUSED7  (0xF8)
#define SSLOAD_COMM_RET_UNUSED8  (0xF9)
#define SSLOAD_COMM_RET_UNUSED9  (0xFA)
#define SSLOAD_COMM_RET_UNUSED10 (0xFB)
#define SSLOAD_COMM_RET_UNUSED11 (0xFC)
#define SSLOAD_COMM_RET_UNUSED12 (0xFD)
#define SSLOAD_COMM_RET_ERROR    (0xFE)
#define SSLOAD_COMM_RET_OK       (0xFF)

extern void ssload_sync(void);

#endif /* !_YAUL_KERNEL_SSLOAD_H_ */
