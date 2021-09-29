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

#define SECTOR_LENGTH_2048      (0)
#define SECTOR_LENGTH_2336      (1)
#define SECTOR_LENGTH_2340      (2)
#define SECTOR_LENGTH_2352      (3)

/* Helpers */
#define FAD2LBA(x)      ((x) - 150)
#define LBA2FAD(x)      ((x) + 150)

typedef uint32_t fad_t;

typedef struct cd_block_hardware_info {
        uint8_t cd_status;
        uint8_t hw_flag;
        uint8_t hw_version;
        uint8_t mpeg_version;
        uint8_t drive_version;
        uint8_t drive_revision;
} cd_block_hardware_info_t;

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

typedef struct cd_block_status {
        uint8_t cd_status;
        uint8_t flag_and_repeat_count;
        uint8_t ctrl_addr;
        uint8_t track_number;
        uint8_t index_number;
        uint32_t fad;
} cd_block_status_t;

extern int cd_block_cmd_status_get(cd_block_status_t *cd_status);
extern int cd_block_cmd_get_hardware_info(cd_block_hardware_info_t *info);
extern int cd_block_cmd_get_toc(uint8_t *cd_status, uint16_t *tocsize);
extern int cd_block_cmd_get_session_info(uint8_t session_number, uint8_t *cd_status, uint8_t *num_sessions, uint32_t *session_lBA);
extern int cd_block_cmd_init_cd_system(int16_t standby);
extern int cd_block_cmd_open_tray(int16_t standby);
extern int cd_block_cmd_end_data_transfer(void);
extern int cd_block_cmd_play_disk(int32_t mode, uint32_t start_fad, int32_t num_sectors);
extern int cd_block_cmd_seek_disk(uint32_t start_play_pos);
extern int cd_block_cmd_scan_disk(uint8_t scan_direction, uint8_t *cd_status);
extern int cd_block_cmd_get_subcode(uint8_t type, uint8_t *cd_status, uint16_t *size_in_words, uint16_t *flags);
extern int cd_block_cmd_set_cd_device_connection(uint8_t filter);
extern int cd_block_cmd_get_cd_device_connection(uint8_t *cd_status, uint8_t *filter_num);
extern int cd_block_cmd_get_last_buffer_destination(uint8_t *cd_status, uint8_t *buff_num);
extern int cd_block_cmd_set_filter_range(uint8_t filter, uint32_t fad, uint32_t range);

extern int cd_block_cmd_set_filter_subheader_conditions(uint8_t channel, uint8_t submode_mask,
    uint8_t code_info_mask, uint8_t filter_num, uint8_t file_id, uint8_t sub_mode_val,
    uint8_t code_info_val);
extern int cd_block_cmd_set_filter_mode(uint8_t mode, uint16_t filter_num);
extern int cd_block_cmd_set_filter_connection(uint8_t conn_num, uint8_t true_conn, uint8_t false_conn, uint16_t filter_num);
extern int cd_block_cmd_reset_selector(uint8_t flags, uint8_t sel_num);
extern int cd_block_cmd_get_buffer_size(uint8_t *cd_status, uint16_t *block_free_space, uint8_t *max_selectors, uint16_t *max_blocks);
extern int cd_block_cmd_get_sector_number(uint8_t buff_num);

extern int cd_block_cmd_set_sector_length(uint8_t size);
extern int cd_block_cmd_get_sector_data(uint16_t sec_offset, uint8_t buf_num, uint16_t sec_num);
extern int cd_block_cmd_delete_sector_data(uint16_t sec_position, uint8_t buf_num, uint16_t sec_num);
extern int cd_block_cmd_get_then_delete_sector_data(uint16_t offset, uint8_t buff_num, uint16_t sec_num);
extern int cd_block_cmd_put_sector_data(uint8_t buff_num, uint16_t sec_num);
extern int cd_block_cmd_copy_sector_data(uint8_t dst_filter, uint16_t sec_offset, uint8_t buff_num, uint16_t sec_num);
extern int cd_block_cmd_move_sector_data(uint8_t dst_filter, uint16_t sec_offset, uint8_t buff_num, uint16_t sec_num);

extern int cd_block_cmd_abort_file(void);
extern int cd_block_cmd_auth_disk(void);
extern int cd_block_cmd_is_auth(uint16_t *disk_type_auth);

__END_DECLS

#endif /* !_CD_BLOCK_CMD_H_ */
