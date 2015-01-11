/*-
 * Copyright (c) 2003-2012, Troy D. Hanson <http://uthash.sourceforge.net/>
 * Copyright (c) 2012, Israel Jacquez <mrkotfw@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _HASH_H_
#define _HASH_H_

#include <sys/queue.h>

#include <common.h>

#include <inttypes.h>
#include <stddef.h>
#include <string.h>

#define DECLTYPE(x) (__typeof(x))

#ifdef NO_DECLTYPE
#define DECLTYPE_ASSIGN(dst, src) do {                                         \
        char **_da_dst = (char **)(&(dst));                                    \
        *_da_dst = (char *)(src);                                              \
} while (0)
#else
#define DECLTYPE_ASSIGN(dst, src) do {                                         \
        (dst) = DECLTYPE(dst)(src);                                            \
} while (0)
#endif /* NO_DECLTYPE */

#ifndef hash_fatal
#define hash_fatal(msg) {} /* Fatal error (out of memory, etc...) */
#endif /* !hash_fatal */

/* Initial number of buckets */
#define HASH_INITIAL_NUM_BUCKETS        32
/* lg of initial number of buckets */
#define HASH_INITIAL_NUM_BUCKETS_LOG2   5

/* Calculate the element whose hash handle address is HHP */
#define _ELEMENT_FROM_HH(tbl, hhp) ((void *)(((char *)(hhp)) - ((tbl)->ht_hho)))

/*
 * Hash table constructor.
 */
#define HASH(head, structure)                                                  \
static structure *head __unused = NULL;                                        \
static struct hash_table CC_CONCAT(head, _table) __unused;                     \
static struct hash_bucket CC_CONCAT(head, _bucket)[HASH_INITIAL_NUM_BUCKETS]   \
        __unused

#define _HASH_MAKE_TABLE(hh, head) do {                                        \
        (head)->hh.hth_tbl = (struct hash_table *)&CC_CONCAT(head, _table);    \
        memset((head)->hh.hth_tbl, 0, sizeof(struct hash_table));              \
        (head)->hh.hth_tbl->ht_tail = &((head)->hh);                           \
        (head)->hh.hth_tbl->ht_num_buckets = HASH_INITIAL_NUM_BUCKETS;         \
        (head)->hh.hth_tbl->ht_log2_num_buckets =                              \
                HASH_INITIAL_NUM_BUCKETS_LOG2;                                 \
        (head)->hh.hth_tbl->ht_hho = (char *)(&(head)->hh) - (char *)(head);   \
        (head)->hh.hth_tbl->ht_buckets = (struct hash_bucket *)&CC_CONCAT(head,\
                _bucket);                                                      \
        memset((head)->hh.hth_tbl->ht_buckets, 0,                              \
                HASH_INITIAL_NUM_BUCKETS * sizeof(struct hash_bucket));        \
        (head)->hh.hth_tbl->ht_signature = HASH_SIGNATURE;                     \
} while (0)

#define HASH_ENTER(hh, head, fieldname, key_len_in, add)                       \
        _HASH_ENTER_KEY_PTR(hh, head, &((add)->fieldname), key_len_in, add)

