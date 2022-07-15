/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_GPIO_H
#define LK2ND_GPIO_H

#include <bits.h>

#define LK2ND_GPIO_ACTIVE_LOW BIT(0)

#define LK2ND_GPIO_DEV_TLMM	1

#define LK2ND_GPIO_PIN(num, dev, flags) \
	((num & 0xFFFF) | (dev & 0xFF) << 16 | (flags & 0xFF) << 24)

#endif /* LK2ND_GPIO_H */
