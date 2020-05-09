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

#define ISO9660_SECTOR_SIZE 2048
#define CD_STATUS_TIMEOUT 0xAA

#ifndef MIN
#define MIN(X,Y) (X < Y ? X : Y)
#endif

#define FAKE_SECTOR_SIZE 2352
#define FAKE_NUM_SECTORS 150

static int 
cd_block_get_cd_status_flags(uint8_t *flags) 
{
        struct cd_block_status cdStatus;
        int ret;

        assert(flags != NULL);
        if ((ret = cd_block_cmd_get_cd_status(&cdStatus)) != 0)
                return ret;

        *flags = cdStatus.cdStatus;
        return 0;
}


static int  
cd_block_wait_hirq_flag(uint16_t flag)
{
        volatile int i;
        for (i = 0; i < 0x240000; ++i) {
                if (MEMORY_READ(16, CD_BLOCK(HIRQ)) & flag)
                        return 0;
        }

        return -1;
}

static int 
cd_block_auth() 
{
        volatile int i, ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;
        uint8_t flags;
        char tmpBuf[100];

        MEMORY_WRITE(16, CD_BLOCK(HIRQ), ~(DCHG | EFLS));

        // Auth
        regs.hirq_mask = EFLS;
        regs.cr1 = 0xE000;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0)
                return ret;

        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & EFLS) == 0)
                        ;

        // Wait for initial seeking.
        while (1) {
                for (i = 0; i < 100000; ++i)
                        ;

                if ((ret = cd_block_get_cd_status_flags(&flags)) != 0)
                        return ret;
        
                sprintf(tmpBuf, "Return - %d\n", flags);
                dbgio_puts(tmpBuf);
                dbgio_flush();
                vdp_sync(0);

                if (flags == CD_STATUS_PAUSE)
                        break;
                else if (flags == CD_STATUS_FATAL)
                        return -CD_STATUS_FATAL;
                else if (flags == CD_STATUS_REJECT)
                        return -CD_STATUS_REJECT;
        }

        return 0;
}

int 
cd_block_init(int16_t standby)
{       
        int ret;

        // Wait for ESEL.
        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & CMOK) == 0)
                ;

        if ((ret = cd_block_cmd_abort_file()) != 0)
                return ret;

        if ((ret = cd_block_cmd_init_cd_system(standby)) != 0)
                return ret;

        if ((ret = cd_block_cmd_end_data_transfer()) != 0)
                return ret;

        if ((ret = cd_block_cmd_reset_selector(0xFC, 0)) != 0)
                return ret;

        // Wait for ESEL.
        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & ESEL) == 0)
                ;

        // Bypass Auth.
        // if ((ret = cd_block_auth()) != 0)
        //         return ret;

        return 0;
}

int
cd_move_sector_data_cd_auth(uint8_t dstFilter, uint16_t sectorPos,
        uint8_t selNum, uint16_t numSectors)
{
        int i;
	int ret;
        struct cd_block_status cdStatus;
	uint16_t isAuthenticated;

        cd_block_cmd_move_sector_data(dstFilter, sectorPos, selNum,
                numSectors);

	// Clear hirq flags
        MEMORY_WRITE(16, CD_BLOCK(HIRQ), ~(DCHG | EFLS));

	// Authenticate disc
        if ((ret = cd_block_cmd_auth_disk()) != 0)
                return ret;

	// Wait till operation is finished
	while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & EFLS) == 0)
                ;

	// Wait until drive has finished seeking
	for (;;)
	{
		// Wait a bit
		for (i = 0; i < 100000; i++)
                        ;

		if (cd_block_cmd_get_cd_status(&cdStatus) != 0)
                        continue;

		if (cdStatus.cdStatus == CD_STATUS_PAUSE)
                        break;
		else if (cdStatus.cdStatus == CD_STATUS_FATAL)
                        return -CD_STATUS_FATAL;
	}

	// Was Authentication successful?
	if (cd_block_cmd_is_auth(&isAuthenticated) == 0)
		return -1;

	return 0;
}