#define _HASH_ENTER_KEY_PTR(hh, head, key_ptr, key_len_in, add) do {           \
        unsigned _ha_bkt;                                                      \
        (add)->hh.hth_next = NULL;                                             \
        (add)->hh.hth_key = (char *)key_ptr;                                   \
        (add)->hh.hth_key_len = (unsigned)key_len_in;                          \
        if ((head) == NULL) {                                                  \
                head = (add);                                                  \
                (head)->hh.hth_prev = NULL;                                    \
                _HASH_MAKE_TABLE(hh, head);                                    \
        } else {                                                               \
                (head)->hh.hth_tbl->ht_tail->hth_hh_next =                     \
                        (struct hash_table_handle *)(add);                     \
                (add)->hh.hth_prev = _ELEMENT_FROM_HH((head)->hh.hth_tbl,      \
                        (head)->hh.hth_tbl->ht_tail);                          \
                (head)->hh.hth_tbl->ht_tail = &((add)->hh);                    \
        }                                                                      \
        (head)->hh.hth_tbl->ht_num_items++;                                    \
        (add)->hh.hth_tbl = (head)->hh.hth_tbl;                                \
        _HASH_FCN(key_ptr, key_len_in, (head)->hh.hth_tbl->ht_num_buckets,     \
                (add)->hh.hth_hashv, _ha_bkt);                                 \
        _HASH_ENTER_TO_BUCKET((head)->hh.hth_tbl->ht_buckets[_ha_bkt],         \
                &(add)->hh);                                                   \
        _HASH_EMIT_KEY(hh, head, key_ptr, key_len_in);                         \
} while (0)

/* Convenient forms of HASH_ENTER */
#define HASH_ENTER_STR(hh, head, strfield, add)                                \
        HASH_ENTER(hh, head, strfield, strlen(add->strfield), add)
#define HASH_ENTER_INT(hh, head, intfield, add)                                \
        HASH_ENTER(hh, head, intfield, sizeof(int), add)
#define HASH_ENTER_PTR(hh, head, ptrfield, add)                                \
        HASH_ENTER(hh, head, ptrfield, sizeof(void *), add)

#define _HASH_TO_BUCKET(hashv, num_bkts, bkt) do {                             \
        bkt = ((hashv) & ((num_bkts) - 1));                                    \
} while (0)

/* Delete an entry from the hash table.
 *
 * The usual patch-up process for the app-order doubly-linked-list. The
 * use of _hd_hh_del below deserves special explanation. These used to
 * be expressed using (delptr) but that led to a bug if someone used the
 * same symbol for the head and delete, like
 *
 * HASH_REMOVE(hh, users, users)
 *
 * We want that to work, but by changing the head (users) below we were
 * forfeiting our ability to further refer to the deletee (users) in the
 * patch-up process. Solution: use scratch space to copy the deletee
 * pointer, then the latter references are via that scratch pointer
 * rather than through the repointed (users) symbol.
 */
#define HASH_REMOVE(hh, head, delptr) do {                                     \
        unsigned _hd_bkt;                                                      \
        struct hash_table_handle *_hd_hh_del;                                  \
        if (head != NULL) {                                                    \
                if (((delptr)->hh.hth_prev == NULL) &&                         \
                        ((delptr)->hh.hth_next == NULL))                       \
                        head = NULL;                                           \
                else {                                                         \
                        _hd_hh_del = &((delptr)->hh);                          \
                        if ((delptr) == _ELEMENT_FROM_HH((head)->hh.hth_tbl,   \
                                (head)->hh.hth_tbl->ht_tail)) {                \
                                        (head)->hh.hth_tbl->ht_tail = (struct hash_table_handle *) \
                                                ((char *)((delptr)->hh.hth_prev) + \
                                                        (head)->hh.hth_tbl->ht_hho); \
                        }                                                      \
                        if ((delptr)->hh.hth_prev) {                           \
                                ((struct hash_table_handle *)((char *)((delptr)->hh.hth_prev) + \
                                        (head)->hh.hth_tbl->ht_hho))->next =   \
                                                (delptr)->hh.hth_next;         \
                        } else                                                 \
                                DECLTYPE_ASSIGN(head, (delptr)->hh.hth_next);  \
                        if (_hd_hh_del->next) {                                \
                                ((struct hash_table_handle *)((char *)_hd_hh_del->next + \
                                        (head)->hh.hth_tbl->ht_hho))->prev =   \
                                        _hd_hh_del->prev;                      \
                        }                                                      \
                        _HASH_TO_BUCKET(_hd_hh_del->hashv,                     \
                                (head)->hh.hth_tbl->ht_num_buckets, _hd_bkt);  \
                        _HASH_REMOVE_IN_BUCKET(hh,                             \
                                (head)->hh.hth_tbl->ht_buckets[_hd_bkt],       \
                                        _hd_hh_del);                           \
                        (head)->hh.hth_tbl->ht_num_items--;                    \
                }                                                              \
        }                                                                      \
} while (0)

