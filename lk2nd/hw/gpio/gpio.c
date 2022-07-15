// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <bits.h>
#include <debug.h>
#include <dev/gpio.h>
#include <stdint.h>

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

	default:
		dprintf(CRITICAL, "gpio_get: device %d is not known.\n", NR_DEV(nr));
	}

	return (NR_FLAGS(nr) & LK2ND_GPIO_ACTIVE_LOW ? !val : !!val);
}
