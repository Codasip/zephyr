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

#if defined(CONFIG_MBEDTLS_HMAC_DRBG_ENABLED)
#include "mbedtls/hmac_drbg.h"

#else
#error Need to set "CONFIG_MBEDTLS_HMAC_DRBG_ENABLED=y" in the prj.conf file
#endif

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
#include "mbedtls/memory_buffer_alloc.h"

unsigned char buf[16000];
#endif

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

#define BUFFER_LENGTH          1025
#define RECHECK_RANDOM_ENTROPY 0x10

static uint8_t buffer[BUFFER_LENGTH] = {0};

int main(void)
{
        int i;
        const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_entropy));
        mbedtls_hmac_drbg_context ctx;
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);

        printk("Hello World of HMAC DRBG Entropy using TRNG! %s\n", CONFIG_BOARD);

#if defined(MBEDTLS_MEMORY_BUFFER_ALLOC_C)
        mbedtls_memory_buffer_alloc_init(buf, sizeof(buf));
#endif

        mbedtls_hmac_drbg_init(&ctx);

#if 0
	/* Non-Entropy software RNG as used in mbedtls_hmac_drbg_self_test() in hmac_drbg.c */
    mbedtls_hmac_drbg_seed( &ctx, md_info,
                            hmac_drbg_self_test_entropy, (void *) entropy_pr,
                            NULL, 0 );
#else

        /* Using Codasip Entropy TRNG as the seed */
        mbedtls_hmac_drbg_seed(&ctx, md_info,
                               (int (*)(void *, unsigned char *, size_t))entropy_get_entropy,
                               (void *)dev, NULL, 0);
#endif

        mbedtls_hmac_drbg_set_prediction_resistance(&ctx, MBEDTLS_HMAC_DRBG_PR_ON);

        mbedtls_hmac_drbg_random(&ctx, buffer, BUFFER_LENGTH - 1);

        printk("Here are some HMAC DRBG random numbers (with prediction resistance) seeded by "
               "Codasip's TRNG:");

        for (i = 0; i < BUFFER_LENGTH - 1; i++) {
                if ((i % 16) == 0) {
                        printk("\n");
                }
                printk("0x%02x  ", buffer[i]);
        }

        mbedtls_hmac_drbg_free(&ctx);

        return 0;
}
