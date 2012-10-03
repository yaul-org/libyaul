/*
 * See LICENSE for details.
 */

/* Definitions describing ISO9660 file system structure, as well as the
 * functions necessary to access fields of ISO9660 file system
 * structures */

#ifndef _ISO9660_INTERNAL_H_
#define _ISO9660_INTERNAL_H_

#include <inttypes.h>

#include <common/endian.h>

#define ISODCL(from, to)        ((to) - (from) + 1)

struct iso_volume_descriptor {
        char type[ISODCL(1,1)]; /* 711 */
        uint8_t id[ISODCL(2, 6)];
        uint8_t version[ISODCL(7, 7)];
        uint8_t data[ISODCL(8, 2048)];
};

/* Volume descriptor types */
#define ISO_VD_PRIMARY          1
#define ISO_VD_SUPPLEMENTARY    2
#define ISO_VD_END              255

#define ISO_STANDARD_ID         "CD001"
#define ISO_ECMA_ID             "CDW01"

#define ISO_MAXNAMLEN           255

struct iso_primary_descriptor {
        char type[ISODCL(1, 1)]; /* 711 */
        char id[ISODCL(2, 6)];
        char version[ISODCL(7, 7)]; /* 711 */
        char unused_1[ISODCL(8, 8)];
        char system_id[ISODCL(9, 40)]; /* achars */
        char volume_id[ISODCL(41, 72)]; /* dchars */
        char unused_2[ISODCL(73, 80)];
        char volume_space_size[ISODCL(81, 88)]; /* 733 */
        char unused_3[ISODCL(89, 120)];
        char volume_set_size[ISODCL(121, 124)]; /* 723 */
        char volume_sequence_number[ISODCL(125, 128)]; /* 723 */
        uint8_t logical_block_size[ISODCL(129, 132)]; /* 723 */
        char path_table_size[ISODCL(133, 140)]; /* 733 */
        char type_l_path_table[ISODCL(141, 144)]; /* 731 */
        char opt_type_l_path_table[ISODCL(145, 148)]; /* 731 */
        char type_m_path_table[ISODCL(149, 152)]; /* 732 */
        char opt_type_m_path_table[ISODCL(153, 156)]; /* 732 */
        char root_directory_record[ISODCL(157, 190)]; /* 9.1 */
        char volume_set_id[ISODCL(191, 318)]; /* dchars */
        char publisher_id[ISODCL(319, 446)]; /* achars */
        char preparer_id[ISODCL(447, 574)]; /* achars */
        char application_id[ISODCL(575, 702)]; /* achars */
        char copyright_file_id[ISODCL(703, 739)]; /* 7.5 dchars */
        char abstract_file_id[ISODCL(740, 776)]; /* 7.5 dchars */
        char bibliographic_file_id[ISODCL(777, 813)]; /* 7.5 dchars */
        char creation_date[ISODCL(814, 830)]; /* 8.4.26.1 */
        char modification_date[ISODCL(831, 847)]; /* 8.4.26.1 */
        char expiration_date[ISODCL(848, 864)]; /* 8.4.26.1 */
        char effective_date[ISODCL(865, 881)]; /* 8.4.26.1 */
        char file_structure_version[ISODCL(882, 882)]; /* 711 */
        char unused_4[ISODCL(883, 883)];
        char application_data[ISODCL(884, 1395)];
        char unused_5[ISODCL(1396, 2048)];
};

#define ISO_DEFAULT_BLOCK_SIZE  2048

struct iso_supplementary_descriptor {
        char type[ISODCL(1, 1)]; /* 711 */
        char id[ISODCL(2, 6)];
        char version[ISODCL(7, 7)]; /* 711 */
        char flags[ISODCL(8, 8)]; /* 711? */
        char system_id[ISODCL(9, 40)]; /* achars */
        char volume_id[ISODCL(41, 72)]; /* dchars */
        char unused_2[ISODCL(73, 80)];
        char volume_space_size[ISODCL(81, 88)]; /* 733 */
        char escape[ISODCL(89, 120)];
        char volume_set_size[ISODCL(121, 124)]; /* 723 */
        char volume_sequence_number[ISODCL(125, 128)]; /* 723 */
        uint8_t logical_block_size[ISODCL(129, 132)]; /* 723 */
        char path_table_size[ISODCL(133, 140)]; /* 733 */
        char type_l_path_table[ISODCL(141, 144)]; /* 731 */
        char opt_type_l_path_table[ISODCL(145, 148)]; /* 731 */
        char type_m_path_table[ISODCL(149, 152)]; /* 732 */
        char opt_type_m_path_table[ISODCL(153, 156)]; /* 732 */
        char root_directory_record[ISODCL(157, 190)]; /* 9.1 */
        char volume_set_id[ISODCL(191, 318)]; /* dchars */
        char publisher_id[ISODCL(319, 446)]; /* achars */
        char preparer_id[ISODCL(447, 574)]; /* achars */
        char application_id[ISODCL(575, 702)]; /* achars */
        char copyright_file_id[ISODCL(703, 739)]; /* 7.5 dchars */
        char abstract_file_id[ISODCL(740, 776)]; /* 7.5 dchars */
        char bibliographic_file_id[ISODCL(777, 813)]; /* 7.5 dchars */
        char creation_date[ISODCL(814, 830)]; /* 8.4.26.1 */
        char modification_date[ISODCL(831, 847)]; /* 8.4.26.1 */
        char expiration_date[ISODCL(848, 864)]; /* 8.4.26.1 */
        char effective_date[ISODCL(865, 881)]; /* 8.4.26.1 */
        char file_structure_version[ISODCL(882, 882)]; /* 711 */
        char unused_4[ISODCL(883, 883)];
        char application_data[ISODCL(884, 1395)];
        char unused_5[ISODCL(1396, 2048)];
};

