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

static void
_hirq_flag_wait(uint16_t flag)
{
        while ((MEMORY_READ(16, CD_BLOCK(HIRQ)) & flag) == 0) {
        }
}

static int
_return_status_check(cd_block_regs_t *status)
{
        assert(status != NULL);

        uint8_t status_flags;

        status_flags = status->cr1 & 0xFF;

        if (status_flags & CD_STATUS_FATAL) {
                return -CD_STATUS_FATAL;
        } else if (status_flags & CD_STATUS_ERROR) {
                return -CD_STATUS_ERROR;
        } else if (status_flags & CD_STATUS_NO_DISC) {
                return -CD_STATUS_NO_DISC;
        } else if (status_flags & CD_STATUS_OPEN) {
                return -CD_STATUS_OPEN;
        } else {
                return 0;
        }
}

int
cd_block_cmd_status_get(cd_block_status_t *cd_status)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0000;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (cd_status != NULL) {
                cd_status->cd_status = status.cr1 >> 8;
                cd_status->flag_and_repeat_count = status.cr1 & 0xFF;
                cd_status->ctrl_addr = status.cr2 >> 8;
                cd_status->track_number = status.cr2 & 0xFF;
                cd_status->index_number = status.cr3 >> 8;
                cd_status->fad = ((status.cr3 & 0xFF) << 16) | status.cr4;
        }

        return 0;
}

int
cd_block_cmd_hw_info_get(cd_block_hardware_info_t *info)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0100;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (info != NULL) {
                info->cd_status = status.cr1 >> 8;
                info->hw_flag = status.cr2 >> 8;
                info->hw_version = status.cr2 & 0xFF;
                info->mpeg_version = status.cr3 & 0xFF;
                info->drive_version = status.cr4 >> 8;
                info->drive_revision = status.cr4 & 0xFF;
        }

        return 0;
}

int
cd_block_cmd_toc_get(uint8_t *cd_status, uint16_t *tocsize)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0200;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        _hirq_flag_wait(DRDY);

        if (cd_status != NULL) {
                *cd_status = status.cr1 >> 8;
        }

        if (tocsize != NULL) {
                *tocsize = status.cr2;
        }

        return 0;
}

int
cd_block_cmd_session_info_get(uint8_t session_num __unused,
    uint8_t *cd_status,
    uint8_t *session_count,
    uint32_t *session_lba)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0300;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (cd_status != NULL) {
                *cd_status = status.cr1 >> 8;
        }

        if (session_count != NULL) {
                *session_count = status.cr3 >> 8;
        }

        if (session_lba != NULL) {
                *session_lba = ((status.cr3 & 0xFF) << 16) | status.cr4;
        }

        return 0;
}

int
cd_block_cmd_cd_system_init(int16_t standby)
{
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0400;
        regs.cr2 = standby;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x040F;

        return cd_block_cmd_execute(&regs, &status);
}

int
cd_block_cmd_tray_open(int16_t standby __unused)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0500;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_data_transfer_end(void)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x0600;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        MEMORY_WRITE_AND(16, CD_BLOCK(HIRQ), ~DRDY);
        MEMORY_WRITE_OR(16, CD_BLOCK(HIRQ), CMOK);

        return 0;
}

