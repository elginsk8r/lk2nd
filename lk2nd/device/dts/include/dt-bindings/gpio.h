/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _DT_BINDINGS_GPIO_H
#define _DT_BINDINGS_GPIO_H

/* BIT(24) defines polarity */
#define GPIO_ACTIVE_HIGH 0
#define GPIO_ACTIVE_LOW	 (1 << 24)

/* BIT(8) and BIT(9) define pull */
#define GPIO_PULL_UP	 (1 << 8)
#define GPIO_PULL_DOWN	 (1 << 9)

#endif /* _DT_BINDINGS_GPIO_H */