typedef struct {
        char length[ISODCL(1, 1)]; /* 711 */
        char ext_attr_length[ISODCL(2, 2)]; /* 711 */
        uint8_t extent[ISODCL(3, 10)]; /* 733 */
        uint8_t size[ISODCL(11, 18)]; /* 733 */
        char date[ISODCL(19, 25)]; /* 7 by 711 */
#define FLAGS_EXISTENCE         0x01
#define FLAGS_DIRECTORY         0x02
#define FLAGS_ASSOCIATED_FILE   0x04
#define FLAGS_RECORD            0x08
#define FLAGS_PROTECTION        0x10
        char flags[ISODCL(26, 26)];
        char file_unit_size[ISODCL(27, 27)]; /* 711 */
        char interleave[ISODCL(28, 28)]; /* 711 */
        char volume_sequence_number[ISODCL(29, 32)]; /* 723 */
        char name_len[ISODCL(33, 33)]; /* 711 */
        char name[1]; /* XXX */
} iso9660_dirent_t;

/* Can't take size of 'iso_directory_record', because of possible
 * alignment of the last entry (34 instead of 33) */
#define ISO_DIRECTORY_RECORD_SIZE       33

struct iso_extended_attributes {
        uint8_t owner[ISODCL(1, 4)]; /* 723 */
        uint8_t group[ISODCL(5, 8)]; /* 723 */
        uint8_t perm[ISODCL(9, 10)]; /* 9.5.3 */
        char ctime[ISODCL(11, 27)]; /* 8.4.26.1 */
        char mtime[ISODCL(28, 44)]; /* 8.4.26.1 */
        char xtime[ISODCL(45, 61)]; /* 8.4.26.1 */
        char ftime[ISODCL(62, 78)]; /* 8.4.26.1 */
        char recfmt[ISODCL(79, 79)]; /* 711 */
        char recattr[ISODCL(80, 80)]; /* 711 */
        uint8_t reclen[ISODCL(81, 84)]; /* 723 */
        char system_id[ISODCL(85, 116)]; /* achars */
        char system_use[ISODCL(117, 180)];
        char version[ISODCL(181, 181)]; /* 711 */
        char len_esc[ISODCL(182, 182)]; /* 711 */
        char reserved[ISODCL(183, 246)];
        uint8_t len_au[ISODCL(247, 250)]; /* 723 */
};

/* 7.1.1: uint8_t */
static __inline int32_t
isonum_711(const char *p)
{
        return (int32_t)*p;
}

/* 7.1.2: signed char */
static __inline __attribute__ ((unused)) uint32_t
isonum_712(const uint8_t *p)
{
        return (uint32_t)*p;
}

/* 7.2.1: unsigned little-endian 16-bit value */
static __inline __attribute__ ((unused)) uint16_t
isonum_721(const uint8_t *p __attribute__ ((unused)))
{
        return 0x0000;
}

/* 7.2.2: unsigned big-endian 16-bit value */
static __inline __attribute__ ((unused)) uint16_t
isonum_722(const uint8_t *p __attribute__ ((unused)))
{
        return 0x0000;
}

/* 7.2.3: unsigned both-endian (little, then big) 16-bit value */
static __inline uint16_t
isonum_723(const uint8_t *p)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return be16dec(p + 2);
#else
        return le16dec(p);
#endif /* __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ */
}

/* 7.3.1: unsigned little-endian 32-bit value */
static __inline __attribute__ ((unused)) uint32_t
isonum_731(const uint8_t *p __attribute__ ((unused)))
{
        return 0;
}

/* 7.3.2: unsigned big-endian 32-bit value */
static __inline __attribute__ ((unused)) uint32_t
isonum_732(const uint8_t *p __attribute__ ((unused)))
{
        return 0;
}

/* 7.3.3: unsigned both-endian (little, then big) 32-bit value */
static __inline __attribute__ ((unused)) uint32_t
isonum_733(const uint8_t *p)
{
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        return be32dec(p + 4);
#else
        return le32dec(p);
#endif /* __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ */
}

/* Associated files have a leading '=' */
#define ASSOC_CHAR      '='

#endif /* _ISO9660_INTERNAL_H_ */
