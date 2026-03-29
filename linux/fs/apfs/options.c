// SPDX-License-Identifier: GPL-2.0-only
/*
 * Mount parameter and fs_context scaffolding for APFS.
 */

#include <linux/buffer_head.h>
#include <linux/fs.h>
#include <linux/fs_context.h>
#include <linux/fs_parser.h>
#include <linux/slab.h>

#include "apfs.h"
#include "apfs_raw.h"

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
		ctx->vol_set = true;
		break;
	case opt_snap:
		if (!param->string || !*param->string)
			return invalf(fc, "snapshot label must not be empty");
		kfree(ctx->snap);
		ctx->snap = kstrdup(param->string, GFP_KERNEL);
		if (!ctx->snap)
			return -ENOMEM;
		break;
	case opt_tier2:
		if (!param->string || !*param->string)
			return invalf(fc, "tier2 path must not be empty");
		kfree(ctx->tier2);
		ctx->tier2 = kstrdup(param->string, GFP_KERNEL);
		if (!ctx->tier2)
			return -ENOMEM;
		break;
	case opt_uid:
		ctx->uid = result.uint_32;
		ctx->uid_set = true;
		break;
	case opt_gid:
		ctx->gid = result.uint_32;
		ctx->gid_set = true;
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

static int apfs_validate_options(struct fs_context *fc)
{
	struct apfs_fs_context *ctx = fc->fs_private;

	if (ctx->snap && ctx->readwrite)
		return invalf(fc, "readwrite mounts with snapshots are not supported");

	return 0;
}

static int apfs_fill_super(struct super_block *sb, struct fs_context *fc)
{
	struct apfs_fs_context *ctx = fc->fs_private;
	struct buffer_head *bh;
	u32 magic;
	u32 block_size;
	u64 block_count;
	u64 features;
	u64 ro_compat;
	u64 incompat;
	struct apfs_nx_superblock *nxsb;

	if (!sb_set_blocksize(sb, APFS_NX_DEFAULT_BLOCK_SIZE))
		return errorf(fc, "failed to set blocksize %u", APFS_NX_DEFAULT_BLOCK_SIZE);

	bh = sb_bread(sb, 0);
	if (!bh)
		return errorf(fc, "failed to read container superblock");

	if (!apfs_verify_block_csum(bh->b_data, sb->s_blocksize)) {
		brelse(bh);
		return errorf(fc, "checksum verification failed for container superblock");
	}

	nxsb = (struct apfs_nx_superblock *)bh->b_data;
	magic = le32_to_cpu(nxsb->nx_magic);
	block_size = le32_to_cpu(nxsb->nx_block_size);
	block_count = le64_to_cpu(nxsb->nx_block_count);
	features = le64_to_cpu(nxsb->nx_features);
	ro_compat = le64_to_cpu(nxsb->nx_readonly_compatible_features);
	incompat = le64_to_cpu(nxsb->nx_incompatible_features);
	brelse(bh);

	if (magic != APFS_NX_MAGIC)
		return errorf(fc, "not an APFS container (magic=0x%08x)", magic);

	if (block_size < APFS_MIN_BLOCK_SIZE || block_size > APFS_MAX_BLOCK_SIZE ||
	    !is_power_of_2(block_size))
		return errorf(fc, "invalid APFS block size %u", block_size);

	if (!block_count)
		return errorf(fc, "invalid APFS container block count %llu", block_count);

	if (features & ~APFS_NX_SUPPORTED_FEATURES_MASK)
		return errorf(fc, "unsupported APFS features 0x%llx",
			      features & ~APFS_NX_SUPPORTED_FEATURES_MASK);

	if (incompat & ~APFS_NX_SUPPORTED_INCOMPAT_MASK)
		return errorf(fc, "unsupported APFS incompatible features 0x%llx",
			      incompat & ~APFS_NX_SUPPORTED_INCOMPAT_MASK);

	if (ro_compat)
		pr_warn_once("apfs: container uses ro-compatible features 0x%llx\n",
			     ro_compat);

	if (!sb_set_blocksize(sb, block_size))
		return errorf(fc, "failed to set APFS block size %u", block_size);

	sb->s_magic = magic;

	pr_warn_once("apfs: %s (source=%s, vol=%u, readwrite=%u, cknodes=%u, snap=%s, tier2=%s, ro_compat=0x%llx)\n",
		     APFS_STUB_MSG,
		     fc->source ? fc->source : "<none>",
		     ctx->vol, ctx->readwrite, ctx->cknodes,
		     ctx->snap ? ctx->snap : "<none>",
		     ctx->tier2 ? ctx->tier2 : "<none>",
		     ro_compat);

	return -APFS_STUB_ERRNO;
}

static int apfs_get_tree(struct fs_context *fc)
{
	int err;

	err = apfs_validate_options(fc);
	if (err)
		return err;

	return get_tree_bdev(fc, apfs_fill_super);
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

int apfs_init_fs_context(struct fs_context *fc)
{
	struct apfs_fs_context *ctx;

	ctx = kzalloc_obj(ctx, GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	fc->ops = &apfs_context_ops;
	fc->fs_private = ctx;
	return 0;
}
