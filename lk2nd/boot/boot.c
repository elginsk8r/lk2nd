// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <stdlib.h>
#include <debug.h>
#include <list.h>
#include <lib/bio.h>
#include <lib/fs.h>

#include <lk2nd/boot.h>

#include "boot.h"

struct list_node actions_list;

/**
 * lk2nd_boot_add_action() - Register a new boot action.
 */
void lk2nd_boot_add_action( char *name,	int priority,
		enum lk2nd_boot_aboot_action (*action)(void *data), void *data)
{
	struct lk2nd_boot_action *act = malloc(sizeof(*act));
	strncpy(act->name, name, LK2ND_BOOT_MAX_NAME_LEN-1);
	act->priority = priority;
	act->action = action;
	act->data = data;

	list_add_tail(&actions_list, &act->node);
}

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
	struct lk2nd_boot_action *entry, *best = NULL;
	struct bdev_struct *bdevs = bio_get_bdevs();
	char mountpoint[16];
	bdev_t *bdev;
	int ret;

	dprintf(INFO, "boot: Trying to boot...\n");

	/*
	 * Step 1: Survey for all available boot entries.
	 */

	list_for_every_entry(&bdevs->list, bdev, bdev_t, node) {

		/* Skip top level block devices. */
		if (!bdev->is_subdev)
			continue;

		strcpy(mountpoint, "/");
		strncat(mountpoint, bdev->name, 14);

		ret = fs_mount(mountpoint, "ext2", bdev->name);
		if (ret < 0)
			continue;

		dprintf(INFO, "Scanning %s ...\n", bdev->name);

		dprintf(INFO, "%s\n", mountpoint);
		print_file_tree(mountpoint, "");
	}

	/* aboot actions go last in the list. */
	action_aboot_register();

	/*
	 * Step 2: Pick the action. (e.g. show the menu or find default)
	 */

	dprintf(INFO, "boot: Available entries:\n");

	dprintf(INFO, " | %-32s | Prio |\n", "Name");
	list_for_every_entry(&actions_list, entry, struct lk2nd_boot_action, node) {
		dprintf(INFO, " | %-32s | %4d |\n", entry->name, entry->priority);
		if (!best || entry->priority > best->priority)
			best = entry;
	}

	if (best) {
		dprintf(INFO, "boot: Picked %s (%d)\n", best->name, best->priority);
		return best->action(best->data);
	}

	dprintf(CRITICAL, "boot: No action was performed, requesting fastboot\n");
	return LK2ND_ABOOT_ACTION_FASTBOOT;
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
	list_initialize(&actions_list);
	lk2nd_boot_dump_devices();
}
