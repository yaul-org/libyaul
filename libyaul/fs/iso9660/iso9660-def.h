/*
 * See LICENSE for details.
 */

/* Definitions describing ISO9660 file system structure, as well as the
 * functions necessary to access fields of ISO9660 file system
 * structures */

#ifndef _ISO9660_DEF_H_
#define _ISO9660_DEF_H_

#include <inttypes.h>

#define ISODCL(from, to)        ((to) - (from) + 1)

struct iso_volume_descriptor {
        uint8_t type[ISODCL(1,1)]; /* 711 */
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
        int8_t type[ISODCL(1, 1)]; /* 711 */
        int8_t id[ISODCL(2, 6)];
        int8_t version[ISODCL(7, 7)]; /* 711 */
        int8_t unused_1[ISODCL(8, 8)];
        int8_t system_id[ISODCL(9, 40)]; /* aint8_ts */
        int8_t volume_id[ISODCL(41, 72)]; /* dint8_ts */
        int8_t unused_2[ISODCL(73, 80)];
        int8_t volume_space_size[ISODCL(81, 88)]; /* 733 */
        int8_t unused_3[ISODCL(89, 120)];
        int8_t volume_set_size[ISODCL(121, 124)]; /* 723 */
        int8_t volume_sequence_number[ISODCL(125, 128)]; /* 723 */
        int8_t logical_block_size[ISODCL(129, 132)]; /* 723 */
        int8_t path_table_size[ISODCL(133, 140)]; /* 733 */
        int8_t type_l_path_table[ISODCL(141, 144)]; /* 731 */
        int8_t opt_type_l_path_table[ISODCL(145, 148)]; /* 731 */
        int8_t type_m_path_table[ISODCL(149, 152)]; /* 732 */
        int8_t opt_type_m_path_table[ISODCL(153, 156)]; /* 732 */
        int8_t root_directory_record[ISODCL(157, 190)]; /* 9.1 */
        int8_t volume_set_id[ISODCL(191, 318)]; /* dint8_ts */
        int8_t publisher_id[ISODCL(319, 446)]; /* aint8_ts */
        int8_t preparer_id[ISODCL(447, 574)]; /* aint8_ts */
        int8_t application_id[ISODCL(575, 702)]; /* aint8_ts */
        int8_t copyright_file_id[ISODCL(703, 739)]; /* 7.5 dint8_ts */
        int8_t abstract_file_id[ISODCL(740, 776)]; /* 7.5 dint8_ts */
        int8_t bibliographic_file_id[ISODCL(777, 813)]; /* 7.5 dint8_ts */
        int8_t creation_date[ISODCL(814, 830)]; /* 8.4.26.1 */
        int8_t modification_date[ISODCL(831, 847)]; /* 8.4.26.1 */
        int8_t expiration_date[ISODCL(848, 864)]; /* 8.4.26.1 */
        int8_t effective_date[ISODCL(865, 881)]; /* 8.4.26.1 */
        int8_t file_structure_version[ISODCL(882, 882)]; /* 711 */
        int8_t unused_4[ISODCL(883, 883)];
        int8_t application_data[ISODCL(884, 1395)];
        int8_t unused_5[ISODCL(1396, 2048)];
};

#define ISO_DEFAULT_BLOCK_SIZE  2048

struct iso_supplementary_descriptor {
        int8_t type[ISODCL(1, 1)]; /* 711 */
        int8_t id[ISODCL(2, 6)];
        int8_t version[ISODCL(7, 7)]; /* 711 */
        int8_t flags[ISODCL(8, 8)]; /* 711? */
        int8_t system_id[ISODCL(9, 40)]; /* aint8_ts */
        int8_t volume_id[ISODCL(41, 72)]; /* dint8_ts */
        int8_t unused_2[ISODCL(73, 80)];
        int8_t volume_space_size[ISODCL(81, 88)]; /* 733 */
        int8_t escape[ISODCL(89, 120)];
        int8_t volume_set_size[ISODCL(121, 124)]; /* 723 */
        int8_t volume_sequence_number[ISODCL(125, 128)]; /* 723 */
        int8_t logical_block_size[ISODCL(129, 132)]; /* 723 */
        int8_t path_table_size[ISODCL(133, 140)]; /* 733 */
        int8_t type_l_path_table[ISODCL(141, 144)]; /* 731 */
        int8_t opt_type_l_path_table[ISODCL(145, 148)]; /* 731 */
        int8_t type_m_path_table[ISODCL(149, 152)]; /* 732 */
        int8_t opt_type_m_path_table[ISODCL(153, 156)]; /* 732 */
        int8_t root_directory_record[ISODCL(157, 190)]; /* 9.1 */
        int8_t volume_set_id[ISODCL(191, 318)]; /* dint8_ts */
        int8_t publisher_id[ISODCL(319, 446)]; /* aint8_ts */
        int8_t preparer_id[ISODCL(447, 574)]; /* aint8_ts */
        int8_t application_id[ISODCL(575, 702)]; /* aint8_ts */
        int8_t copyright_file_id[ISODCL(703, 739)]; /* 7.5 dint8_ts */
        int8_t abstract_file_id[ISODCL(740, 776)]; /* 7.5 dint8_ts */
        int8_t bibliographic_file_id[ISODCL(777, 813)]; /* 7.5 dint8_ts */
        int8_t creation_date[ISODCL(814, 830)]; /* 8.4.26.1 */
        int8_t modification_date[ISODCL(831, 847)]; /* 8.4.26.1 */
        int8_t expiration_date[ISODCL(848, 864)]; /* 8.4.26.1 */
        int8_t effective_date[ISODCL(865, 881)]; /* 8.4.26.1 */
        int8_t file_structure_version[ISODCL(882, 882)]; /* 711 */
        int8_t unused_4[ISODCL(883, 883)];
        int8_t application_data[ISODCL(884, 1395)];
        int8_t unused_5[ISODCL(1396, 2048)];
};