#define HASH_FIND(hh, head, key_ptr, key_len, out) do {                        \
        unsigned _hf_bkt;                                                      \
        unsigned _hf_hashv;                                                    \
        out = NULL;                                                            \
        if (head != NULL) {                                                    \
                _HASH_FCN(key_ptr, key_len, (head)->hh.hth_tbl->ht_num_buckets,\
                        _hf_hashv, _hf_bkt);                                   \
                _HASH_FIND_IN_BUCKET((head)->hh.hth_tbl, hh,                   \
                        (head)->hh.hth_tbl->ht_buckets[_hf_bkt], key_ptr,      \
                                key_len, out);                                 \
        }                                                                      \
} while (0)

/* Convenient forms of HASH_FIND */
#define HASH_FIND_STR(hh, head, findstr, out)                                  \
        HASH_FIND(hh, head, findstr, strlen(findstr), out)
#define HASH_FIND_INT(hh, head, findint, out)                                  \
        HASH_FIND(hh, head, findint, sizeof(int), out)
#define HASH_FIND_PTR(hh, head, findptr, out)                                  \
        HASH_FIND(hh, head, findptr, sizeof(void *), out)

/* When compiled with HASH_EMIT_KEYS, length-prefixed keys are emitted
 * to the descriptor to which this macro is defined for tuning the hash
 * function. The app can include "unistd.h" to get the prototype for
 * write(2). */
#ifdef HASH_EMIT_KEYS
#define _HASH_EMIT_KEY(hh, head, key_ptr, fieldlen) do {                       \
        unsigned _klen = fieldlen;                                             \
        write(HASH_EMIT_KEYS, &_klen, sizeof(_klen));                          \
        write(HASH_EMIT_KEYS, key_ptr, fieldlen);                              \
} while (0)
#else
#define _HASH_EMIT_KEY(hh, head, key_ptr, fieldlen)
#endif /* HASH_EMIT_KEYS */

/* Default to Jenkin's hash unless overridden e.g. -DHASH_FUNCTION = HASH_SAX */
#ifdef HASH_FUNCTION
#define _HASH_FCN HASH_FUNCTION
#else
#define _HASH_FCN _HASH_JEN
#endif /* HASH_FUNCTION */

/* The Bernstein hash function, used in Perl prior to v5.6 */
#define _HASH_BER(key, key_len, num_bkts, hashv, bkt) do {                     \
        unsigned _hb_key_len = key_len;                                        \
        char *_hb_key = (char *)(key);                                         \
        (hashv) = 0;                                                           \
        while (_hb_key_len--)                                                  \
                (hashv) = ((hashv) * 33) + *_hb_key++;                         \
        bkt = (hashv) & (num_bkts - 1);                                        \
} while (0)

/* SAX/FNV/OAT/JEN hash functions are macro variants of those listed at
 * http://eternallyconfuzzled.com/tuts/algorithms/jsw_tut_hashing.aspx */
#define _HASH_SAX(key, key_len, num_bkts, hashv, bkt) do {                     \
        unsigned _sx_i;                                                        \
        char *_hs_key = (char *)(key);                                         \
        hashv = 0;                                                             \
        for (_sx_i = 0; _sx_i < key_len; _sx_i++)                              \
                hashv ^= (hashv << 5) + (hashv >> 2) + _hs_key[_sx_i];         \
        bkt = hashv & (num_bkts - 1);                                          \
} while (0)

