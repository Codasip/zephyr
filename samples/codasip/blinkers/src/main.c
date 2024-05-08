/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)
#define LED4_NODE DT_ALIAS(led4)
#define LED5_NODE DT_ALIAS(led5)
#define LED6_NODE DT_ALIAS(led6)
#define LED7_NODE DT_ALIAS(led7)

#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
#define SW2_NODE DT_ALIAS(sw2)
#define SW3_NODE DT_ALIAS(sw3)
#define SW4_NODE DT_ALIAS(sw4)
#define SW5_NODE DT_ALIAS(sw5)
#define SW6_NODE DT_ALIAS(sw6)
#define SW7_NODE DT_ALIAS(sw7)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec ld0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec ld1 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec ld2 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec ld3 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);
static const struct gpio_dt_spec ld4 = GPIO_DT_SPEC_GET(LED4_NODE, gpios);
static const struct gpio_dt_spec ld5 = GPIO_DT_SPEC_GET(LED5_NODE, gpios);
static const struct gpio_dt_spec ld6 = GPIO_DT_SPEC_GET(LED6_NODE, gpios);
static const struct gpio_dt_spec ld7 = GPIO_DT_SPEC_GET(LED7_NODE, gpios);

static const struct gpio_dt_spec sw0 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec sw1 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
static const struct gpio_dt_spec sw2 = GPIO_DT_SPEC_GET(SW2_NODE, gpios);
static const struct gpio_dt_spec sw3 = GPIO_DT_SPEC_GET(SW3_NODE, gpios);
static const struct gpio_dt_spec sw4 = GPIO_DT_SPEC_GET(SW4_NODE, gpios);
static const struct gpio_dt_spec sw5 = GPIO_DT_SPEC_GET(SW5_NODE, gpios);
static const struct gpio_dt_spec sw6 = GPIO_DT_SPEC_GET(SW6_NODE, gpios);
static const struct gpio_dt_spec sw7 = GPIO_DT_SPEC_GET(SW7_NODE, gpios);

const struct device *gpio00_dev = DEVICE_DT_GET(DT_NODELABEL(gpio00));

int main(void)
{
        if (!gpio_is_ready_dt(&ld0) | !gpio_is_ready_dt(&ld1) | !gpio_is_ready_dt(&ld2) |
            !gpio_is_ready_dt(&ld3) | !gpio_is_ready_dt(&ld4) | !gpio_is_ready_dt(&ld5) |
            !gpio_is_ready_dt(&ld6) | !gpio_is_ready_dt(&ld7)

            | !gpio_is_ready_dt(&sw0) | !gpio_is_ready_dt(&sw1) | !gpio_is_ready_dt(&sw2) |
            !gpio_is_ready_dt(&sw3) | !gpio_is_ready_dt(&sw4) | !gpio_is_ready_dt(&sw5) |
            !gpio_is_ready_dt(&sw6) | !gpio_is_ready_dt(&sw7)) {
                return 0;
        }

        if ((gpio_pin_configure_dt(&ld0, GPIO_OUTPUT_ACTIVE) < 0) |
            (gpio_pin_configure_dt(&ld1, GPIO_OUTPUT_ACTIVE) < 0) |
            (gpio_pin_configure_dt(&ld2, GPIO_OUTPUT_ACTIVE) < 0) |
            (gpio_pin_configure_dt(&ld3, GPIO_OUTPUT_ACTIVE) < 0) |
            (gpio_pin_configure_dt(&ld4, GPIO_OUTPUT_ACTIVE) < 0) |
            (gpio_pin_configure_dt(&ld5, GPIO_OUTPUT_ACTIVE) < 0) |
            (gpio_pin_configure_dt(&ld6, GPIO_OUTPUT_ACTIVE) < 0) |
            (gpio_pin_configure_dt(&ld7, GPIO_OUTPUT_ACTIVE) < 0)

            | (gpio_pin_configure_dt(&sw0, GPIO_INPUT) < 0) |
            (gpio_pin_configure_dt(&sw1, GPIO_INPUT) < 0) |
            (gpio_pin_configure_dt(&sw2, GPIO_INPUT) < 0) |
            (gpio_pin_configure_dt(&sw3, GPIO_INPUT) < 0) |
            (gpio_pin_configure_dt(&sw4, GPIO_INPUT) < 0) |
            (gpio_pin_configure_dt(&sw5, GPIO_INPUT) < 0) |
            (gpio_pin_configure_dt(&sw6, GPIO_INPUT) < 0) |
            (gpio_pin_configure_dt(&sw7, GPIO_INPUT) < 0)) {
                return 0;
        }

        while (1) {
                if (gpio_pin_get_dt(&sw0) && (gpio_pin_toggle_dt(&ld0) < 0)) {
                        return 0;
                }
                if (gpio_pin_get_dt(&sw1) && (gpio_pin_toggle_dt(&ld1) < 0)) {
                        return 0;
                }
                if (gpio_pin_get_dt(&sw2) && (gpio_pin_toggle_dt(&ld2) < 0)) {
                        return 0;
                }
                if (gpio_pin_get_dt(&sw3) && (gpio_pin_toggle_dt(&ld3) < 0)) {
                        return 0;
                }
                if (gpio_pin_get_dt(&sw4) && (gpio_pin_toggle_dt(&ld4) < 0)) {
                        return 0;
                }
                if (gpio_pin_get_dt(&sw5) && (gpio_pin_toggle_dt(&ld5) < 0)) {
                        return 0;
                }

                /* Test the other GPIO functions with bits 6 & 7 */
                uint32_t value;

                /* Test RAW port get and set with bit 6 */
                gpio_port_get_raw(gpio00_dev, &value);               /* Read sw7-sw0 */
                gpio_port_set_masked_raw(gpio00_dev, 1 << 6, value); /* Set ld6 with sw6 value */

                /* Test _set_bits_raw and _clear_bit_raw using bit 7 */
                if (value & (1 << 7)) /* Test sw7 value */
                {
                        gpio_port_set_bits_raw(gpio00_dev, 1 << 7); /* Set ld7 */
                } else {
                        gpio_port_clear_bits_raw(gpio00_dev, 1 << 7); /* Clear ld7 */
                }

                k_msleep(SLEEP_TIME_MS);
        }
        return 0;
}
