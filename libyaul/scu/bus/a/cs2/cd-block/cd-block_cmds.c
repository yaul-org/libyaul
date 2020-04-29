/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes Machado <abra185@gmail.com>
 */

#include <cd-block.h>
#include <smpc/smc.h>

#include "cd-block-internal.h"
#include <dbgio.h>

static void
cd_block_loop_wait_hirq_flag(uint16_t flag)
{
        while((MEMORY_READ(16, CD_BLOCK(HIRQ)) & flag) == 0)
                ;
}

static int
cd_block_check_return_status(struct cd_block_regs *status) 
{
        uint8_t statusFlags;

        assert(status != NULL);
        statusFlags = status->cr1 & 0xFF;

        if (statusFlags & CD_STATUS_FATAL)
                return -CD_STATUS_FATAL;
        else if (statusFlags & CD_STATUS_ERROR)
                return -CD_STATUS_ERROR;
        else if (statusFlags & CD_STATUS_NO_DISC)
                return -CD_STATUS_NO_DISC;
        else if (statusFlags & CD_STATUS_OPEN)
                return -CD_STATUS_OPEN;
        else
                return 0;
}

int 
cd_block_cmd_get_cd_status(struct cd_block_status *cdStatus)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0000;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (cdStatus != NULL) {
                cdStatus->cdStatus = status.cr1 >> 8;
                cdStatus->flagAndRepeatCount = status.cr1 & 0xFF;
                cdStatus->ctrlAddr = status.cr2 >> 8;
                cdStatus->trackNumber = status.cr2 & 0xFF;
                cdStatus->indexNumber = status.cr3 >> 8;
                cdStatus->fad = ((status.cr3 & 0xFF) << 16) | status.cr4;
        }

        return 0;
}

int 
cd_block_cmd_get_hardware_info(struct cd_block_hardware_info *info)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0100;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret =cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (info != NULL) {
                info->cdStatus = status.cr1 >> 8;
                info->hwFlag = status.cr2 >> 8;
                info->hwVersion = status.cr2 & 0xFF;
                info->mpegVersion = status.cr3 & 0xFF;
                info->driveVersion = status.cr4 >> 8;
                info->driveRevision = status.cr4 & 0xFF;
        }

        return 0;
}

int 
cd_block_cmd_get_toc(uint8_t *cdStatus, uint16_t *tocsize)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0200;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret =cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;
        
        cd_block_loop_wait_hirq_flag(DRDY);

        if (cdStatus != NULL)
                *cdStatus = status.cr1 >> 8;

        if (tocsize != NULL)
                *tocsize = status.cr2;        

        return 0;
}

int 
cd_block_cmd_get_session_info(uint8_t sessionNumber, uint8_t *cdStatus,
                uint8_t *numSessions, uint32_t *sessionLBA)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0300;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret =cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;
        
        if (cdStatus != NULL)
                *cdStatus = status.cr1 >> 8;

        if (numSessions != NULL)
                *numSessions = status.cr3 >> 8;

        if (sessionLBA != NULL)
                *sessionLBA = ((status.cr3 & 0xFF) << 16) | status.cr4;

        return 0;
}

int 
cd_block_cmd_init_cd_system(int16_t standby)
{
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0400;
        regs.cr2 = standby;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x040F;

        return cd_block_cmd_execute(&regs, &status);
}

int 
cd_block_cmd_open_tray(int16_t standby)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0500;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_end_data_transfer(void) 
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0600;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        MEMORY_WRITE(16, CD_BLOCK(HIRQ), (~DRDY) | CMOK);
        return 0;
}

int 
cd_block_cmd_play_disk(int32_t mode, int32_t startFAD, int32_t numSectors)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        // Clear flags
        MEMORY_WRITE(16, CD_BLOCK(HIRQ), ~(PEND | CSCT) | CMOK);

        // The OR on 0x80 for lower byte happens so the system interprets our
        // data as FAD position instead of Track number.
        regs.hirq_mask = 0;
        regs.cr1 = 0x1080 | (startFAD >> 16);
        regs.cr2 = startFAD;
        regs.cr3 = (mode << 8) | 0x80 | (numSectors >> 16);
        regs.cr4 = numSectors;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_seek_disk(uint32_t startPlayPos)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x1180 | ((startPlayPos >> 16) & 0xFF);
        regs.cr2 = (startPlayPos & 0xFFFF);
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_scan_disk(uint8_t scanDirection, uint8_t *cdStatus)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x1200 | scanDirection;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (cdStatus != NULL)
                *cdStatus = status.cr1 >> 8;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_get_subcode(uint8_t type, uint8_t *cdStatus, 
                uint16_t *sizeInWords, uint16_t *flags)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x2000 | type;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (cdStatus != NULL)
                *cdStatus = status.cr1 >> 8;

        if (sizeInWords != NULL)
                *sizeInWords = status.cr2;

        if (flags != NULL)
                *flags = status.cr4;

        return 0;
}

int 
cd_block_cmd_set_cd_device_connection(uint8_t filter)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = ESEL;
        regs.cr1 = 0x3000;
        regs.cr2 = 0x0000;
        regs.cr3 = (filter << 8);
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_get_cd_device_connection(uint8_t *cdStatus, uint8_t *filterNum)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x3100;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (cdStatus != NULL)
                *cdStatus = status.cr1 >> 8;

        if (filterNum != NULL)
                *filterNum = status.cr3 >> 8;

        return 0;
}

