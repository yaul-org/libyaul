/*
 * Copyright (c) Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _YAUL_BUP_H_
#define _YAUL_BUP_H_

#include <sys/cdefs.h>

#include <assert.h>
#include <stdint.h>

#include <smpc/smc.h>

__BEGIN_DECLS

/// @defgroup BUP Backup (BUP)

/// @addtogroup BUP
/// @{

/// @brief Space required when using BUP functions.
/// @see bup_init
typedef struct bup {
    /// @private
    uint32_t lib[4096];
    /// @private
    uint32_t workarea[2048];
} __aligned(4) bup_t;

/// @brief Type of device.
typedef enum bup_device {
    /// @brief Internal memory.
    BUP_DEVICE_INTERNAL = 0,
    /// @brief External cartridge.
    BUP_DEVICE_EXTERNAL = 1,
    /// @brief Serial.
    BUP_DEVICE_SERIAL   = 2
} bup_device_t;

/// @brief Storage device connection information.
/// @see bup_init
typedef struct bup_config {
    /// @brief Connected device. If no device is connected, -1 is used.
    bup_device_t device;
    /// @brief Number of partitions.
    uint16_t partition_count;
} bup_config_t;

/// @brief BUP device configurations connected when BUP is initialized.
/// @see bup_init
typedef union bup_devices {
    struct {
        /// @brief Configuration for the built-in memory.
        bup_config_t internal;
        /// @brief Configuration for the cartridge.
        bup_config_t external;
        /// @brief Configuration for the serial interface.
        bup_config_t serial;
    };

    /// @brief Configurations.
    bup_config_t configs[3];
} bup_devices_t;

/// @brief Language.
typedef enum bup_language {
    BUP_LANG_JAPANESE,
    BUP_LANG_ENGLISH,
    BUP_LANG_FRENCH,
    BUP_LANG_GERMAN,
    BUP_LANG_SPANISH,
    BUP_LANG_ITALIAN
} __packed bup_language_t;

static_assert(sizeof(bup_language_t) == 1);

/// @brief Status of BUP functions.
typedef enum bup_status {
    /// @brief Success.
    BUP_OK                = 0,
    /// @brief Not connected.
    BUP_NON               = 1,
    /// @brief Not formatted.
    BUP_UNFORMAT          = 2,
    /// @brief File is write protected.
    BUP_WRITE_PROTECTED   = 3,
    /// @brief Not enough memory available.
    BUP_NOT_ENOUGH_MEMORY = 4,
    /// @brief File not found.
    BUP_NOT_FOUND         = 5,
    /// @brief File with the same name exists.
    BUP_FOUND             = 6,
    /// @brief Verification failed.
    BUP_NO_MATCH          = 7,
    /// @brief File is corrupted.
    BUP_BROKEN            = 8
} bup_status_t;

/// @brief Write mode.
/// @see bup_write
typedef enum bup_write_mode {
    /// @brief Overwrite save even if the filename of the same name exists.
    BUP_WRITE_MODE_OVERWRITE,
    /// @brief Protect save if the filename of the same name exists.
    BUP_WRITE_MODE_PROTECT
} bup_write_mode_t;

/// @brief Status information.
/// @see bup_status
typedef struct bup_stat {
    /// @brief Total size in bytes.
    uint32_t total_size;
    /// @brief Number of used blocks.
    uint32_t total_blocks;
    /// @brief Size of one block in bytes.
    uint32_t block_size;
    /// @brief Remaining bytes.
    uint32_t free_size;
    /// @brief Number of free blocks.
    uint32_t free_blocks;
    /// @brief Number of items that can be written.
    uint32_t data_num;
} bup_stat_t;

/// @brief Date and time.
/// @see bup_date_get
/// @see bup_date_set
typedef struct bup_date {
    /// @brief Year with 1980 subtracted from the year.
    uint8_t year;
    /// @brief Month in the `1` to `12` range.
    uint8_t month;
    /// @brief Day in the `1` to `31` range.
    uint8_t day;
    /// @brief Hour in the `0` to `23` range.
    uint8_t hour;
    /// @brief Minute in the `0` to `59` range.
    uint8_t minute;
    /// @brief Day of week, where Sunday is `0`, and Saturday is `6`.
    uint8_t week;
} bup_date_t;

/// @brief BUP date data type.
typedef uint32_t bup_dir_date_t;

/// @brief Directory information
typedef struct bup_directory {
    /// @brief File name. 12th byte is `\0`.
    char filename[12];
    /// @brief comment. 10th byte is `\0`.
    char comment[11];
    /// @brief Language comment.
    bup_language_t language;
    /// @brief Date and time.
    bup_dir_date_t date;
    /// @brief Size in bytes.
    size_t data_size;
    /// @brief Size in blocks.
    uint16_t block_size;
} bup_directory_t;

/// @private
typedef struct {
    unsigned int : 32;
    /// @private
    bup_status_t (
        *op_partition_select)(bup_device_t device, uint16_t partition);
    /// @private
    bup_status_t (*op_format)(bup_device_t device);
    /// @private
    bup_status_t (
        *op_status)(bup_device_t device, size_t data_size, bup_stat_t *tb);
    /// @private
    bup_status_t (*op_write)(bup_device_t device, bup_directory_t *tb,
        const void *data, uint8_t wmode);
    /// @private
    bup_status_t (
        *op_read)(bup_device_t device, const char *filename, void *data);
    /* The "op_" prefix was used to avoid conflict with C++'s reserved
     * keyword "delete" */
    /// @private
    bup_status_t (*op_delete)(bup_device_t device, const char *filename);
    /// @private
    int32_t (*op_dir)(bup_device_t device, const char *filename,
        uint16_t tb_size, bup_directory_t *tb);
    /// @private
    bup_status_t (*op_verify)(bup_device_t device, const char *filename,
        const void *data);
    /// @private
    void (*op_date_get)(bup_dir_date_t dir_date, bup_date_t *tb);
    /// @private
    bup_dir_date_t (*op_date_set)(const bup_date_t *tb);
} __packed bup_stub_t;

