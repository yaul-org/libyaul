/*
 * Copyright (c) 2012-2019 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 * Romulo Fernandes Machado <abra185@gmail.com>
 */

#include <cd-block.h>

#include <cpu/instructions.h>

#include <smpc/smc.h>

#include "cd-block-internal.h"

#define CD_STATUS_TIMEOUT 0xAA

#define FAKE_SECTOR_SIZE        2352
#define FAKE_NUM_SECTORS        150

static int _status_flags_get(uint8_t *);
static int _hirq_flag_wait(uint16_t);
static int _cd_block_auth(void);

int
cd_block_init(int16_t standby)
{
        int ret;

        /* Wait for ESEL */
        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & CMOK) == 0) {
        }

        if ((ret = cd_block_cmd_file_abort()) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_cd_system_init(standby)) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_data_transfer_end()) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_selector_reset(0xFC, 0)) != 0) {
                return ret;
        }

        /* Wait for ESEL */
        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & ESEL) == 0) {
        }

        return 0;
}

int
cd_sector_data_cd_auth_move(uint8_t dst_filter, uint16_t sector_pos,
    uint8_t sel_num, uint16_t num_sectors)
{
        int i;
        int ret;
        struct cd_block_status cd_status;
        uint16_t is_authenticated;

        cd_block_cmd_sector_data_move(dst_filter, sector_pos, sel_num,
                                      num_sectors);

        /* Clear hirq flags */
        MEMORY_WRITE(16, CD_BLOCK(HIRQ), ~(DCHG | EFLS));

        /* Authenticate disc */
        if ((ret = cd_block_cmd_disk_auth()) != 0) {
                return ret;
        }

        /* Wait till operation is finished */
        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & EFLS) == 0)
                ;

        /* Wait until drive has finished seeking */
        for (;;) {
                /* Wait a bit */
                for (i = 0; i < 100000; i++) {
                }

                if (cd_block_cmd_status_get(&cd_status) != 0) {
                        continue;
                }

                if (cd_status.cd_status == CD_STATUS_PAUSE) {
                        break;
                }

                if (cd_status.cd_status == CD_STATUS_FATAL) {
                        return -CD_STATUS_FATAL;
                }
        }

        /* Was Authentication successful? */
        if ((cd_block_cmd_auth_check(&is_authenticated)) == 0) {
                return -1;
        }

        return 0;
}

int
cd_block_security_bypass(void)
{
        int i;
        int j;
        int ret;
        cd_block_regs_t regs __unused;
        cd_block_regs_t status __unused;

        if ((ret = cd_block_cmd_sector_length_set(SECTOR_LENGTH_2352)) != 0) {
                return ret;
        }

        /* Write 150 x 2353 sectors */
        if ((ret = cd_block_cmd_sector_data_put(0, 150)) != 0) {
                return ret;
        }

        for (j = 0; j < 150; ++j) {
                for (i = 0; i < (2352 / 4); i += 4) {
                        MEMORY_WRITE(32, CD_BLOCK(DTR), 0x00020002);
                }
        }

        if ((ret = cd_block_cmd_data_transfer_end()) != 0) {
                return ret;
        }

        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & EHST) == 0) {
        }

        cd_sector_data_cd_auth_move(0, 0, 0, 50);

        cd_block_cmd_auth_check(NULL);

        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & ECPY) == 0) {
        }

        if ((ret = cd_block_cmd_data_transfer_end()) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_sector_length_set(SECTOR_LENGTH_2048)) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_selector_reset(0xFC, 0)) != 0) {
                return ret;
        }

        return 0;
}

int
cd_block_transfer_data(uint16_t offset, uint16_t buffer_number, uint8_t *output_buffer, uint32_t buffer_length)
{
        assert(output_buffer != NULL);

        /* Start transfer */
        int ret;
        ret = cd_block_cmd_sector_data_get_delete(offset, buffer_number, 1);
        if (ret != 0) {
                return ret;
        }

        /* Wait for data */
        if ((_hirq_flag_wait(DRDY | EHST)) != 0) {
                return CD_STATUS_TIMEOUT;
        }

        /* Transfer from register to user space */
        uint16_t *read_buffer;
        read_buffer = (uint16_t *)output_buffer;

        uint32_t bytes_to_read = ISO9660_SECTOR_SIZE;
        if (bytes_to_read > buffer_length) {
                bytes_to_read = buffer_length;
        }

        uint32_t read_bytes = 0;
        for (uint32_t i = 0; i < bytes_to_read; i += 2) {
                *read_buffer = MEMORY_READ(16, CD_BLOCK(DTR));
                read_buffer++;
                read_bytes += 2;
        }

        /* If odd number of bytes, read the last one separated */
        if (read_bytes < buffer_length) {
                const uint16_t tmp = MEMORY_READ(16, CD_BLOCK(DTR));

                output_buffer[buffer_length - 1] = tmp >> 8;
        }

        if ((ret = cd_block_cmd_data_transfer_end()) != 0) {
                return ret;
        }

        return 0;
}

