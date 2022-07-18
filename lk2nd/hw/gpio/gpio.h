/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef HW_GPIO_H
#define HW_GPIO_H

#include <stdint.h>

/* TLMM gpios */
int lk2nd_gpio_tlmm_config(uint32_t num, uint32_t flags);
void lk2nd_gpio_tlmm_set(uint32_t num, uint32_t on);
int lk2nd_gpio_tlmm_get(uint32_t num);

/* pm8x41 gpios */
int lk2nd_gpio_pm8x41_config(uint32_t num, uint32_t flags);
void lk2nd_gpio_pm8x41_set(uint32_t num, uint32_t on);
int lk2nd_gpio_pm8x41_get(uint32_t num);

/* pm8x41 PON lines */
static inline int lk2nd_gpio_pm8x41_pon_config(uint32_t num, uint32_t flags) { return 0; };
static inline void lk2nd_gpio_pm8x41_pon_set(uint32_t num, uint32_t on) { };
int lk2nd_gpio_pm8x41_pon_get(uint32_t num);

/* pm8921 gpios */
int lk2nd_gpio_pm8921_config(uint32_t num, uint32_t flags);
void lk2nd_gpio_pm8921_set(uint32_t num, uint32_t on);
int lk2nd_gpio_pm8921_get(uint32_t num);

/* pm8921 PON lines */
static inline int lk2nd_gpio_pm8921_pon_config(uint32_t num, uint32_t flags) { return 0; };
static inline void lk2nd_gpio_pm8921_pon_set(uint32_t num, uint32_t on) { };
int lk2nd_gpio_pm8921_pon_get(uint32_t num);

#endif /* HW_GPIO_H */