int 
cd_block_bypass_copy_protection()
{       
        int i, j, ret;
        struct cd_block_regs regs;
        struct cd_block_regs status;

        if ((ret = cd_block_cmd_set_sector_length(SECTOR_LENGTH_2352)) != 0)
                return ret;

        // Write 150 x 2353 sectors.
        if ((ret = cd_block_cmd_put_sector_data(0, 150)) != 0)
                return ret;

        for (j = 0; j < 150; ++j) {
                for (i = 0; i < (2352/4); i += 4) {
                        MEMORY_WRITE(32, CD_BLOCK(DTR), 0x00020002);
                }
        }
        
        if ((ret = cd_block_cmd_end_data_transfer()) != 0)
                return ret;

        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & EHST) == 0)
          ;

        cd_move_sector_data_cd_auth(0, 0, 0, 50);

        cd_block_cmd_is_auth(NULL);

        while (MEMORY_READ(16, CD_BLOCK(HIRQ) & ECPY) == 0)
                ;

        if ((ret = cd_block_cmd_end_data_transfer()) != 0)
                return ret;

        if ((ret = cd_block_cmd_set_sector_length(SECTOR_LENGTH_2048)) != 0)
                return ret;

        if ((ret = cd_block_cmd_reset_selector(0xFC, 0)) != 0)
                return ret;

        return 0;
}

int 
cd_block_transfer_data(uint16_t offset, uint16_t buffnum, 
        uint32_t sizeInBytes, uint8_t* outputBuffer) 
{
        int ret;
        uint32_t i;
        uint32_t numSectors;
        uint16_t *readBuffer;

        assert(outputBuffer != NULL);

        numSectors = sizeInBytes / ISO9660_SECTOR_SIZE;

        // Start transfer
        if ((ret = cd_block_cmd_get_then_delete_sector_data(offset, 
                buffnum, numSectors)) != 0) {

                return ret;
        }

        // Wait for data.
        if (cd_block_wait_hirq_flag(DRDY | EHST) != 0)
                return CD_STATUS_TIMEOUT;


        // Transfer from register to user space.
        readBuffer = (uint16_t*) outputBuffer;
        for (i = 0; i < sizeInBytes; i += 2) {
                *readBuffer = MEMORY_READ(16, CD_BLOCK(DTR));
                readBuffer++;
        }

        if ((ret = cd_block_cmd_end_data_transfer()) != 0)
                return ret;

        return 0;
}

int 
cd_block_read_data(uint16_t fad, uint32_t sizeInBytes, 
        uint8_t* outputBuffer)
{
        assert(outputBuffer != NULL);
        assert(fad >= 150);
        assert((sizeInBytes % ISO9660_SECTOR_SIZE) == 0);

        int ret;
        uint32_t numSectors;
        uint32_t missingBytes;
        uint8_t *outputBuffer2;
        uint8_t statusFlags;

        // At least one sector must be read.
        numSectors = sizeInBytes / ISO9660_SECTOR_SIZE;

        if ((ret = cd_block_cmd_set_sector_length(SECTOR_LENGTH_2048)) != 0)
                return ret;

        if ((ret = cd_block_cmd_reset_selector(0, 0)) != 0)
                return ret;

        if ((ret = cd_block_cmd_set_cd_device_connection(0)) != 0)
                return ret;

        // Start reading.
        if ((ret = cd_block_cmd_play_disk(0, fad, numSectors)) != 0)
                return ret;

        // Wait for data availability.
        outputBuffer2 = outputBuffer;
        missingBytes = sizeInBytes;
        while (true) {
                // If at least one sector has transferred, we copy it.
                int readySectors;
                uint32_t bytesToRead;

                while (!(readySectors = cd_block_cmd_get_sector_number(0)))
                        ;
                        
                bytesToRead = readySectors * ISO9660_SECTOR_SIZE;
                if (bytesToRead > missingBytes)
                        bytesToRead = missingBytes;

                if ((ret = cd_block_transfer_data(0, 0, bytesToRead, 
                        outputBuffer2)) != 0) {

                        return ret;
                }

                outputBuffer2 += bytesToRead;
                missingBytes -= bytesToRead;

                if (missingBytes == 0)
                        break;
        }

        return 0;
}