int
cd_block_sector_read(fad_t fad, void *output_buffer)
{
        assert(fad >= 150);
        assert(output_buffer != NULL);

        int ret;

        if ((ret = cd_block_cmd_sector_length_set(SECTOR_LENGTH_2048)) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_selector_reset(0, 0)) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_cd_dev_connection_set(0)) != 0) {
                return ret;
        }

        /* Start reading */
        if ((ret = cd_block_cmd_disk_play(0, fad, /* num_sectors */ 1)) != 0) {
                return ret;
        }

        /* If at least one sector has transferred, we copy it */
        while ((cd_block_cmd_sector_number_get(0)) == 0) {
        }

        if ((ret = cd_block_transfer_data(0, 0, output_buffer, ISO9660_SECTOR_SIZE)) != 0) {
                return ret;
        }

        return 0;
}

int
cd_block_sectors_read(fad_t fad, void *output_buffer, uint32_t length)
{
        assert(fad >= 150);
        assert(output_buffer != NULL);
        assert(length > 0);

        uint8_t *buffer_ptr = (uint8_t *)output_buffer;

        /* Get the sector count from length */
        const uint32_t sector_count = (length + (ISO9660_SECTOR_SIZE - 1)) / ISO9660_SECTOR_SIZE;

        int ret;

        if ((ret = cd_block_cmd_selector_reset(0, 0)) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_cd_dev_connection_set(0)) != 0) {
                return ret;
        }

        if ((ret = cd_block_cmd_disk_play(0, fad, sector_count)) != 0) {
                return ret;
        }

        uint32_t bytes_missing = length;

        while (bytes_missing > 0) {
                /* Wait until there's data ready */
                uint32_t sectors_ready;

                do {
                        sectors_ready = cd_block_cmd_sector_number_get(0);
                } while (sectors_ready == 0);

                uint32_t bytes_to_read;

                if ((sectors_ready * ISO9660_SECTOR_SIZE) > bytes_missing) {
                        bytes_to_read = bytes_missing;
                } else {
                        bytes_to_read = sectors_ready * ISO9660_SECTOR_SIZE;
                }

                /* Setup a transfer from CD buffer to buffer, then delete data
                 * from CD buffer */
                if ((ret = cd_block_transfer_data(0, 0, buffer_ptr, bytes_to_read)) != 0) {
                        return ret;
                }

                buffer_ptr += bytes_to_read;
                bytes_missing -= bytes_to_read;
        }

        return 0;
}

static int
_status_flags_get(uint8_t *flags)
{
        assert(flags != NULL);

        struct cd_block_status cd_status;

        int ret;

        if ((ret = cd_block_cmd_status_get(&cd_status)) != 0) {
                return ret;
        }

        *flags = cd_status.cd_status;

        return 0;
}

static int
_hirq_flag_wait(uint16_t flag)
{
        volatile uint32_t i;

        for (i = 0; i < 0x240000; ++i) {
                if (MEMORY_READ(16, CD_BLOCK(HIRQ)) & flag) {
                        return 0;
                }
        }

        return -1;
}

static int __used
_cd_block_auth(void)
{
        volatile int i;
        cd_block_regs_t regs;
        cd_block_regs_t status;
        uint8_t flags;

        MEMORY_WRITE(16, CD_BLOCK(HIRQ), ~(DCHG | EFLS));

        /* Auth */
        regs.hirq_mask = EFLS;
        regs.cr1 = 0xE000;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        volatile int ret;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & EFLS) == 0) {
        }

        /* Wait for initial seeking */
        while (true) {
                for (i = 0; i < 131072; ++i) {
                        cpu_instr_nop();
                }

                if ((ret = _status_flags_get(&flags)) != 0) {
                        return ret;
                }

                if (flags == CD_STATUS_PAUSE) {
                        return 0;
                }

                if (flags == CD_STATUS_FATAL) {
                        return -CD_STATUS_FATAL;
                }

                if (flags == CD_STATUS_REJECT) {
                        return -CD_STATUS_REJECT;
                }
        }

        return 0;
}