#define _HASH_FNV(key, key_len, num_bkts, hashv, bkt) do {                     \
        unsigned _fn_i;                                                        \
        char *_hf_key = (char *)(key);                                         \
        hashv = 2166136261UL;                                                  \
        for (_fn_i = 0; _fn_i < key_len; _fn_i++) {                            \
                hashv = (hashv * 16777619) ^ _hf_key[_fn_i];                   \
        }                                                                      \
        bkt = hashv & (num_bkts - 1);                                          \
} while (0)

#define _HASH_OAT(key, key_len, num_bkts, hashv, bkt) do {                     \
        unsigned _ho_i;                                                        \
        char *_ho_key = (char *)(key);                                         \
        hashv = 0;                                                             \
        for (_ho_i = 0; _ho_i < key_len; _ho_i++) {                            \
                hashv += _ho_key[_ho_i];                                       \
                hashv += (hashv << 10);                                        \
                hashv ^= (hashv >> 6);                                         \
        }                                                                      \
        hashv += (hashv << 3);                                                 \
        hashv ^= (hashv >> 11);                                                \
        hashv += (hashv << 15);                                                \
        bkt = hashv & (num_bkts - 1);                                          \
} while (0)

#define _HASH_JEN_MIX(a, b, c) do {                                            \
        a -= b;                                                                \
        a -= c;                                                                \
        a ^= (c >> 13);                                                        \
                                                                               \
        b -= c;                                                                \
        b -= a;                                                                \
        b ^= (a << 8);                                                         \
                                                                               \
        c -= a;                                                                \
        c -= b;                                                                \
        c ^= (b >> 13);                                                        \
                                                                               \
        a -= b;                                                                \
        a -= c;                                                                \
        a ^= (c >> 12);                                                        \
                                                                               \
        b -= c;                                                                \
        b -= a;                                                                \
        b ^= (a << 16);                                                        \
                                                                               \
        c -= a;                                                                \
        c -= b;                                                                \
        c ^= (b >> 5);                                                         \
                                                                               \
        a -= b;                                                                \
        a -= c;                                                                \
        a ^= (c >> 3);                                                         \
                                                                               \
        b -= c;                                                                \
        b -= a;                                                                \
        b ^= (a << 10);                                                        \
                                                                               \
        c -= a;                                                                \
        c -= b;                                                                \
        c ^= (b >> 15);                                                        \
} while (0)

#define _HASH_JEN(key, key_len, num_bkts, hashv, bkt) do {                     \
        unsigned _hj_i, _hj_j, _hj_k;                                          \
        char *_hj_key = (char *)(key);                                         \
        hashv = 0xFEEDBEEF;                                                    \
        _hj_i = _hj_j = 0x9E3779B9;                                            \
        _hj_k = (unsigned)key_len;                                             \
        while (_hj_k >= 12) {                                                  \
                _hj_i += (_hj_key[0] + ((unsigned)_hj_key[1] << 8) +           \
                        ((unsigned)_hj_key[2] << 16) +                         \
                        ((unsigned)_hj_key[3] << 24));                         \
                _hj_j += (_hj_key[4] + ((unsigned)_hj_key[5] << 8) +           \
                        ((unsigned)_hj_key[6] << 16) +                         \
                        ((unsigned)_hj_key[7] << 24));                         \
                hashv += (_hj_key[8] + ((unsigned)_hj_key[9] << 8) +           \
                        ((unsigned)_hj_key[10] << 16) +                        \
                        ((unsigned)_hj_key[11] << 24));                        \
                _HASH_JEN_MIX(_hj_i, _hj_j, hashv);                            \
                _hj_key += 12;                                                 \
                _hj_k -= 12;                                                   \
        }                                                                      \
        hashv += key_len;                                                      \
        switch (_hj_k) {                                                       \
        case 11:                                                               \
                hashv += ((unsigned)_hj_key[10] << 24);                        \
        case 10:                                                               \
                hashv += ((unsigned)_hj_key[9] << 16);                         \
        case 9:                                                                \
                hashv += ((unsigned)_hj_key[8] << 8);                          \
        case 8:                                                                \
                _hj_j += ((unsigned)_hj_key[7] << 24);                         \
        case 7:                                                                \
                _hj_j += ((unsigned)_hj_key[6] << 16);                         \
        case 6:                                                                \
                _hj_j += ((unsigned)_hj_key[5] << 8);                          \
        case 5:                                                                \
                _hj_j += _hj_key[4];                                           \
        case 4:                                                                \
                _hj_i += ((unsigned)_hj_key[3] << 24);                         \
        case 3:                                                                \
                _hj_i += ((unsigned)_hj_key[2] << 16);                         \
        case 2:                                                                \
                _hj_i += ((unsigned)_hj_key[1] << 8);                          \
        case 1:                                                                \
                _hj_i += _hj_key[0];                                           \
        }                                                                      \
        _HASH_JEN_MIX(_hj_i, _hj_j, hashv);                                    \
        bkt = hashv & (num_bkts - 1);                                          \
} while (0)

