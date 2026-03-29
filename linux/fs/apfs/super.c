// SPDX-License-Identifier: GPL-2.0-only
/*
 * APFS filesystem registration scaffold.
 */

#include <linux/fs.h>
#include <linux/module.h>

#include "apfs.h"

static struct file_system_type apfs_fs_type = {
	.owner			= THIS_MODULE,
	.name			= "apfs",
	.init_fs_context	= apfs_init_fs_context,
	.kill_sb		= kill_block_super,
	.fs_flags		= FS_REQUIRES_DEV,
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

MODULE_DESCRIPTION("APFS integration scaffold");
MODULE_AUTHOR("linux-apfs project contributors");
MODULE_LICENSE("GPL");
