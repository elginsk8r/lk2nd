// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <bits.h>
#include <debug.h>
#include <dev/gpio.h>
#include <stdint.h>
#include <string.h>
#include <libfdt.h>

#include <lk2nd/gpio.h>

#include "gpio.h"

#define NR_PIN(nr)   BITS_SHIFT(nr, 15, 0)
#define NR_DEV(nr)   BITS_SHIFT(nr, 23, 16)
#define NR_FLAGS(nr) BITS_SHIFT(nr, 31, 24)

/**
 * gpio_config() - Set the GPIO configuration.
 * @nr:    GPIO number.
 *         The number also includes the gpio-chip id as
 *         well as some flags that the user might need.
 * @flags: Configuration flags.
 *
 * Return: 0 or success, -1 on failure.
 */
int gpio_config(uint32_t nr, uint32_t flags)
{
	switch (NR_DEV(nr)) {
	case 0:
	case LK2ND_GPIO_DEV_TLMM:
		return lk2nd_gpio_tlmm_config(NR_PIN(nr), flags);

	case LK2ND_GPIO_DEV_PM8X41:
		return lk2nd_gpio_pm8x41_config(NR_PIN(nr), flags);

	case LK2ND_GPIO_DEV_PM8X41_PON:
		return lk2nd_gpio_pm8x41_pon_config(NR_PIN(nr), flags);

	default:
		dprintf(CRITICAL, "gpio_config: device %d is not known.\n", NR_DEV(nr));
	}

	return -1;
}

/**
 * gpio_set() - Set the GPIO state.
 * @nr:    GPIO number.
 * @on:    GPIO shall be active.
 *
 * The "active" level is encoded in the nr.
 */
void gpio_set(uint32_t nr, uint32_t on)
{
	int val = (NR_FLAGS(nr) & LK2ND_GPIO_ACTIVE_LOW ? !on : !!on);

	switch (NR_DEV(nr)) {
	case 0:
	case LK2ND_GPIO_DEV_TLMM:
		lk2nd_gpio_tlmm_set(NR_PIN(nr), val);
		break;

	case LK2ND_GPIO_DEV_PM8X41:
		lk2nd_gpio_pm8x41_set(NR_PIN(nr), val);
		break;

	case LK2ND_GPIO_DEV_PM8X41_PON:
		lk2nd_gpio_pm8x41_pon_set(NR_PIN(nr), val);
		break;

	default:
		dprintf(CRITICAL, "gpio_set: device %d is not known.\n", NR_DEV(nr));
	}
}

/**
 * gpio_set() - Set the GPIO state.
 * @nr:    GPIO number.
 *
 * Returns: 1 if the GPIO is "active", 0 otherwise.
 *
 * If a failure arizes while attempting to read the
 * GPIO state, the return value will be "not active".
 */
int gpio_get(uint32_t nr)
{
	int val = 0;

	switch (NR_DEV(nr)) {
	case 0:
	case LK2ND_GPIO_DEV_TLMM:
		val = lk2nd_gpio_tlmm_get(NR_PIN(nr));
		break;

	case LK2ND_GPIO_DEV_PM8X41:
		val = lk2nd_gpio_pm8x41_get(NR_PIN(nr));
		break;

	case LK2ND_GPIO_DEV_PM8X41_PON:
		val = lk2nd_gpio_pm8x41_pon_get(NR_PIN(nr));
		break;

	default:
		dprintf(CRITICAL, "gpio_get: device %d is not known.\n", NR_DEV(nr));
	}

	return (NR_FLAGS(nr) & LK2ND_GPIO_ACTIVE_LOW ? !val : !!val);
}

/**
 * lk2nd_dev_gpio_get() - Get gpio number from the DT definition.
 * @dtb:  pointer to the DT.
 * @node: Offset of the node containing the foo-gpios property.
 * @name: Name of the gpio in the DT node (i.e. foo for foo-gpios).
 *        name can be NULL for a simple "gpios" property. 
 *
 * Returns: GPIO nr that can be used for GPIO operations.
 *
 * If an error occurs, a negative value will be returned.
 */
int lk2nd_dev_gpio_get(const void *dtb, int node, const char *name)
{
	char prop_name[32 + 7] = "gpios";
	const uint32_t *prop_val;
	uint32_t nr, num, dev, flags;
	int len, ret;

	if (name) {
		strncpy(prop_name, name, 32);
		strcat(prop_name, "-gpios");
	}

	prop_val = fdt_getprop(dtb, node, prop_name, &len);

	if (len < 0) {
		dprintf(CRITICAL, "gpio_get failed: Unable to get property: %d\n", len);
		return len;
	} else if (len != 3 * sizeof(uint32_t)) {
		dprintf(CRITICAL, "gpio_get failed: gpios arrays are not supported. (len=%d)\n", len);
		return -1;
	}

	num = fdt32_to_cpu(prop_val[1]);
	dev = fdt32_to_cpu(prop_val[0]) & 0xff;
	flags = fdt32_to_cpu(prop_val[2]);

	/* Last byte is used for persisstent flags */
	nr = LK2ND_GPIO_PIN(num, dev, NR_FLAGS(flags));

	/* First 3 bytes are initial configuration */
	ret = gpio_config(nr, flags & 0xffffff);
	if (ret < 0) {
		dprintf(CRITICAL, "gpio_get failed: Unable to configure gpio: %d\n", len);
		return ret;
	}

	return nr;
}
