/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _FS_APFS_APFS_H
#define _FS_APFS_APFS_H

#include <linux/errno.h>
#include <linux/fs_context.h>

#define APFS_STUB_MSG "APFS support is not available in this kernel build"
#define APFS_STUB_ERRNO EOPNOTSUPP

struct apfs_fs_context {
	bool readwrite;
	bool cknodes;
	u32 vol;
	u32 uid;
	u32 gid;
	char *snap;
	char *tier2;
};

int apfs_init_fs_context(struct fs_context *fc);

#endif /* _FS_APFS_APFS_H */