/* Key comparison function; return 0 if keys equal */
#define _HASH_KEYCMP(a, b, len) memcmp(a, b, len)

/*
 * Iterate over items in a known bucket to find desired item
 */
#define _HASH_FIND_IN_BUCKET(tbl, hh, head, key_ptr, key_len_in, out) do {     \
        if ((head).hb_hh_head != NULL)                                         \
                DECLTYPE_ASSIGN(out, _ELEMENT_FROM_HH(tbl, (head).hb_hh_head));\
        else                                                                   \
                out = NULL;                                                    \
        while (out) {                                                          \
                if ((out)->hh.hth_key_len == key_len_in) {                     \
                        if ((_HASH_KEYCMP((out)->hh.hth_key, key_ptr,          \
                                key_len_in)) == 0) {                           \
                                break;                                         \
                        }                                                      \
                }                                                              \
                if ((out)->hh.hth_hh_next) {                                   \
                        DECLTYPE_ASSIGN(out, _ELEMENT_FROM_HH(tbl,             \
                                (out)->hh.hth_hh_next));                       \
                } else                                                         \
                        out = NULL;                                            \
        }                                                                      \
} while (0)

/*
 * Add an item to a bucket.
 */
#define _HASH_ENTER_TO_BUCKET(head, addhh) do {                                \
        (head).hb_count++;                                                     \
        (addhh)->hth_hh_next = (head).hb_hh_head;                              \
        (addhh)->hth_hh_prev = NULL;                                           \
        if ((head).hb_hh_head)                                                 \
                (head).hb_hh_head->hth_hh_prev = (addhh);                      \
        (head).hb_hh_head = addhh;                                             \
} while (0)

/*
 * Remove an item from a given bucket.
 */
#define _HASH_REMOVE_IN_BUCKET(hh, head, hh_del)                               \
        (head).hb_count--;                                                     \
        if ((head).hb_hh_head == hh_del)                                       \
                (head).hb_hh_head = hh_del->hth_hh_next;                       \
        if (hh_del->hth_hh_prev)                                               \
                hh_del->hth_hh_prev->hth_hh_next = hh_del->hth_hh_next;        \
        if (hh_del->hth_hh_next)                                               \
                hh_del->hth_hh_next->hth_hh_prev = hh_del->hth_hh_prev;        \

/*
 * Sort the hash table using the comparison function CMPFCN.
 *
 * This is an adaptation of Simon Tatham's O(n log(n)) mergesort.
 */
