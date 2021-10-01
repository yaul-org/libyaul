/*
 * See LICENSE for details.
 */

#ifndef _KERNEL_FS_ISO9660_INTERNAL_H_
#define _KERNEL_FS_ISO9660_INTERNAL_H_

#include <sys/types.h>
#include <sys/cdefs.h>

#include <stdint.h>

/* Volume descriptor types */
#define ISO_VD_BOOT_RECORD      0   /* Boot record */
#define ISO_VD_PRIMARY          1   /* Primary volume descriptor */
#define ISO_VD_SUPPLEMENTARY    2   /* Supplementary volume descriptor */
#define ISO_VD_VOLUME_PARTITION 3   /* Volume partition descriptor */
#define ISO_VD_SET_TERMINATOR   255 /* Volume descriptor set terminator */

#define ISO_STANDARD_ID         "CD001"
#define ISO_ECMA_ID             "CDW01"

#define DIRENT_FILE_FLAGS_FILE                  0x00
#define DIRENT_FILE_FLAGS_HIDDEN                0x01
#define DIRENT_FILE_FLAGS_DIRECTORY             0x02
#define DIRENT_FILE_FLAGS_ASSOCIATED_FILE       0x04
#define DIRENT_FILE_FLAGS_RECORD                0x08
#define DIRENT_FILE_FLAGS_PROTECTION            0x10
#define DIRENT_FILE_FLAGS_MULTI_EXTENT          0x80

#define ISODCL(from, to) ((to) - (from) + 1)

typedef struct {
        char type[ISODCL(1,1)];        /* 711 */
        uint8_t id[ISODCL(2, 6)];
        uint8_t version[ISODCL(7, 7)];
        uint8_t data[ISODCL(8, 2048)];
} iso9660_vd_t;

typedef struct iso9660_pvd {
        uint8_t type[ISODCL(1, 1)];                       /* 711 */
        uint8_t id[ISODCL(2, 6)];
        uint8_t version[ISODCL(7, 7)];                    /* 711 */
        uint8_t unused_1[ISODCL(8, 8)];
        uint8_t system_id[ISODCL(9, 40)];                 /* achars */
        uint8_t volume_id[ISODCL(41, 72)];                /* dchars */
        uint8_t unused_2[ISODCL(73, 80)];
        uint8_t volume_space_size[ISODCL(81, 88)];        /* 733 */
        uint8_t unused_3[ISODCL(89, 120)];
        uint8_t volume_set_size[ISODCL(121, 124)];        /* 723 */
        uint8_t volume_sequence_number[ISODCL(125, 128)]; /* 723 */
        uint8_t logical_block_size[ISODCL(129, 132)];     /* 723 */
        uint8_t path_table_size[ISODCL(133, 140)];        /* 733 */
        uint8_t type_l_path_table[ISODCL(141, 144)];      /* 731 */
        uint8_t opt_type_l_path_table[ISODCL(145, 148)];  /* 731 */
        uint8_t type_m_path_table[ISODCL(149, 152)];      /* 732 */
        uint8_t opt_type_m_path_table[ISODCL(153, 156)];  /* 732 */
        uint8_t root_directory_record[ISODCL(157, 190)];  /* 9.1 */
        uint8_t volume_set_id[ISODCL(191, 318)];          /* dchars */
        uint8_t publisher_id[ISODCL(319, 446)];           /* achars */
        uint8_t preparer_id[ISODCL(447, 574)];            /* achars */
        uint8_t application_id[ISODCL(575, 702)];         /* achars */
        uint8_t copyright_file_id[ISODCL(703, 739)];      /* 7.5 dchars */
        uint8_t abstract_file_id[ISODCL(740, 776)];       /* 7.5 dchars */
        uint8_t bibliographic_file_id[ISODCL(777, 813)];  /* 7.5 dchars */
        uint8_t creation_date[ISODCL(814, 830)];          /* 8.4.26.1 */
        uint8_t modification_date[ISODCL(831, 847)];      /* 8.4.26.1 */
        uint8_t expiration_date[ISODCL(848, 864)];        /* 8.4.26.1 */
        uint8_t effective_date[ISODCL(865, 881)];         /* 8.4.26.1 */
        uint8_t file_structure_version[ISODCL(882, 882)]; /* 711 */
        uint8_t unused_4[ISODCL(883, 883)];
        uint8_t application_data[ISODCL(884, 1395)];
        uint8_t unused_5[ISODCL(1396, 2048)];
} __packed iso9660_pvd_t;

typedef struct {
        uint8_t length[ISODCL(1, 1)];                   /* 711 */
        uint8_t ext_attr_length[ISODCL(2, 2)];          /* 711 */
        uint8_t extent[ISODCL(3, 10)];                  /* 733 */
        uint8_t data_length[ISODCL(11, 18)];            /* 733 */
        uint8_t date[ISODCL(19, 25)];                   /* 7 by 711 */
        uint8_t file_flags[ISODCL(26, 26)];             /* 711 */
        uint8_t file_unit_size[ISODCL(27, 27)];         /* 711 */
        uint8_t interleave[ISODCL(28, 28)];             /* 711 */
        uint8_t volume_sequence_number[ISODCL(29, 32)]; /* 723 */
        uint8_t file_id_len[ISODCL(33, 33)];            /* 711 */
        uint8_t name[ISODCL(34, 34)];                   /* 711 */
} __packed iso9660_dirent_t;

#undef ISODCL

static inline __always_inline uint16_t
be16dec(const void *buf)
{
        const uint8_t *p;
        p = (const uint8_t *)buf;

        return (uint16_t)((p[0] << 8) | p[1]);
}

static inline __always_inline uint32_t
be32dec(const void *buf)
{
        const uint8_t *p;
        p = (const uint8_t *)buf;

        return ((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

static inline __always_inline uint16_t
le16dec(const void *buf)
{
        uint8_t const *p;
        p = (uint8_t const *)buf;

        return ((p[1] << 8) | p[0]);
}

static inline __always_inline uint32_t
le32dec(const void *buf)
{
        uint8_t const *p;
        p = (uint8_t const *)buf;

        return ((p[3] << 24) | (p[2] << 16) | (p[1] << 8) | p[0]);
}

/* 7.1.1: uint8_t */
static inline __always_inline uint8_t
isonum_711(const uint8_t *p)
{
        return *p;
}

/* 7.2.3: unsigned both-endian (little, then big) 16-bit value */
static inline __always_inline uint16_t
isonum_723(const uint8_t *p)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return be16dec(p + 2);
#else
        return le16dec(p);
#endif /* __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ */
}

/* 7.3.3: unsigned both-endian (little, then big) 32-bit value */
static inline __always_inline __unused uint32_t
isonum_733(const uint8_t *p)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return be32dec(p + 4);
#else
        return le32dec(p);
#endif /* __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ */
}

#endif /* !_KERNEL_FS_ISO9660_INTERNAL_H_ */
