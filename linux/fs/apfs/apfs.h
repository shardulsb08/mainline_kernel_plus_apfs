/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _FS_APFS_APFS_H
#define _FS_APFS_APFS_H

#include <linux/errno.h>
#include <linux/fs_context.h>

#define APFS_STUB_MSG "APFS support is not available in this kernel build"
#define APFS_STUB_ERRNO EOPNOTSUPP

#define APFS_NX_DEFAULT_BLOCK_SIZE 4096

#define APFS_MIN_BLOCK_SIZE 512
#define APFS_MAX_BLOCK_SIZE 65536

struct apfs_fs_context {
	bool readwrite;
	bool cknodes;
	bool vol_set;
	bool uid_set;
	bool gid_set;
	u32 vol;
	u32 uid;
	u32 gid;
	char *snap;
	char *tier2;
};

int apfs_init_fs_context(struct fs_context *fc);
bool apfs_verify_block_csum(const void *block, size_t size);

#endif /* _FS_APFS_APFS_H */
