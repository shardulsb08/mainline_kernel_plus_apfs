// SPDX-License-Identifier: GPL-2.0-only
/*
 * Checksum helpers for APFS object headers.
 */

#include <linux/math64.h>

#include "apfs.h"
#include "apfs_raw.h"

/*
 * This is intentionally scoped to APFS block sizes (<= 64KiB). We do not try
 * to provide a generic fletcher64 implementation here.
 */
static u64 apfs_fletcher64(const void *addr, size_t len)
{
	const __le32 *buff = addr;
	u64 sum1 = 0;
	u64 sum2 = 0;
	u64 c1, c2;
	int i, count_32;

	count_32 = len >> 2;
	for (i = 0; i < count_32; i++) {
		sum1 += le32_to_cpu(buff[i]);
		sum2 += sum1;
	}

	c1 = sum1 + sum2;
	c1 = 0xFFFFFFFF - do_div(c1, 0xFFFFFFFF);
	c2 = sum1 + c1;
	c2 = 0xFFFFFFFF - do_div(c2, 0xFFFFFFFF);

	return (c2 << 32) | c1;
}

bool apfs_verify_block_csum(const void *block, size_t size)
{
	const struct apfs_obj_phys *obj = block;
	u64 header_csum;
	u64 actual_csum;

	if (size <= APFS_MAX_CKSUM_SIZE)
		return false;

	header_csum = le64_to_cpu(obj->o_cksum);
	actual_csum = apfs_fletcher64((const char *)block + APFS_MAX_CKSUM_SIZE,
				      size - APFS_MAX_CKSUM_SIZE);
	return header_csum == actual_csum;
}
