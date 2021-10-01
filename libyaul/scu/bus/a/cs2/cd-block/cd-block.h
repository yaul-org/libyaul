/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes Machado <abra185@gmail.com>
 */

#ifndef _YAUL_CD_BLOCK_H_
#define _YAUL_CD_BLOCK_H_

#include <stdint.h>

#include <cd-block/cmd.h>

#define ISO9660_SECTOR_SIZE (2048U)

__BEGIN_DECLS

/**
 * Initialize the cd block subsystem.
 *
 * @param standby Controls the time before the CD stops moving while idle
 *                (default timer is 0x1000).
 */
extern int cd_block_init(int16_t standby);

/**
 * Bypass copy protection (by using JHL and CyberWarriorX exploit).
 */
extern int cd_block_security_bypass(void);

/**
 * Transfer data from CD-block buffer to memory.
 *
 * @param offset        Offset from current FAD.
 * @param buffer_number Number of buffer to start reading.
 * @param output_buffer Buffer where data will be recorded.
 * @param buffer_length Size of the buffer where data will be recorded.
 */
extern int cd_block_transfer_data(uint16_t offset, uint16_t buffer_number, uint8_t *output_buffer, uint32_t buffer_length);

/**
 * Read a sector to a memory location. This function initialize and spins the
 * disk to retrieve data.
 *
 * @param fad           FAD to start reading from.
 * @param output_buffer Buffer where data will be recorded.
 */
extern int cd_block_sector_read(fad_t fad, void *output_buffer);

/**
 * Read multiple sectors to a memory location. This function initialize and
 * spins the disk to retrieve data.
 *
 * @param output_buffer Buffer where data will be recorded.
 * @param length        Length of buffer in bytes.
 * @param fad           FAD to start reading from.
 */
extern int cd_block_sectors_read(fad_t fad, void *output_buffer, uint32_t length);

__END_DECLS

#endif /* !_YAUL_CD_BLOCK_H_ */
