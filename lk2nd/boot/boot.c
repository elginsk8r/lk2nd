// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <list.h>
#include <kernel/mutex.h>
#include <lib/bio.h>
#include <lib/fs.h>

#include <lk2nd/boot.h>

#include "boot.h"

static void dump_devices()
{
	struct bdev_struct *bdevs = bio_get_bdevs();
	bdev_t *entry;

	dprintf(INFO, "block devices:\n");
	mutex_acquire(&bdevs->lock);
	dprintf(INFO, " | dev    | label      | size       | Sub |\n");
	list_for_every_entry(&bdevs->list, entry, bdev_t, node) {
		dprintf(INFO, " | %-6s | %-10s | %6lld %s | %-3s |\n",
			entry->name,
			(entry->label ? entry->label : ""),
			entry->size / (entry->size > 1024 * 1024 ? 1024*1024 : 1024),
			(entry->size > 1024 * 1024 ? "MiB" : "KiB"),
			(entry->is_subdev ? "Yes" : "")
			);
	}
	mutex_release(&bdevs->lock);
}

/**
 * lk2nd_boot_init() - Prepare lk2nd boot.
 *
 * This method is called early in aboot to do any preparations
 * or dump some debug information to the log.
 */
void lk2nd_boot_init()
{
	dprintf(INFO, "boot: Init\n");
	dump_devices();
}
