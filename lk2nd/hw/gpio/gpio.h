/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef HW_GPIO_H
#define HW_GPIO_H

#include <stdint.h>

/* TLMM gpios */
int lk2nd_gpio_tlmm_config(uint32_t num, uint32_t flags);
void lk2nd_gpio_tlmm_set(uint32_t num, uint32_t on);
int lk2nd_gpio_tlmm_get(uint32_t num);

#endif /* HW_GPIO_H */
