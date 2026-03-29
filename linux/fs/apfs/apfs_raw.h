/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _FS_APFS_APFS_RAW_H
#define _FS_APFS_APFS_RAW_H

#include <linux/types.h>

#define APFS_NX_MAGIC 0x4253584E

/* Known container feature flags */
#define APFS_NX_FEATURE_DEFRAG 0x0000000000000001ULL
#define APFS_NX_FEATURE_LCFD 0x0000000000000002ULL
#define APFS_NX_SUPPORTED_FEATURES_MASK \
	(APFS_NX_FEATURE_DEFRAG | APFS_NX_FEATURE_LCFD)

/* Known incompatible feature flags */
#define APFS_NX_INCOMPAT_VERSION1 0x0000000000000001ULL
#define APFS_NX_INCOMPAT_VERSION2 0x0000000000000002ULL
#define APFS_NX_INCOMPAT_FUSION 0x0000000000000100ULL
#define APFS_NX_SUPPORTED_INCOMPAT_MASK \
	(APFS_NX_INCOMPAT_VERSION2 | APFS_NX_INCOMPAT_FUSION)

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
	__le64 nx_block_count;
	__le64 nx_features;
	__le64 nx_readonly_compatible_features;
	__le64 nx_incompatible_features;
} __packed;

#endif /* _FS_APFS_APFS_RAW_H */