int
cd_block_cmd_disk_play(int32_t mode, fad_t start_fad, int32_t num_sectors)
{
        /* Clear flags */
        MEMORY_WRITE_AND(16, CD_BLOCK(HIRQ), ~(PEND | CSCT));
        MEMORY_WRITE_OR(16, CD_BLOCK(HIRQ), CMOK);

        cd_block_regs_t status;
        cd_block_regs_t regs;

        /* The OR on 0x80 for lower byte happens so the system interprets our
         * data as FAD position instead of Track number */
        regs.hirq_mask = 0;
        regs.cr1 = 0x1080 | (start_fad >> 16);
        regs.cr2 = start_fad;
        regs.cr3 = (mode << 8) | 0x80 | (num_sectors >> 16);
        regs.cr4 = num_sectors;

        int ret;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_disk_seek(uint32_t start_play_pos)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x1180 | ((start_play_pos >> 16) & 0xFF);
        regs.cr2 = (start_play_pos & 0xFFFF);
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_disk_scan(uint8_t scan_direction, uint8_t *cd_status)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x1200 | scan_direction;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (cd_status != NULL) {
                *cd_status = status.cr1 >> 8;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_subcode_get(uint8_t type, uint8_t *cd_status,
    uint16_t *size_in_words, uint16_t *flags)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x2000 | type;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (cd_status != NULL) {
                *cd_status = status.cr1 >> 8;
        }

        if (size_in_words != NULL) {
                *size_in_words = status.cr2;
        }

        if (flags != NULL) {
                *flags = status.cr4;
        }

        return 0;
}

int
cd_block_cmd_cd_dev_connection_set(uint8_t filter)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = ESEL;
        regs.cr1 = 0x3000;
        regs.cr2 = 0x0000;
        regs.cr3 = (filter << 8);
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_cd_dev_connection_get(uint8_t *cd_status, uint8_t *filter_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x3100;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (cd_status != NULL) {
                *cd_status = status.cr1 >> 8;
        }

        if (filter_num != NULL) {
                *filter_num = status.cr3 >> 8;
        }

        return 0;
}

int
cd_block_cmd_last_buffer_destination_get(uint8_t *cd_status, uint8_t *buff_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x3200;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (cd_status != NULL) {
                *cd_status = status.cr1 >> 8;
        }

        if (buff_num != NULL) {
                *buff_num = status.cr3 >> 8;
        }

        return 0;
}

int
cd_block_cmd_filter_range_set(uint8_t filter, fad_t fad, uint32_t range)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x4000 | (fad >> 16);
        regs.cr2 = (fad & 0xFFFF);
        regs.cr3 = (filter << 8) | ((range >> 16) & 0xFF);
        regs.cr4 = (range & 0xFFFF);

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        _hirq_flag_wait(ESEL);

        return _return_status_check(&status);
}

/* TODO:
 * Get Filter Range                0x41
 * Get Filter Subheader Conditions 0x43
 * Get Filter Mode                 0x45
 * Set Filter Connection           0x46
 * Get Filter Connection           0x47 */

int
cd_block_cmd_filter_subheader_conditions_set(uint8_t channel, uint8_t submode_mask,
    uint8_t code_info_mask, uint8_t filter_num, uint8_t file_id, uint8_t sub_mode_val,
    uint8_t code_info_val)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x4200 | channel;
        regs.cr2 = (submode_mask << 8) | code_info_mask;
        regs.cr3 = (filter_num << 8) | file_id;
        regs.cr4 = (sub_mode_val << 8) | code_info_val;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        _hirq_flag_wait(ESEL);

        return _return_status_check(&status);
}

int
cd_block_cmd_filter_mode_set(uint8_t mode, uint16_t filter_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x4400 | mode;
        regs.cr2 = 0x0000;
        regs.cr3 = filter_num;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        _hirq_flag_wait(ESEL);

        return _return_status_check(&status);
}

int
cd_block_cmd_filter_connection_set(uint8_t conn_num, uint8_t true_conn, uint8_t false_conn, uint16_t filter_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x4600 | conn_num;
        regs.cr2 = (true_conn << 8) | false_conn;
        regs.cr3 = filter_num;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        _hirq_flag_wait(ESEL);

        return _return_status_check(&status);
}

int
cd_block_cmd_selector_reset(uint8_t flags, uint8_t sel_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = EFLS;
        regs.cr1 = 0x4800 | flags;
        regs.cr2 = 0x0000;
        regs.cr3 = (sel_num << 8);
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        _hirq_flag_wait(ESEL);

        return _return_status_check(&status);
}

int
cd_block_cmd_buffer_size_get(uint8_t *cd_status, uint16_t *block_free_space,
    uint8_t *max_selectors, uint16_t *max_blocks)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x5000;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (cd_status != NULL) {
                *cd_status = status.cr1 >> 8;
        }

        if (block_free_space != NULL) {
                *block_free_space = status.cr2;
        }

        if (max_selectors != NULL) {
                *max_selectors = status.cr3 >> 8;
        }

        if (max_blocks != NULL) {
                *max_blocks = status.cr4;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_sector_number_get(uint8_t buffer_number)
{
        cd_block_regs_t regs;

        regs.hirq_mask = 0;
        regs.cr1 = 0x5100;
        regs.cr2 = 0x0000;
        regs.cr3 = (buffer_number << 8);
        regs.cr4 = 0x0000;

        cd_block_regs_t status;

        int ret;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return 0;
        }

        return status.cr4;
}

