// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <dev/flash.h>
#include <lib/ptable.h>
#include <partition_parser.h>
#include <string.h>
#include <target.h>

#include <lk2nd/init.h>
#include "device.h"

static void partition_split_mmc(const char *base_name, const char *name,
				uint32_t num_blocks, bool end)
{
	struct partition_entry *backup, *base, *split;
	int index = partition_get_index(base_name);

	if (index == INVALID_PTN) {
		dprintf(CRITICAL, "%s partition not found (as base for %s)\n",
			base_name, name);
		return;
	}
	base = &partition_get_partition_entries()[index];

	if (base->size < num_blocks) {
		dprintf(CRITICAL, "%s partition has not enough space for %s (%llu < %u)\n",
			base_name, name, base->size, num_blocks);
		return;
	}

	backup = partition_allocate();
	if (backup) {
		memcpy(backup, base, sizeof(*backup));
		snprintf((char*)backup->name, sizeof(backup->name), "real_%s", base_name);
	} else {
		dprintf(CRITICAL, "Too many partitions, cannot backup %s partition entry\n",
			base_name);
	}

	split = partition_allocate();
	if (split) {
		memcpy(split, base, sizeof(*split));
		strlcpy((char*)split->name, name, sizeof(split->name));
		if (end)
			split->first_lba = split->last_lba - num_blocks + 1;
		else
			split->last_lba = split->first_lba + num_blocks - 1;
		split->size = num_blocks;
	} else {
		dprintf(CRITICAL, "Too many partitions, cannot add virtual %s partition\n",
			name);
	}

	if (end)
		base->last_lba -= num_blocks;
	else
		base->first_lba += num_blocks;
	base->size -= num_blocks;
}

static void partition_rename_mmc(const char *base_name, const char *name)
{
	struct partition_entry *base;
	int index = partition_get_index(base_name);

	if (index == INVALID_PTN) {
		dprintf(CRITICAL, "%s partition not found (as base for %s)\n",
			base_name, name);
		return;
	}
	base = &partition_get_partition_entries()[index];

	strlcpy((char*)base->name, name, sizeof(base->name));
}

static void partition_split_flash(struct ptable *ptable, const char *base_name,
				  const char *name, unsigned length, bool end)
{
	struct ptentry *base = ptable_find(ptable, base_name);

	if (!base) {
		dprintf(CRITICAL, "%s partition not found (as base for %s)\n",
			base_name, name);
		return;
	}

	if (base->length < length) {
		dprintf(CRITICAL, "%s partition has not enough space for %s (%u < %u)\n",
			base_name, name, base->length, length);
		return;
	}

	if (ptable_size(ptable) < MAX_PTABLE_PARTS) {
		char backup_name[MAX_PTENTRY_NAME];
		snprintf(backup_name, sizeof(backup_name), "real_%s", base_name);
		ptable_add(ptable, backup_name, base->start, base->length,
				base->flags, base->type, base->perm);
	} else {
		dprintf(CRITICAL, "Too many partitions, cannot backup %s partition entry\n",
			base_name);
	}

	if (ptable_size(ptable) < MAX_PTABLE_PARTS) {
		unsigned start = base->start;
		if (end)
			start += base->length - length;

		ptable_add(ptable, (char*)name, start, length, base->flags,
			   base->type, base->perm);
	} else {
		dprintf(CRITICAL, "Too many partitions, cannot add virtual %s partition\n",
			name);
	}

	if (!end)
		base->start += length;
	base->length -= length;
}

static void lk2nd_partition_split_mmc(void)
{
	uint32_t block_size __UNUSED = mmc_get_device_blocksize();

	bool use_alt_boot = lk2nd_dev.android_partitions.boot;
	if (lk2nd_dev.android_partitions.boot) {
		partition_rename_mmc(LK2ND_BOOT_PARTITION_BASE, LK2ND_BOOT_PARTITION_NAME);
		partition_rename_mmc(lk2nd_dev.android_partitions.boot, LK2ND_BOOT_PARTITION_BASE);
	} else if (LK2ND_BOOT_PARTITION_SIZE > 0) {
		partition_split_mmc(LK2ND_BOOT_PARTITION_BASE,
					LK2ND_BOOT_PARTITION_NAME,
					LK2ND_BOOT_PARTITION_SIZE / block_size, false);
	}

	if (lk2nd_dev.android_partitions.recovery || lk2nd_dev.android_partitions.recovery_is_boot) {
		partition_rename_mmc(LK2ND_RECOVERY_PARTITION_BASE, LK2ND_RECOVERY_PARTITION_NAME);
		if (lk2nd_dev.android_partitions.recovery) {
			partition_rename_mmc(lk2nd_dev.android_partitions.recovery, LK2ND_RECOVERY_PARTITION_BASE);
		}
	} else if (LK2ND_RECOVERY_PARTITION_SIZE > 0) {
		partition_split_mmc(LK2ND_RECOVERY_PARTITION_BASE,
					LK2ND_RECOVERY_PARTITION_NAME,
					LK2ND_RECOVERY_PARTITION_SIZE / block_size, false);
	}

	if (lk2nd_dev.android_partitions.misc) {
		partition_rename_mmc(lk2nd_dev.android_partitions.misc, "misc");
	}
}

static void lk2nd_partition_split_flash(void)
{
	struct ptable *ptable = flash_get_ptable();
	unsigned block_size __UNUSED = flash_block_size();

	if (!ptable)
		return;

	if (LK2ND_BOOT_PARTITION_SIZE > 0) {
		partition_split_flash(ptable, LK2ND_BOOT_PARTITION_BASE,
					  LK2ND_BOOT_PARTITION_NAME,
					  LK2ND_BOOT_PARTITION_SIZE / block_size, false);
	}

	if (LK2ND_RECOVERY_PARTITION_SIZE > 0) {
		partition_split_flash(ptable, LK2ND_RECOVERY_PARTITION_BASE,
					  LK2ND_RECOVERY_PARTITION_NAME,
					  LK2ND_RECOVERY_PARTITION_SIZE / block_size, false);
	}
}

static void lk2nd_device2nd_partition_split(void)
{
	if (target_is_emmc_boot())
		lk2nd_partition_split_mmc();
	else
		lk2nd_partition_split_flash();
}
LK2ND_INIT(lk2nd_device2nd_partition_split);