#define HASH_SORT(hh, head, cmpfcn) do {                                       \
        unsigned _hs_i;                                                        \
        unsigned _hs_looping, _hs_nmerges, _hs_insize, _hs_psize, _hs_qsize;   \
        struct hash_table_handle *_hs_p, *_hs_q, *_hs_e, *_hs_list, *_hs_tail; \
        if (head != NULL) {                                                    \
                _hs_insize = 1;                                                \
                _hs_looping = 1;                                               \
                _hs_list = &((head)->hh);                                      \
                while (_hs_looping) {                                          \
                        _hs_p = _hs_list;                                      \
                        _hs_list = NULL;                                       \
                        _hs_tail = NULL;                                       \
                        _hs_nmerges = 0;                                       \
                        while (_hs_p) {                                        \
                                _hs_nmerges++;                                 \
                                _hs_q = _hs_p;                                 \
                                _hs_psize = 0;                                 \
                                for (_hs_i = 0; _hs_i < _hs_insize; _hs_i++) { \
                                        _hs_psize++;                           \
                                        _hs_q = (struct hash_table_handle *)((_hs_q->next) \
                                                ? ((void *)((char *)(_hs_q->next) + \
                                                        (head)->hh.hth_tbl->ht_hho)) \
                                                : NULL);                       \
                                        if (!(_hs_q))                          \
                                                break;                         \
                                }                                              \
                                _hs_qsize = _hs_insize;                        \
                                while ((_hs_psize > 0) || ((_hs_qsize > 0)     \
                                        && _hs_q)) {                           \
                                        if (_hs_psize == 0) {                  \
                                                _hs_e = _hs_q;                 \
                                                _hs_q = (struct hash_table_handle *)((_hs_q->next) \
                                                        ? ((void *)((char *)(_hs_q->next) + \
                                                                (head)->hh.hth_tbl->ht_hho)) \
                                                        : NULL);               \
                                                _hs_qsize--;                   \
                                        } else if ((_hs_qsize == 0) || !(_hs_q)) { \
                                                _hs_e = _hs_p;                 \
                                                _hs_p = (struct hash_table_handle *)((_hs_p->next) \
                                                        ? ((void *)((char *)(_hs_p->next) + \
                                                                (head)->hh.hth_tbl->ht_hho)) \
                                                        : NULL);               \
                                                _hs_psize--;                   \
                                        } else if ((cmpfcn(DECLTYPE(head)(_ELEMENT_FROM_HH((head)->hh.hth_tbl, _hs_p)), \
                                                DECLTYPE(head)(_ELEMENT_FROM_HH((head)->hh.hth_tbl, _hs_q)))) <= 0) { \
                                                _hs_e = _hs_p;                 \
                                                _hs_p = (struct hash_table_handle *)((_hs_p->next) \
                                                        ? ((void *)((char *)(_hs_p->next) + \
                                                                (head)->hh.hth_tbl->ht_hho)) \
                                                        : NULL);               \
                                                _hs_psize--;                   \
                                        } else {                               \
                                                _hs_e = _hs_q;                 \
                                                _hs_q = (struct hash_table_handle *)((_hs_q->next) \
                                                        ? ((void *)((char *)(_hs_q->next) + \
                                                                (head)->hh.hth_tbl->ht_hho)) \
                                                        : NULL);               \
                                                _hs_qsize--;                   \
                                        }                                      \
                                        if (_hs_tail) {                        \
                                                _hs_tail->next = ((_hs_e)      \
                                                        ? _ELEMENT_FROM_HH((head)->hh.hth_tbl, _hs_e) \
                                                        : NULL);               \
                                        } else                                 \
                                                _hs_list = _hs_e;              \
                                        _hs_e->prev = ((_hs_tail)              \
                                                ? _ELEMENT_FROM_HH((head)->hh.hth_tbl, _hs_tail) \
                                                : NULL);                       \
                                        _hs_tail = _hs_e;                      \
                                }                                              \
                                _hs_p = _hs_q;                                 \
                        }                                                      \
                        _hs_tail->next = NULL;                                 \
                        if (_hs_nmerges <= 1) {                                \
                                _hs_looping = 0;                               \
                                (head)->hh.hth_tbl->ht_tail = _hs_tail;        \
                                DECLTYPE_ASSIGN(head,                          \
                                        _ELEMENT_FROM_HH((head)->hh.hth_tbl,   \
                                                _hs_list));                    \
                        }                                                      \
                        _hs_insize *= 2;                                       \
                }                                                              \
        }                                                                      \
} while (0)