/* TODO:
 * Calculate Actual Size  0x52
 * Get Actual Size        0x53
 * Get Sector Info        0x54
 * Execute FAD Search     0x55
 * Get FAD Search Results 0x56 */

int
cd_block_cmd_sector_length_set(uint8_t size)
{
        cd_block_regs_t regs;

        regs.hirq_mask = ESEL;
        regs.cr1 = 0x6000 | (size & 0xFF);
        regs.cr2 = (size << 8);
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        cd_block_regs_t status;

        int ret;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_sector_data_get(uint16_t sec_offset, uint8_t buf_num,
    uint16_t sec_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x6100;
        regs.cr2 = sec_offset;
        regs.cr3 = buf_num << 8;
        regs.cr4 = sec_num;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_sector_data_delete(uint16_t sec_position, uint8_t buf_num,
    uint16_t sec_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = EHST;
        regs.cr1 = 0x6200;
        regs.cr2 = sec_position;
        regs.cr3 = buf_num << 8;
        regs.cr4 = sec_num;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_sector_data_get_delete(uint16_t offset, uint8_t buff_num,
    uint16_t sec_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = EHST;
        regs.cr1 = 0x6300;
        regs.cr2 = offset;
        regs.cr3 = (buff_num << 8);
        regs.cr4 = sec_num;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_sector_data_put(uint8_t buff_num, uint16_t sec_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = EHST;
        regs.cr1 = 0x6400;
        regs.cr2 = 0x0000;
        regs.cr3 = (buff_num << 8);
        regs.cr4 = sec_num;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_sector_data_copy(uint8_t dst_filter, uint16_t sec_offset,
    uint8_t buff_num, uint16_t sec_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x6500 | dst_filter;
        regs.cr2 = sec_offset;
        regs.cr3 = (buff_num << 8);
        regs.cr4 = sec_num;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_sector_data_move(uint8_t dst_filter, uint16_t sec_offset,
    uint8_t buff_num, uint16_t sec_num)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x6600 | dst_filter;
        regs.cr2 = sec_offset;
        regs.cr3 = (buff_num << 8);
        regs.cr4 = sec_num;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_get_copy_error(uint8_t *cd_status, uint8_t *error_code)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0x6700;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (cd_status != NULL) {
                *cd_status = status.cr1 >> 8;
        }

        if (error_code != NULL) {
                *error_code = status.cr1 & 0xFF;
        }

        return 0;
}

/* TODO:
 * Change Directory      0x70
 * Read Directory        0x71
 * Get File System Scope 0x72
 * Get File Info         0x73
 * Read File             0x74 */

int
cd_block_cmd_file_abort(void)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        /* Abort file */
        regs.hirq_mask = EFLS;
        regs.cr1 = 0x7500;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        return _return_status_check(&status);
}

int
cd_block_cmd_disk_auth(void)
{
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = EFLS;
        regs.cr1 = 0xE000;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        return cd_block_cmd_execute(&regs, &status);
}

int
cd_block_cmd_auth_check(uint16_t *disk_type_auth)
{
        int ret;
        cd_block_regs_t regs;
        cd_block_regs_t status;

        regs.hirq_mask = 0;
        regs.cr1 = 0xE100;
        regs.cr2 = 0x0000;
        regs.cr3 = 0x0000;
        regs.cr4 = 0x0000;

        if ((ret = cd_block_cmd_execute(&regs, &status)) != 0) {
                return ret;
        }

        if (disk_type_auth != NULL) {
                *disk_type_auth = status.cr2;
        }

        /* Disc type Authenticated:
         *   0x00: No CD or not authenticated
         *   0x01: Audio CD
         *   0x02: Regular data CD(not Saturn disc)
         *   0x03: Copied or pirated Saturn disc
         *   0x04: Original Saturn disc */
        return ((status.cr2 != 0x00) && (status.cr2 != 0x03));
}
