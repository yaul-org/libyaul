/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes Machado <abra185@gmail.com>
 */

#ifndef _CD_BLOCK_CMD_H_
#define _CD_BLOCK_CMD_H_

#include <stdint.h>

#include <sys/cdefs.h>

__BEGIN_DECLS

#define SECTOR_LENGTH_2048 0
#define SECTOR_LENGTH_2336 1
#define SECTOR_LENGTH_2340 2
#define SECTOR_LENGTH_2352 3

/* Helpers */
#define FAD2LBA(x)      ((x) - 150)
#define LBA2FAD(x)      ((x) + 150)

typedef uint32_t fad_t; 

struct cd_block_hardware_info {
        uint8_t cdStatus;
        uint8_t hwFlag;
        uint8_t hwVersion; 
        uint8_t mpegVersion; 
        uint8_t driveVersion; 
        uint8_t driveRevision;
};

#define CD_STATUS_BUSY          0x00
#define CD_STATUS_PAUSE         0x01
#define CD_STATUS_STANDBY       0x02
#define CD_STATUS_PLAY          0x03
#define CD_STATUS_SEEK          0x04
#define CD_STATUS_SCAN          0x05
#define CD_STATUS_OPEN          0x06
#define CD_STATUS_NO_DISC       0x07
#define CD_STATUS_RETRY         0x08
#define CD_STATUS_ERROR         0x09
#define CD_STATUS_FATAL         0x0A /* Fatal error (hard reset required) */
#define CD_STATUS_PERI          0x20 /* Periodic response if set, else command response */
#define CD_STATUS_TRANS         0x40 /* Data transfer request if set */
#define CD_STATUS_WAIT          0x80
#define CD_STATUS_REJECT        0xFF

struct cd_block_status {
        uint8_t cdStatus;
        uint8_t flagAndRepeatCount;
        uint8_t ctrlAddr;
        uint8_t trackNumber;
        uint8_t indexNumber;
        uint32_t fad;
};

extern int cd_block_cmd_get_cd_status(struct cd_block_status *cdStatus);
extern int cd_block_cmd_get_hardware_info(struct cd_block_hardware_info *info);
extern int cd_block_cmd_get_toc(uint8_t *cdStatus, uint16_t *tocsize);
extern int cd_block_cmd_get_session_info(uint8_t sessionNumber, uint8_t *cdStatus, uint8_t *numSessions, uint32_t *sessionLBA);
extern int cd_block_cmd_init_cd_system(int16_t standby);
extern int cd_block_cmd_open_tray(int16_t standby);
extern int cd_block_cmd_end_data_transfer(void);
extern int cd_block_cmd_play_disk(int32_t mode, uint32_t start_fad, int32_t num_sectors);
extern int cd_block_cmd_seek_disk(uint32_t startPlayPos);
extern int cd_block_cmd_scan_disk(uint8_t scanDirection, uint8_t *cdStatus);
extern int cd_block_cmd_get_subcode(uint8_t type, uint8_t *cdStatus, uint16_t *sizeInWords, uint16_t *flags);
extern int cd_block_cmd_set_cd_device_connection(uint8_t filter);
extern int cd_block_cmd_get_cd_device_connection(uint8_t *cdStatus, uint8_t *filterNum);
extern int cd_block_cmd_get_last_buffer_destination(uint8_t *cdStatus, uint8_t *buffNum);
extern int cd_block_cmd_set_filter_range(uint8_t filter, uint32_t fad, uint32_t range);

// TODO:
// Get Filter Range(0x41)
// Set Filter Subheader Conditions(0x42)
// Get Filter Subheader Conditions(0x43)
// Set Filter Mode(0x44)
// Get Filter Mode(0x45)
// Set Filter Connection(0x46)
// Get Filter Connection(0x47)

extern int cd_block_cmd_reset_selector(uint8_t flags, uint8_t selNum);
extern int cd_block_cmd_get_buffer_size(uint8_t *cdStatus, uint16_t *blockFreeSpace, uint8_t *maxSelectors, uint16_t *maxBlocks);
extern int cd_block_cmd_get_sector_number(uint8_t buffNum);

// TODO:
// Calculate Actual Size(0x52)
// Get Actual Size(0x53)
// Get Sector Info(0x54)
// Execute FAD Search(0x55)
// Get FAD Search Results(0x56)

extern int cd_block_cmd_set_sector_length(uint16_t size);
extern int cd_block_cmd_get_sector_data(uint16_t secOffset, uint8_t bufNum, uint16_t secNum);
extern int cd_block_cmd_delete_sector_data(uint16_t secPosition, uint8_t bufNum, uint16_t secNum);
extern int cd_block_cmd_get_then_delete_sector_data(uint16_t offset, uint8_t buffNum, uint16_t secNum);
extern int cd_block_cmd_put_sector_data(uint8_t buffNum, uint16_t secNum);
extern int cd_block_cmd_copy_sector_data(uint8_t dstFilter, uint16_t secOffset, uint8_t buffNum, uint16_t secNum);
extern int cd_block_cmd_move_sector_data(uint8_t dstFilter, uint16_t secOffset, uint8_t buffNum, uint16_t secNum);

// TODO:
// Get Copy Error(0x67)
// Change Directory(0x70)
// Read Directory(0x71)
// Get File System Scope(0x72)
// Get File Info(0x73)
// Read File(0x74)

extern int cd_block_cmd_abort_file(void);
extern int cd_block_cmd_auth_disk(void);
extern int cd_block_cmd_is_auth(uint16_t *diskTypeAuth);

__END_DECLS

#endif /* !_CD_BLOCK_CMD_H_ */