#ifdef NO_DECLTYPE
#define HASH_FOREACH(hh, head, el, tmp)                                        \
        for ((el) = (head),                                                    \
                (*(char **)(&(tmp))) = (char *)((head)                         \
                        ? (head)->hh.hth_next                                  \
                        : NULL);                                               \
                el;                                                            \
                (el) = (tmp), (*(char **)(&(tmp))) = (char *)((tmp)            \
                        ? (tmp)->hh.hth_next                                   \
                        : NULL))
#else
#define HASH_FOREACH(hh, head, el, tmp)                                        \
        for ((el) = (head),                                                    \
                (tmp) = DECLTYPE(el)((head) ? (head)->hh.hth_next : NULL);     \
                el;                                                            \
                (el) = (tmp), (tmp) = DECLTYPE(el)((tmp)                       \
                        ? (tmp)->hh.hth_next                                   \
                        : NULL))
#endif /* NO_DECLTYPE */

/*
 * Obtain a count of items in the hash
 */
#define HASH_COUNT(hh, head) ((head != NULL)                                   \
        ? (head)->hh.hth_tbl->ht_num_items                                     \
        : 0)

/*
 * Obtain the maximum number of items allowed in the hash
 */
#define HASH_CAPACITY_COUNT() (HASH_INITIAL_NUM_BUCKETS)

struct hash_bucket {
        struct hash_table_handle *hb_hh_head;
        unsigned hb_count;
};

/* random signature used only to find hash tables in external analysis */
#define HASH_SIGNATURE          0xA0111FE1

struct hash_table {
        struct hash_bucket *ht_buckets;
        unsigned ht_num_buckets;
        unsigned ht_log2_num_buckets;
        unsigned ht_num_items;
        struct hash_table_handle *ht_tail; /* Tail hh in app order, for
                                            * fast append */
        ptrdiff_t ht_hho; /* Hash handle offset (byte pos of hash handle
                           * in element */

        /* In an ideal situation (all buckets used equally), no bucket
         * would have more than ceil(#items / #buckets) items. That's
         * the ideal chain length.
         */
        unsigned ht_ideal_chain_maxlen;

        /* nonideal_items is the number of items in the hash whose chain
         * position exceeds the ideal chain maxlen. these items pay the
         * penalty for an uneven hash distribution; reaching them in a
         * chain traversal takes >ideal steps.
         */
        unsigned ht_nonideal_items;

        /* Ineffective expands occur when a bucket doubling was
         * performed, but afterward, more than half the items in the
         * hash had nonideal chain positions. If this happens on two
         * consecutive expansions we inhibit any further expansion, as
         * it's not helping; this happens when the hash function isn't a
         * good fit for the key domain. When expansion is inhibited the
         * hash will still work, albeit no longer in constant time.
         */
        unsigned ht_ineff_expands;
        unsigned ht_noexpand;

        uint32_t ht_signature; /* Used only to find hash tables in
                                * external analysis */
};

struct hash_table_handle {
        struct hash_table *hth_tbl;
        void *hth_prev; /* Previous element in app order */
        void *hth_next; /* Next element in app order */
        struct hash_table_handle *hth_hh_prev; /* Previous hh in bucket order */
        struct hash_table_handle *hth_hh_next; /* Next hh in bucket order */
        void *hth_key; /* Pointer to enclosing struct's key */
        unsigned hth_key_len; /* Enclosing struct's key len */
        unsigned hth_hashv; /* Result of hash-fcn(key) */
};

#endif /* _HASH_H_ */
