// SPDX-License-Identifier: GPL-2.0-only
/*
 * Minimal APFS in-tree integration stub.
 *
 * This is intentionally non-functional and exists only to provide a concrete
 * compile target while APFS code is imported in small reviewable steps.
 */

#include <linux/fs.h>
#include <linux/module.h>
#include <linux/pseudo_fs.h>

#define APFS_STUB_MSG "APFS support is not available in this kernel build"

static struct dentry *apfs_mount(struct file_system_type *fs_type, int flags,
				 const char *dev_name, void *data)
{
	pr_warn_once("apfs: %s\n", APFS_STUB_MSG);
	return ERR_PTR(-EOPNOTSUPP);
}

static void apfs_kill_sb(struct super_block *sb)
{
	kill_block_super(sb);
}

static struct file_system_type apfs_fs_type = {
	.owner		= THIS_MODULE,
	.name		= "apfs",
	.mount		= apfs_mount,
	.kill_sb	= apfs_kill_sb,
	.fs_flags	= FS_REQUIRES_DEV,
};
MODULE_ALIAS_FS("apfs");

static int __init init_apfs_fs(void)
{
	return register_filesystem(&apfs_fs_type);
}

static void __exit exit_apfs_fs(void)
{
	unregister_filesystem(&apfs_fs_type);
}

module_init(init_apfs_fs);
module_exit(exit_apfs_fs);

MODULE_DESCRIPTION("APFS integration stub");
MODULE_AUTHOR("linux-apfs project contributors");
MODULE_LICENSE("GPL");
