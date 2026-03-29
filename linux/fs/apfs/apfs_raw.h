/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _FS_APFS_APFS_RAW_H
#define _FS_APFS_APFS_RAW_H

#include <linux/types.h>

#define APFS_NX_MAGIC 0x4253584E

struct apfs_obj_phys {
	__le64 o_cksum;
	__le64 o_oid;
	__le64 o_xid;
	__le32 o_type;
	__le32 o_subtype;
} __packed;

struct apfs_nx_superblock {
	struct apfs_obj_phys nx_o;
	__le32 nx_magic;
	__le32 nx_block_size;
} __packed;

#endif /* _FS_APFS_APFS_RAW_H */
