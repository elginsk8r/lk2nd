// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <stdint.h>

#if WITH_DEV_PMIC_PM8921

#include <bits.h>
#include <debug.h>
#include <dev/gpio.h>
#include <dev/pm8921.h>
#include <config.h>

#include <lk2nd/gpio.h>

#include "gpio.h"

#define NUM_PIN(num)   BITS_SHIFT(num, 7, 0)
#define NUM_SID(num)   BITS_SHIFT(num, 15, 8)

int lk2nd_gpio_pm8921_config(uint32_t num, uint32_t flags)
{
	struct pm8921_gpio pm_gpio = {
		.function  = 0,
		.vin_sel   = 2,
	};

	if (flags & GPIO_PULLUP)
		pm_gpio.pull = PM_GPIO_PULL_UP_1_5;
	else if (flags & GPIO_PULLDOWN)
		pm_gpio.pull = PM_GPIO_PULL_DN;
	else
		pm_gpio.pull = PM_GPIO_PULL_NO;

	if (flags & GPIO_OUTPUT)
		pm_gpio.direction = PM_GPIO_DIR_OUT;
	else
		pm_gpio.direction = PM_GPIO_DIR_IN;

	return pm8921_gpio_config(NUM_PIN(num), &pm_gpio);
}

void lk2nd_gpio_pm8921_set(uint32_t num, uint32_t on) { }

int lk2nd_gpio_pm8921_get(uint32_t num)
{
	uint8_t status;

	pm8921_gpio_get(NUM_PIN(num), &status);

	return status;
}

int lk2nd_gpio_pm8921_pon_get(uint32_t num)
{
	uint8_t status = 0;

	switch (NUM_PIN(num)) {
	case GPIO_PM8X41_PWRKEY:
		pm8921_pwrkey_status(&status);
		return status;
	}

	dprintf(CRITICAL, "gpio_pm8921: PON key %d is unknown.", NUM_PIN(num));
	return status;
}

#else

int lk2nd_gpio_pm8921_config(uint32_t num, uint32_t flags) { return -1; }
void lk2nd_gpio_pm8921_set(uint32_t num, uint32_t on) { }
int lk2nd_gpio_pm8921_get(uint32_t num) { return 0; }
int lk2nd_gpio_pm8921_pon_get(uint32_t num) { return 0; }

#endif
