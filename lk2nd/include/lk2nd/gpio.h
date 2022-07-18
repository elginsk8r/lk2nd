/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_GPIO_H
#define LK2ND_GPIO_H

#include <bits.h>

#define LK2ND_GPIO_ACTIVE_LOW BIT(0)

#define LK2ND_GPIO_DEV_TLMM	1
#define LK2ND_GPIO_DEV_PM8X41	2
#define LK2ND_GPIO_DEV_PM8X41_PON	3

#define LK2ND_GPIO_PIN(num, dev, flags) \
	((num & 0xFFFF) | (dev & 0xFF) << 16 | (flags & 0xFF) << 24)

/* Controller-specific macros */
#define GPIO_PM8X41_NUM(num, sid) ((num & 0xff) | (sid & 0xff) << 8)

#define GPIO_PM8X41_PWRKEY	1
#define GPIO_PM8X41_RESIN	2

int lk2nd_dev_gpio_get(const void *dtb, int node, const char *name);

#endif /* LK2ND_GPIO_H */
