/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes Machado <abra185@gmail.com>
 */

#ifndef _CD_BLOCK_H_
#define _CD_BLOCK_H_

#include <cd-block/cmd.h>
#include <stdint.h>


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
extern int cd_block_bypass_copy_protection();

/**
 * Transfer data from cdblock buffer to memory.
 * 
 * @param offset Offset from current FAD.
 * @param buffnum Number of buffer to start reading.
 * @param sizeInBytes Number of bytes to transfer.
 * @param outputBuffer Buffer where data will be recorded.
 */
extern int cd_block_transfer_data(uint16_t offset, 
        uint16_t buffnum, uint32_t sizeInBytes, uint8_t* outputBuffer);

/**
 * Read bytes from a sector to a memory location. This function initialize
 * and spins the disk to retrieve data.
 * 
 * @param fad Fad to start reading from.
 * @param buffnum Number of buffer to start reading.
 * @param sizeInBytes Number of bytes to transfer.
 * @param outputBuffer Buffer where data will be recorded.
 */
extern int cd_block_read_data(uint16_t fad, uint32_t sizeInBytes, 
        uint8_t* outputBuffer);

__END_DECLS

#endif /* !_CD_BLOCK_H_ */