/// @brief Select partition.
///
/// @param device    The device.
/// @param partition The partition index.
///
/// @returns The status.
static inline bup_status_t __always_inline
bup_partition_select(bup_device_t device, uint16_t partition)
{
    extern bup_stub_t *__bup_stub;

    smpc_smc_resdisa_call();
    const bup_status_t bup_status = __bup_stub->op_partition_select(device,
        partition);
    smpc_smc_resenab_call();

    return bup_status;
}

/// @brief Format.
///
/// @param device The device.
///
/// @returns The status.
static inline bup_status_t __always_inline
bup_format(bup_device_t device)
{
    extern bup_stub_t *__bup_stub;

    smpc_smc_resdisa_call();
    const bup_status_t bup_status = __bup_stub->op_format(device);
    smpc_smc_resenab_call();

    return bup_status;
}

/// @brief Get status.
///
/// @param      device    The device.
/// @param      data_size The size of data to be written in bytes.
/// @param[out] tb        The status structure to write to.
///
/// @returns The status.
static inline bup_status_t __always_inline
bup_status(bup_device_t device, size_t data_size, bup_stat_t *tb)
{
    extern bup_stub_t *__bup_stub;

    smpc_smc_resdisa_call();
    const bup_status_t bup_status = __bup_stub->op_status(device, data_size,
        tb);
    smpc_smc_resenab_call();

    return bup_status;
}

/// @brief Write to the backup device.
///
/// @param device     The device.
/// @param tb         The directory structure where @p
///                   bup_directory_t.block_size is written.
/// @param data       The data to write.
/// @param write_mode The write mode.
///
/// @returns The status.
static inline bup_status_t __always_inline
bup_write(bup_device_t device, bup_directory_t *tb, const void *data,
    bup_write_mode_t write_mode)
{
    extern bup_stub_t *__bup_stub;

    smpc_smc_resdisa_call();
    const bup_status_t bup_status = __bup_stub->op_write(device, tb, data,
        write_mode);
    smpc_smc_resenab_call();

    return bup_status;
}

/// @brief Read data from the backup device storage.
///
/// @param      device   The device.
/// @param      filename The filename.
/// @param[out] data     The data to write to.
///
/// @returns The status.
static inline bup_status_t __always_inline
bup_read(bup_device_t device, const char *filename, void *data)
{
    extern bup_stub_t *__bup_stub;

    smpc_smc_resdisa_call();
    const bup_status_t bup_status = __bup_stub->op_read(device, filename, data);
    smpc_smc_resenab_call();

    return bup_status;
}

/// @brief Delete data from the backup device storage.
///
/// @param device   The device.
/// @param filename The filename
///
/// @returns The status.
static inline bup_status_t __always_inline
bup_delete(bup_device_t device, const char *filename)
{
    extern bup_stub_t *__bup_stub;

    smpc_smc_resdisa_call();
    const bup_status_t bup_status = __bup_stub->op_delete(device, filename);
    smpc_smc_resenab_call();

    return bup_status;
}

/// @brief Get directory information.
///
/// @param      device   The device.
/// @param      filename The filename.
/// @param      tb_size  The number of directories.
/// @param[out] tb       The directory.
///
/// @returns The number of directories matching @p filename. Otherwise, a
/// negative value is returned if @p tb_size is smaller than the number of
/// matches found. Negating the return value yields the number of directories
/// matched. For example, if `-5` is returned, then there are `5` directories.
static inline int32_t __always_inline
bup_dir(bup_device_t device, const char *filename, uint16_t tb_size,
    bup_directory_t *tb)
{
    extern bup_stub_t *__bup_stub;

    smpc_smc_resdisa_call();
    const int32_t value = __bup_stub->op_dir(device, filename, tb_size, tb);
    smpc_smc_resenab_call();

    return value;
}

/// @brief Verify data written to the backup device storage.
///
/// @param     device   The device.
/// @param     filename The filename.
/// @param[in] data     The data to verify.
///
/// @returns The status.
static inline bup_status_t __always_inline
bup_verify(bup_device_t device, const char *filename, const void *data)
{
    extern bup_stub_t *__bup_stub;

    smpc_smc_resdisa_call();
    const bup_status_t bup_status = __bup_stub->op_verify(device, filename,
        data);
    smpc_smc_resenab_call();

    return bup_status;
}

/// @brief Expands data and time data in the directory.
///
/// @param      date The data and time data from the directory.
/// @param[out] tb   The date.
///
/// @see bup_directory_t
static inline void __always_inline
bup_date_get(bup_dir_date_t date, bup_date_t *tb)
{
    extern bup_stub_t *__bup_stub;

    __bup_stub->op_date_get(date, tb);
}

/// @brief Compress date and time data.
///
/// @param[in] tb The date and time table.
///
/// @returns The data in date and data form.
static inline bup_dir_date_t __always_inline
bup_date_set(const bup_date_t *tb)
{
    extern bup_stub_t *__bup_stub;

    return __bup_stub->op_date_set(tb);
}

/// @brief Initialize the BUP.
///
/// @warning @p bup_t is 24,576 bytes. It's advisable to allocate temporary
/// memory.
///
/// @param      bup     The BUP state.
/// @param[out] devices The device configurations.
void bup_init(bup_t *bup, bup_devices_t *devices);

/// @}

__END_DECLS

#endif /* !_YAUL_BUP_H_ */
