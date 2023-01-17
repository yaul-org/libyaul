/*
 * Copyright (c) 2012-2019 Israel Jacquez
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

typedef struct bup {
        uint32_t lib[4096];
        uint32_t workarea[2048];
} __aligned(4) bup_t;

typedef enum bup_device {
        BUP_DEVICE_INTERNAL = 0,
        BUP_DEVICE_EXTERNAL = 1
} bup_device_t;

typedef enum bup_language {
        BUP_LANG_JAPANESE,
        BUP_LANG_ENGLISH,
        BUP_LANG_FRENCH,
        BUP_LANG_GERMAN,
        BUP_LANG_SPANISH,
        BUP_LANG_ITALIAN
} __packed bup_language_t;

static_assert(sizeof(bup_language_t) == 1);

typedef enum bup_status {
        BUP_OK                 = 0,
        BUP_NON                = 1,
        BUP_UNFORMAT           = 2,
        BUP_WRITE_PROTECT      = 3,
        BUP_NOT_ENOUGH_MEMORY  = 4,
        BUP_NOT_FOUND          = 5,
        BUP_FOUND              = 6,
        BUP_NO_MATCH           = 7,
        BUP_BROKEN             = 8
} bup_status_t;

typedef struct bup_config {
        uint16_t unit_id;
        uint16_t partition;
} bup_config_t;

typedef struct bup_stat {
        uint32_t total_size;
        uint32_t total_blocks;
        uint32_t block_size;
        uint32_t free_size;
        uint32_t free_blocks;
        uint32_t data_num;
} bup_stat_t;

typedef struct bup_date {
        uint8_t year;
        uint8_t month;
        uint8_t day;
        uint8_t hour;
        uint8_t minute;
        uint8_t week;
} bup_date_t;

typedef uint32_t bup_dir_date_t;

typedef struct bup_directory {
        char filename[12];
        char comment[11];
        bup_language_t language;
        bup_dir_date_t date;
        uint32_t data_size;
        uint16_t block_size;
} bup_directory_t;

typedef struct {
        unsigned int :32;
        bup_status_t (*partition_select)(bup_device_t device, uint16_t partition);
        bup_status_t (*format)(bup_device_t device);
        bup_status_t (*status)(bup_device_t device, size_t data_size, bup_stat_t *tb);
        bup_status_t (*write)(bup_device_t device, bup_directory_t *tb, const void *data, uint8_t wmode);
        bup_status_t (*read)(bup_device_t device, const char *filename, void *data);
        bup_status_t (*delete)(bup_device_t device, const char *filename);
        bup_status_t (*dir)(bup_device_t device, const char *filename, uint16_t tb_size, bup_directory_t *tb);
        bup_status_t (*verify)(bup_device_t device, const char *filename, const void *data);
        void (*date_get)(bup_dir_date_t dir_date, bup_date_t *tb);
        bup_dir_date_t (*date_set)(const bup_date_t *tb);
} __packed bup_stub_t;

static inline bup_status_t __always_inline
bup_partition_select(bup_device_t device, uint16_t partition)
{
        extern bup_stub_t *__bup_stub;

        smpc_smc_resdisa_call();
        const bup_status_t bup_status = __bup_stub->partition_select(device, partition);
        smpc_smc_resenab_call();

        return bup_status;
}

static inline bup_status_t __always_inline
bup_format(bup_device_t device)
{
        extern bup_stub_t *__bup_stub;

        smpc_smc_resdisa_call();
        const bup_status_t bup_status = __bup_stub->format(device);
        smpc_smc_resenab_call();

        return bup_status;
}

static inline bup_status_t __always_inline
bup_status(bup_device_t device, size_t data_size, bup_stat_t *tb)
{
        extern bup_stub_t *__bup_stub;

        smpc_smc_resdisa_call();
        const bup_status_t bup_status = __bup_stub->status(device, data_size, tb);
        smpc_smc_resenab_call();

        return bup_status;
}

static inline bup_status_t __always_inline
bup_write(bup_device_t device, bup_directory_t *tb, const void *data, uint8_t wmode)
{
        extern bup_stub_t *__bup_stub;

        smpc_smc_resdisa_call();
        const bup_status_t bup_status = __bup_stub->write(device, tb, data, wmode);
        smpc_smc_resenab_call();

        return bup_status;
}

static inline bup_status_t __always_inline
bup_read(bup_device_t device, const char *filename, void *data)
{
        extern bup_stub_t *__bup_stub;

        smpc_smc_resdisa_call();
        const bup_status_t bup_status = __bup_stub->read(device, filename, data);
        smpc_smc_resenab_call();

        return bup_status;
}

static inline bup_status_t __always_inline
bup_delete(bup_device_t device, const char *filename)
{
        extern bup_stub_t *__bup_stub;

        smpc_smc_resdisa_call();
        const bup_status_t bup_status = __bup_stub->delete(device, filename);
        smpc_smc_resenab_call();

        return bup_status;
}

static inline bup_status_t __always_inline
bup_dir(bup_device_t device, const char *filename, uint16_t tb_size, bup_directory_t *tb)
{
        extern bup_stub_t *__bup_stub;

        smpc_smc_resdisa_call();
        const bup_status_t bup_status = __bup_stub->dir(device, filename, tb_size, tb);
        smpc_smc_resenab_call();

        return bup_status;
}

static inline bup_status_t __always_inline
bup_verify(bup_device_t device, const char *filename, const void *data)
{
        extern bup_stub_t *__bup_stub;

        smpc_smc_resdisa_call();
        const bup_status_t bup_status = __bup_stub->verify(device, filename, data);
        smpc_smc_resenab_call();

        return bup_status;
}

static inline void __always_inline
bup_date_get(bup_dir_date_t date, bup_date_t *tb)
{
        extern bup_stub_t *__bup_stub;

        __bup_stub->date_get(date, tb);
}

static inline bup_dir_date_t __always_inline
bup_date_set(const bup_date_t *tb)
{
        extern bup_stub_t *__bup_stub;

        return __bup_stub->date_set(tb);
}

void bup_init(bup_t *bup, bup_config_t configs[3]);

__END_DECLS

#endif /* !_YAUL_BUP_H_ */