int 
cd_block_cmd_get_last_buffer_destination(uint8_t *cdStatus, uint8_t *buffNum)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x3200;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (cdStatus != NULL)
                *cdStatus = status.cr1 >> 8;

        if (buffNum != NULL)
                *buffNum = status.cr3 >> 8;

        return 0;
}

int 
cd_block_cmd_set_filter_range(uint8_t filter, uint32_t fad, uint32_t range)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x4000 | (fad >> 16);
        regs.cr2 = (fad & 0xFFFF);
        regs.cr3 = (filter << 8) | ((range >> 16) & 0xFF);
        regs.cr4 = (range & 0xFFFF);

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        cd_block_loop_wait_hirq_flag(ESEL);
        
        return cd_block_check_return_status(&status);
}

// TODO:
// Get Filter Range(0x41)
// Set Filter Subheader Conditions(0x42)
// Get Filter Subheader Conditions(0x43)
// Set Filter Mode(0x44)
// Get Filter Mode(0x45)
// Set Filter Connection(0x46)
// Get Filter Connection(0x47)

int 
cd_block_cmd_reset_selector(uint8_t flags, uint8_t selNum)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = EFLS;
        regs.cr1 = 0x4800 | flags;
        regs.cr2 = 0x0000;
        regs.cr3 = (selNum << 8);
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        cd_block_loop_wait_hirq_flag(ESEL);

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_get_buffer_size(uint8_t *cdStatus, uint16_t *blockFreeSpace,
                uint8_t *maxSelectors, uint16_t *maxBlocks)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x5000;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (cdStatus != NULL)
                *cdStatus = status.cr1 >> 8;

        if (blockFreeSpace != NULL)
                *blockFreeSpace = status.cr2;

        if (maxSelectors != NULL)
                *maxSelectors = status.cr3 >> 8;

        if (maxBlocks != NULL)
                *maxBlocks = status.cr4;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_get_sector_number(uint8_t buffNum)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x5100;
        regs.cr2 = 0x0000;
        regs.cr3 = (buffNum << 8);
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return 0;

        return status.cr4;
}

// TODO:
// Calculate Actual Size(0x52)
// Get Actual Size(0x53)
// Get Sector Info(0x54)
// Execute FAD Search(0x55)
// Get FAD Search Results(0x56)

int 
cd_block_cmd_set_sector_length(uint16_t size) 
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = ESEL;
        regs.cr1 = 0x6000 | (size & 0xFF);
        regs.cr2 = (size << 8);
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_get_sector_data(uint16_t secOffset, uint8_t bufNum,
        uint16_t secNum) 
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x6100;
        regs.cr2 = secOffset;
        regs.cr3 = bufNum << 8;
        regs.cr4 = secNum;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_delete_sector_data(uint16_t secPosition, uint8_t bufNum,
        uint16_t secNum) 
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = EHST;
        regs.cr1 = 0x6200;
        regs.cr2 = secPosition;
        regs.cr3 = bufNum << 8;
        regs.cr4 = secNum;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_get_then_delete_sector_data(uint16_t offset, uint8_t buffNum, 
                uint16_t secNum)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = EHST;
        regs.cr1 = 0x6300;
        regs.cr2 = offset;
        regs.cr3 = (buffNum << 8);
        regs.cr4 = secNum;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;
        
        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_put_sector_data(uint8_t buffNum, uint16_t secNum) 
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = EHST;
        regs.cr1 = 0x6400;
        regs.cr2 = 0x0000;
        regs.cr3 = (buffNum << 8);
        regs.cr4 = secNum;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_copy_sector_data(uint8_t dstFilter, uint16_t secOffset,
        uint8_t buffNum, uint16_t secNum) 
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x6500 | dstFilter;
        regs.cr2 = secOffset;
        regs.cr3 = (buffNum << 8);
        regs.cr4 = secNum;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_move_sector_data(uint8_t dstFilter, uint16_t secOffset,
        uint8_t buffNum, uint16_t secNum) 
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x6600 | dstFilter;
        regs.cr2 = secOffset;
        regs.cr3 = (buffNum << 8);
        regs.cr4 = secNum;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_get_copy_error(uint8_t *cdStatus, uint8_t *errorCode)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x6700;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (cdStatus != NULL)
                *cdStatus = status.cr1 >> 8;

        if (errorCode != NULL)
                *errorCode = status.cr1 & 0xFF;

        return 0;
}

// TODO:
// Change Directory(0x70)
// Read Directory(0x71)
// Get File System Scope(0x72)
// Get File Info(0x73)
// Read File(0x74)

int 
cd_block_cmd_abort_file(void)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        /* Abort file */
        regs.hirq_mask = EFLS;
        regs.cr1 = 0x7500;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        return cd_block_check_return_status(&status);
}

int 
cd_block_cmd_auth_disk(void)
{
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = EFLS;
        regs.cr1 = 0xE000;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        return cd_block_cmd_execute(&regs, &status);
}

int 
cd_block_cmd_is_auth(uint16_t *diskTypeAuth)
{
        int ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        regs.hirq_mask = 0;
        regs.cr1 = 0xE100;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        if (diskTypeAuth != NULL)
                *diskTypeAuth = status.cr2;

        // Disc type Authenticated:
        // 0x00: No CD/Not Authenticated
        // 0x01: Audio CD
        // 0x02: Regular Data CD(not Saturn disc)
        // 0x03: Copied/Pirated Saturn Disc
        // 0x04: Original Saturn Disc
        return ((status.cr2 != 0x00) && (status.cr2 != 0x03));
}
