/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_BOOT_H
#define LK2ND_BOOT_H

enum lk2nd_boot_aboot_action {
	LK2ND_ABOOT_ACTION_NONE,
	LK2ND_ABOOT_ACTION_BOOT,
	LK2ND_ABOOT_ACTION_RECOVERY,
	LK2ND_ABOOT_ACTION_FASTBOOT
};

void lk2nd_boot_init();
enum lk2nd_boot_aboot_action lk2nd_boot_do_action();

#endif /* LK2ND_BOOT_H */
