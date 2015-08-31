/*
 * Copyright (c) 2012-2016 Israel Jacquez
 * See LICENSE for details.
 *
 * Israel Jacquez <mrkotfw@gmail.com>
 */

#ifndef _VNODE_H_
#define _VNODE_H_

#include <sys/queue.h>

enum vnode_obj_type {
        VNODE_OBJ_NONE,
        VNODE_OBJ_REGULAR,
        VNODE_OBJ_DIRECTORY,
        VNODE_OBJ_BAD
};

enum vnode_tag_type {
        VNODE_TAG_NONE,
        VNODE_TAG_ISO9660,
        VNODE_TAG_ROMDISK
};

#define VNODE_TYPE_NAMES                                                      \
        "VNODE_NONE",                                                         \
        "VNODE_REGULAR",                                                      \
        "VNODE_DIRECTORY",                                                    \
        "VNODE_BAD"

struct vnode_ops;

struct vnode {
        enum vnode_obj_type vn_type;
        enum vnode_tag_type vn_tag;
        void *vn_data;
        struct vnode_ops **vn_ops;
};

struct vnode_ops {
        int (*vnop_abort_op)(void *);
        int (*vnop_close)(void *);
        int (*vnop_inactive)(void *);
        int (*vnop_ioctl)(void *);
        int (*vnop_lookup)(void *);
        int (*vnop_open)(void *);
        int (*vnop_poll)(void *);
        int (*vnop_print)(void *);
        int (*vnop_read)(void *);
        int (*vnop_read_dir)(void *);
        int (*vnop_realloc_blocks)(void *);
        int (*vnop_reclaim)(void *);
        int (*vnop_revoke)(void *);
        int (*vnop_strategy)(void *);
};

#endif /* !_VNODE_H_ */