struct iso_directory_record {
        int8_t length[ISODCL(1, 1)]; /* 711 */
        int8_t ext_attr_length[ISODCL(2, 2)]; /* 711 */
        uint8_t extent[ISODCL(3, 10)]; /* 733 */
        uint8_t size[ISODCL(11, 18)]; /* 733 */
        int8_t date[ISODCL(19, 25)]; /* 7 by 711 */
        int8_t flags[ISODCL(26, 26)];
        int8_t file_unit_size[ISODCL(27, 27)]; /* 711 */
        int8_t interleave[ISODCL(28, 28)]; /* 711 */
        int8_t volume_sequence_number[ISODCL(29, 32)]; /* 723 */
        int8_t name_len[ISODCL(33, 33)]; /* 711 */
        int8_t name[1]; /* XXX */
};

/* Can't take size of 'iso_directory_record', because of possible
 * alignment of the last entry (34 instead of 33) */
#define ISO_DIRECTORY_RECORD_SIZE       33

struct iso_extended_attributes {
        uint8_t owner[ISODCL(1, 4)]; /* 723 */
        uint8_t group[ISODCL(5, 8)]; /* 723 */
        uint8_t perm[ISODCL(9, 10)]; /* 9.5.3 */
        int8_t ctime[ISODCL(11, 27)]; /* 8.4.26.1 */
        int8_t mtime[ISODCL(28, 44)]; /* 8.4.26.1 */
        int8_t xtime[ISODCL(45, 61)]; /* 8.4.26.1 */
        int8_t ftime[ISODCL(62, 78)]; /* 8.4.26.1 */
        int8_t recfmt[ISODCL(79, 79)]; /* 711 */
        int8_t recattr[ISODCL(80, 80)]; /* 711 */
        uint8_t reclen[ISODCL(81, 84)]; /* 723 */
        int8_t system_id[ISODCL(85, 116)]; /* aint8_ts */
        int8_t system_use[ISODCL(117, 180)];
        int8_t version[ISODCL(181, 181)]; /* 711 */
        int8_t len_esc[ISODCL(182, 182)]; /* 711 */
        int8_t reserved[ISODCL(183, 246)];
        uint8_t len_au[ISODCL(247, 250)]; /* 723 */
};

/* 7.1.1: uint8_t */
static __inline __attribute__ ((unused)) int
isonum_711(const uint8_t *p)
{
        return *p;
}

/* 7.1.2: signed char */
static __inline __attribute__ ((unused)) int
isonum_712(const uint8_t *p)
{
        return (int8_t)*p;
}

/* 7.2.1: unsigned little-endian 16-bit value */
static __inline __attribute__ ((unused)) uint16_t
isonum_721(const uint8_t *p __attribute__ ((unused)))
{
        return 0;
}

/* 7.2.2: unsigned big-endian 16-bit value */
static __inline __attribute__ ((unused)) uint16_t
isonum_722(const uint8_t *p __attribute__ ((unused)))
{
        return 0;
}

/* 7.2.3: unsigned both-endian (little, then big) 16-bit value */
static __inline __attribute__ ((unused)) uint16_t
isonum_723(const uint8_t *p __attribute__ ((unused)))
{
        return 0;
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
isonum_733(const uint8_t *p __attribute__ ((unused)))
{
        return 0;
}

/* Associated files have a leading '=' */
#define ASSOCCHAR       '='

#endif /* _ISO9660_DEF_H_ */
