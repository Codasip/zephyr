/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Outputs:
Hello World! l31helium
uart0  0 IRQ number 267
gpio01 0 IRQ number 779
gpio01 1 IRQ number 1035
Etc.
*/

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

#define SERIAL DT_NODELABEL(uart0)

/* The devicetree node identifiers for "irq2" & "irq3" alias. */
#define IRQ2_NODE DT_ALIAS(irq2)
#define IRQ3_NODE DT_ALIAS(irq3)

#define SERIAL_IRQ_NUM DT_IRQ_BY_IDX(SERIAL, 0, irq)
#define IRQ2_NUM       DT_IRQ_BY_IDX(DT_NODELABEL(gpio01), 1, irq)
#define IRQ3_NUM       DT_IRQ_BY_IDX(DT_NODELABEL(gpio01), 2, irq)

/*
 * A build error on this line means your board is unsupported.
 * See the sample documentation for information on how to fix this.
 */
static const struct gpio_dt_spec irq2 = GPIO_DT_SPEC_GET(IRQ2_NODE, gpios);
static const struct gpio_dt_spec irq3 = GPIO_DT_SPEC_GET(IRQ3_NODE, gpios);

static void irq2_handler(const void *arg)
{
        /* Clear interrupt source */
        gpio_pin_set_dt(&irq2, 0);

        /* Do somthing */
        printk(" *irq2-handled* ");
}

static void irq3_handler(const void *arg)
{
        /* Clear interrupt source */
        gpio_pin_set_dt(&irq3, 0);

        /* Do somthing */
        printk(" *irq3-handled* ");
}

int main(void)
{
        int seconds = 0;
        printk("Hello World! %s\n", CONFIG_BOARD);
        printk("uart0  0 IRQ number %d\n", SERIAL_IRQ_NUM);
        printk("gpio01 2 IRQ number %d\n", IRQ2_NUM);
        printk("gpio01 3 IRQ number %d\n", IRQ3_NUM);

        if (!gpio_is_ready_dt(&irq2) || !gpio_is_ready_dt(&irq3)) {
                return 0;
        }

        if ((gpio_pin_configure_dt(&irq2, GPIO_OUTPUT_INACTIVE) < 0) ||
            (gpio_pin_configure_dt(&irq3, GPIO_OUTPUT_INACTIVE) < 0)) {
                return 0;
        }

        /* Setup IRQ handler irq2 */
        IRQ_CONNECT(IRQ2_NUM, 0, irq2_handler, NULL, 0);

        /* Setup IRQ handler irq3 */
        IRQ_CONNECT(IRQ3_NUM, 0, irq3_handler, NULL, 0);

        while (1) {
                printk("\nSeconds %d  ", seconds);

                switch (seconds % 10) {
                case 1:
                        printk("Disabling irq2 & irq3 ");
                        irq_disable(IRQ2_NUM);
                        irq_disable(IRQ3_NUM);
                        break;

                case 2:
                        printk("Enabling irq2 ");
                        irq_enable(IRQ2_NUM);
                        break;

                case 3:
                        printk("Enabling irq3 ");
                        irq_enable(IRQ3_NUM);
                        break;

                case 4:
                        printk("Firing irq2 ");
                        gpio_pin_set_dt(&irq2, 1);
                        break;

                case 5:
                        printk("Firing irq3 ");
                        gpio_pin_set_dt(&irq3, 1);
                        break;

                case 6:
                        printk("Disabling irq2 ");
                        irq_disable(IRQ2_NUM);
                        break;

                case 7:
                        printk("Firing irq2 & irq3 ");
                        gpio_pin_set_dt(&irq2, 1);
                        gpio_pin_set_dt(&irq3, 1);
                        break;

                case 8:
                        printk("Disabling irq3 ");
                        irq_disable(IRQ3_NUM);
                        break;

                case 9:
                        printk("Firing irq2 & irq3 ");
                        gpio_pin_set_dt(&irq2, 1);
                        gpio_pin_set_dt(&irq3, 1);
                        break;

                case 0:
                        printk("Enabling irq2 & irq3 ");
                        irq_enable(IRQ2_NUM);
                        irq_enable(IRQ3_NUM);
                        break;
                }

                k_msleep(SLEEP_TIME_MS);
                seconds++;
        }

        return 0;
}
