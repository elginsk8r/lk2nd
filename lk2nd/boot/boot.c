// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <list.h>
#include <kernel/mutex.h>
#include <lib/bio.h>
#include <lib/fs.h>

#include <lk2nd/boot.h>

#include "boot.h"

/**
 * lk2nd_boot_do_action() - Boot the OS.
 *
 * This is a top-level boot application method that surveys all
 * possible boot targets, possibly shows the selection menu and
 * performs the boot of the selected option. It can also instruct
 * aboot to some action if needed.
 *
 * Return: The method never returns if the boot action succeds.
 * If the selected boot action returns, it's action code will be
 * returned to correct the aboot behavior. If no action fires,
 * LK2ND_ABOOT_ACTION_FASTBOOT is returned.
 */
enum lk2nd_boot_aboot_action lk2nd_boot_do_action()
{
	dprintf(INFO, "boot: Trying to boot...\n");

	dprintf(CRITICAL, "boot: No action was performed, requesting fastboot\n");
	return LK2ND_ABOOT_ACTION_FASTBOOT;
}

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
