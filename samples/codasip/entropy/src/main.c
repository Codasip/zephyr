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

#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/entropy.h>

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

#define BUFFER_LENGTH          1025
#define RECHECK_RANDOM_ENTROPY 0x10

static uint8_t buffer[BUFFER_LENGTH] = {0};

int main(void)
{
        printk("Hello World of Entropy! %s\n", CONFIG_BOARD);

        const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_entropy));

        int ret, i;
        int count = 0;
        uint8_t num = 0xa5;

        (void)memset(buffer, num, BUFFER_LENGTH);

        /* The BUFFER_LENGTH-1 is used so the driver will not
         * write the last byte of the buffer. If that last
         * byte is not 0 on return it means the driver wrote
         * outside the passed buffer, and that should never
         * happen.
         */
        ret = entropy_get_entropy(dev, buffer, BUFFER_LENGTH - 1);
        if (ret) {
                printk("Error: entropy_get_entropy failed: %d\n", ret);
                return -1;
        }
        if (buffer[BUFFER_LENGTH - 1] != num) {
                printk("Error: entropy_get_entropy buffer overflow\n");
                return -1;
        }

        printk("Here are some random numbers:");

        for (i = 0; i < BUFFER_LENGTH - 1; i++) {
                if ((i % 16) == 0) {
                        printk("\n");
                }
                printk("0x%02x  ", buffer[i]);
                if (buffer[i] == num) {
                        count++;
                }
        }

        return 0;
}
