// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>

#include <lk2nd/boot.h>

#include "boot.h"

/**
 * lk2nd_boot_init() - Prepare lk2nd boot.
 *
 * This method is called early in aboot to do any preparations
 * or dump some debug information to the log.
 */
void lk2nd_boot_init()
{
	dprintf(INFO, "boot: Init\n");
}
