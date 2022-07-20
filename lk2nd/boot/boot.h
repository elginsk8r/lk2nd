/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_BOOT_BOOT_H
#define LK2ND_BOOT_BOOT_H

#include <list.h>
#include <string.h>

#define LK2ND_BOOT_MAX_NAME_LEN 32

/**
 * struct lk2nd_boot_action - Boot action.
 * @name: Entry name.
 * @priority: Priority of the entry.
 *            Negative values will never be auto-picked.
 * @action:   Function that will attempt the boot.
 * @data:     Action-specific data passed to the action.
 */
struct lk2nd_boot_action {
	struct list_node node;
	char name[LK2ND_BOOT_MAX_NAME_LEN];
	int priority;
	enum lk2nd_boot_aboot_action (*action)(void *data);
	void *data;
};

/* boot.c */
extern struct list_node actions_list;

void lk2nd_boot_add_action(
		char *name,
		int priority,
		enum lk2nd_boot_aboot_action (*action)(void *data),
		void *data);

/* util.c */
void lk2nd_boot_dump_devices();
void print_file_tree(char *root, char *prefix);

/* aboot.c */
void action_aboot_register();

#endif /* LK2ND_BOOT_BOOT_H */
