// SPDX-License-Identifier: GPL-2.0-only
/*
 * Minimal APFS in-tree integration scaffold.
 *
 * This is intentionally non-functional and exists only to provide a concrete
 * compile target while APFS code is imported in larger reviewable steps.
 */

#include <linux/fs.h>
#include <linux/fs_context.h>
#include <linux/fs_parser.h>
#include <linux/module.h>
#include <linux/slab.h>

#include "apfs.h"

enum apfs_param {
	opt_vol,
	opt_snap,
	opt_tier2,
	opt_uid,
	opt_gid,
	opt_readwrite,
	opt_cknodes,
};

static const struct fs_parameter_spec apfs_param_specs[] = {
	fsparam_u32("vol", opt_vol),
	fsparam_string("snap", opt_snap),
	fsparam_string("tier2", opt_tier2),
	fsparam_u32("uid", opt_uid),
	fsparam_u32("gid", opt_gid),
	fsparam_flag("readwrite", opt_readwrite),
	fsparam_flag("cknodes", opt_cknodes),
	{}
};

struct apfs_fs_context {
	bool readwrite;
	bool cknodes;
	u32 vol;
	u32 uid;
	u32 gid;
	char *snap;
	char *tier2;
};

static int apfs_parse_param(struct fs_context *fc, struct fs_parameter *param)
{
	struct apfs_fs_context *ctx = fc->fs_private;
	struct fs_parse_result result;
	int opt;

	opt = fs_parse(fc, apfs_param_specs, param, &result);
	if (opt < 0)
		return opt;

	switch (opt) {
	case opt_vol:
		ctx->vol = result.uint_32;
		break;
	case opt_snap:
		kfree(ctx->snap);
		ctx->snap = kstrdup(param->string, GFP_KERNEL);
		if (!ctx->snap)
			return -ENOMEM;
		break;
	case opt_tier2:
		kfree(ctx->tier2);
		ctx->tier2 = kstrdup(param->string, GFP_KERNEL);
		if (!ctx->tier2)
			return -ENOMEM;
		break;
	case opt_uid:
		ctx->uid = result.uint_32;
		break;
	case opt_gid:
		ctx->gid = result.uint_32;
		break;
	case opt_readwrite:
		ctx->readwrite = true;
		break;
	case opt_cknodes:
		ctx->cknodes = true;
		break;
	default:
		return -EINVAL;
	}
	return 0;
}

static int apfs_get_tree(struct fs_context *fc)
{
	struct apfs_fs_context *ctx = fc->fs_private;

	pr_warn_once("apfs: %s (source=%s, vol=%u, readwrite=%u, cknodes=%u)\n",
		     APFS_STUB_MSG,
		     fc->source ? fc->source : "<none>",
		     ctx->vol, ctx->readwrite, ctx->cknodes);
	return -APFS_STUB_ERRNO;
}

static void apfs_free_fc(struct fs_context *fc)
{
	struct apfs_fs_context *ctx = fc->fs_private;

	if (!ctx)
		return;
	kfree(ctx->snap);
	kfree(ctx->tier2);
	kfree(ctx);
}

static const struct fs_context_operations apfs_context_ops = {
	.free		= apfs_free_fc,
	.parse_param	= apfs_parse_param,
	.get_tree	= apfs_get_tree,
};

static int apfs_init_fs_context(struct fs_context *fc)
{
	struct apfs_fs_context *ctx;

	ctx = kzalloc_obj(ctx, GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	fc->ops = &apfs_context_ops;
	fc->fs_private = ctx;
	return 0;
}

static struct file_system_type apfs_fs_type = {
	.owner			= THIS_MODULE,
	.name			= "apfs",
	.init_fs_context	= apfs_init_fs_context,
	.parameters		= apfs_param_specs,
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
