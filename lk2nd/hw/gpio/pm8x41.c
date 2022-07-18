// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <stdint.h>

#if WITH_DEV_PMIC_PM8X41

#include <bits.h>
#include <debug.h>
#include <dev/gpio.h>
#include <pm8x41.h>
#include <config.h>

#include <lk2nd/gpio.h>

#include "gpio.h"

#define NUM_PIN(num)   BITS_SHIFT(num, 7, 0)
#define NUM_SID(num)   BITS_SHIFT(num, 15, 8)

int lk2nd_gpio_pm8x41_config(uint32_t num, uint32_t flags)
{
	struct pm8x41_gpio pm_gpio = {
		.function  = 0,
		.vin_sel   = 2,
	};

	if (flags & GPIO_PULLUP)
		pm_gpio.pull = PM_GPIO_PULL_UP_1_5;
	else if (flags & GPIO_PULLDOWN)
		pm_gpio.pull = PM_GPIO_PULLDOWN_10;
	else
		pm_gpio.pull = PM_GPIO_NO_PULL;

	if (flags & GPIO_OUTPUT)
		pm_gpio.direction = PM_GPIO_DIR_OUT;
	else
		pm_gpio.direction = PM_GPIO_DIR_IN;

	return pm8x41_gpio_config_sid(NUM_SID(num), NUM_PIN(num), &pm_gpio);
}

void lk2nd_gpio_pm8x41_set(uint32_t num, uint32_t on)
{
	pm8x41_gpio_set_sid(NUM_SID(num), NUM_PIN(num), on);
}

int lk2nd_gpio_pm8x41_get(uint32_t num)
{
	uint8_t status;

	pm8x41_gpio_get_sid(NUM_SID(num), NUM_PIN(num), &status);

	return status;
}

int lk2nd_gpio_pm8x41_pon_get(uint32_t num)
{
	switch (NUM_PIN(num)) {
		case GPIO_PM8X41_PWRKEY:
			return pm8x41_get_pwrkey_is_pressed();

		case GPIO_PM8X41_RESIN:
			return pm8x41_resin_status();
	}

	dprintf(CRITICAL, "gpio_pm8x41: PON key %d is unknown.", NUM_PIN(num));

	return 0;
}

#else

int lk2nd_gpio_pm8x41_config(uint32_t num, uint32_t flags) { return -1; }
void lk2nd_gpio_pm8x41_set(uint32_t num, uint32_t on) { }
int lk2nd_gpio_pm8x41_get(uint32_t num) { return 0; }
int lk2nd_gpio_pm8x41_pon_get(uint32_t num) { return 0; }

#endif
