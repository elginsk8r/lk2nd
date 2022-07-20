// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <libfdt.h>
#include <stdlib.h>
#include <string.h>
#include <dev/gpio.h>
#include <target.h>

#include <lk2nd/gpio.h>
#include <lk2nd/keys.h>

#include "device.h"

#define LK2ND_MAX_KEYS 4

struct lk2nd_keymap {
	uint32_t keycode;
	uint32_t gpio;
};

static struct lk2nd_keymap keymap[LK2ND_MAX_KEYS] = {0};

#if WITH_DEV_PMIC_PM8X41 || WITH_DEV_PMIC_PM8921
extern uint32_t pm8x41_get_pwrkey_is_pressed(void);
static int target_powerkey() { return pm8x41_get_pwrkey_is_pressed(); }
#else
static int target_powerkey() { return 0; }
#endif

/**
 * lk2nd_keys_pressed() - Check if the key is pressed.
 * @keycode: Keycode to check.
 *
 * The method will first check the override keymap
 * and try to find a gpio attached to the keycode.
 * If there is no override key for the code, the
 * default target-specific key checks will be used
 * for certain keys.
 *
 * Returns: 1 if the key is pressed, 0 if not.
 *
 * If the key is absent, 0 will be returned.
 */
int lk2nd_keys_pressed(uint32_t keycode)
{
	int i = 0, st;
	bool found = false;

	for (i = 0; i < LK2ND_MAX_KEYS; ++i) {
		if (!keymap[i].keycode)
			break;

		if (keymap[i].keycode == keycode) {
			found = true;
			st = gpio_get(keymap[i].gpio);

			if (st)
				return 1;
		}
	}

	if (found)
		return 0;

	/* fallback to default handlers only if the keycode wasn't set in the DT */
	switch (keycode) {
		case KEY_VOLUMEDOWN:
			return target_volume_down();
		case KEY_VOLUMEUP:
			return target_volume_up();
		case KEY_POWER:
			return target_powerkey();
	}

	return 0;
}

static int lk2nd_keys_init(const void *dtb, int node)
{
	int i = 0, subnode, gpio, keycode, len;
	const uint32_t *val;

	dprintf(INFO, "keys: Applying keymap overrides.\n");
	dprintf(INFO, " | label | code  | gpio       |\n");
	fdt_for_each_subnode(subnode, dtb, node) {

		if (i == LK2ND_MAX_KEYS) {
			dprintf(CRITICAL, "keys: Too many keys defined, ignoring rest.\n");
			break;
		}

		gpio = lk2nd_dev_gpio_get(dtb, subnode, NULL);
		if (gpio < 0) {
			dprintf(CRITICAL, "keys: Failed to get gpio for %s: %d\n",
				fdt_get_name(dtb, subnode, NULL), gpio);
			continue;
		}

		val = fdt_getprop(dtb, subnode, "lk2nd,code", &len);
		if (len < 0) {
			dprintf(CRITICAL, "keys: Failed to get keycode for %s: %d\n",
				fdt_get_name(dtb, subnode, NULL), len);
			continue;
		}
		keycode = fdt32_to_cpu(*val);

		dprintf(INFO, " | %5s | %5d | 0x%08x |\n",
			fdt_get_name(dtb, subnode, NULL), keycode, gpio);

		keymap[i].keycode = keycode;
		keymap[i].gpio = gpio;
		i++;
	}

	if ((subnode < 0) && (subnode != -FDT_ERR_NOTFOUND)) {
		dprintf(CRITICAL, "keys: Failed to parse subnodes: %d\n", subnode);
		return 0;
	}

	return 0;
}
LK2ND_DEVICE_INIT("gpio-keys", lk2nd_keys_init);
