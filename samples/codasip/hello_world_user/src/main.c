/*
 * Copyright (c) 2020 BayLibre, SAS
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <stdio.h>
#define USER_STACKSIZE 2048
#define SLEEP_TIME_MS  1000

struct k_thread user_thread;
K_THREAD_STACK_DEFINE(user_stack, USER_STACKSIZE);

static void user_function(void *p1, void *p2, void *p3)
{
        volatile int *ptr = NULL;

        printf("Hello World from %s (%s)\n",
               k_is_user_context() ? "UserSpace!" : "privileged mode.", CONFIG_BOARD);
        __ASSERT(k_is_user_context(), "User mode execution was expected");

        printf("Reading data from address 0x00 - This should fail with 'E:  mcause: 5, Load access "
               "fault'\n");
        k_msleep(SLEEP_TIME_MS);

        printf("Read %d from address 0x00\n", *ptr);
}

int main(void)
{
        k_thread_create(&user_thread, user_stack, USER_STACKSIZE, user_function, NULL, NULL, NULL,
                        -1, K_USER, K_MSEC(0));
        return 0;
}
