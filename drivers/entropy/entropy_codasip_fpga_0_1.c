/*
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT codasip_fpga_trng_0_1

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/entropy.h>

/**
 * Codasip s.r.o.
 *
 * CONFIDENTIAL
 *
 * Copyright 2023 Codasip s.r.o.
 *
 * All Rights Reserved.
 * This file is part of a Codasip product. No part of this file may be use, copied,
 * modified, or distributed except in accordance with the terms contained in the
 * Codasip license agreement under which you obtained this file.
 */

#include <stdbool.h>
#include <stdint.h>

/** \brief Structure representing address space of a TRNG peripheral */
typedef struct {
    volatile uint32_t RAW;
    volatile uint32_t RAWN;
    volatile uint32_t RND;
    volatile uint32_t RNDN;
    volatile uint32_t CONFIG;
    volatile uint32_t STATUS;
} TRNG_Type;

/** \brief Structure holding data neccessary to service the TRNG peripheral. */
typedef struct {
    TRNG_Type *regs; ///< Pointer to the peripheral registers
} bm_trng_t;

#if 0 /* Not used */
/**
 * \brief Retrieve a raw data sample
 *
 * \param trng TRNG peripheral
 *
 * \return Raw data
 */
static
uint32_t bm_trng_get_raw(bm_trng_t *trng);
#endif

/**
 * \brief Retrieve a processed, random, data sample
 *
 * \param trng TRNG peripheral
 *
 * \return Random data
 */
static
uint32_t bm_trng_get_rnd(bm_trng_t *trng);

#if 0 /* Not used */
/**
 * \brief Read TRNG status
 *
 * \param trng TRNG peripheral
 *
 * \return Number of errors
 */
static
uint32_t bm_trng_get_status(bm_trng_t *trng);
#endif

#if 0 /* Not used */
/**
 * \brief Configure TRNG peripheral
 *
 * \param trng TRNG peripheral
 * \param sample_teros Sample the TERO at this count
 * \param clear_teros Clear the TERO at this count
 * \param wrap_counter Wrap the counter at this count
 * \param disable_unbiasing Disable un-biasing
 * \param disable_checking Disable error checking
 */
static
void bm_trng_configure(bm_trng_t *trng,
                       uint8_t    sample_teros,
                       uint8_t    clear_teros,
                       uint8_t    wrap_counter,
                       bool       disable_unbiasing,
                       bool       disable_checking);
#endif


#if 0 /* Not used */
static
uint32_t bm_trng_get_raw(bm_trng_t *trng)
{
    // Wait for available sample
    while (!trng->regs->RAWN)
        ;
    return trng->regs->RAW;
}
#endif

static
uint32_t bm_trng_get_rnd(bm_trng_t *trng)
{
    // Wait for available sample
    while (!trng->regs->RNDN)
        ;
    return trng->regs->RND;
}

#if 0 /* Not used */
static
uint32_t bm_trng_get_status(bm_trng_t *trng)
{
    return trng->regs->STATUS;
}
#endif

#if 0 /* Not used */
static
void bm_trng_configure(bm_trng_t *trng,
                       uint8_t    sample_teros,
                       uint8_t    clear_teros,
                       uint8_t    wrap_counter,
                       bool       disable_unbiasing,
                       bool       disable_checking)
{
    trng->regs->CONFIG = sample_teros | (clear_teros << 8) | (wrap_counter << 8) |
                         (disable_unbiasing ? (1 << 30) : 0) | (disable_checking ? (1 << 31) : 0);
}
#endif

static void entropy_codasip_fpga_trng_read(bm_trng_t *trng, uint8_t *output, size_t len)
{
	int      bytes = 0;
	uint32_t rnd_number;
	
	while(len)
	{
		if (bytes == 0)
		{
			/* Read a random number */
			rnd_number = bm_trng_get_rnd(trng);
			bytes = 4;
		}
		
		*output++    = (uint8_t) rnd_number;
		rnd_number >>= 8;
		bytes --;
		len   --;
	}
}

static int entropy_codasip_fpga_trng_get_entropy(const struct device *dev,
                                                       uint8_t       *buffer,
                                                       uint16_t       length)
{
    bm_trng_t *trng = (bm_trng_t *) dev->config;

    size_t count = 0;

    while (length) {
#if 0
        size_t available;

        available = trng->regs->RNDN * 4;
		if (available == 0) {
			return -EINVAL;
		}
		count = Z_MIN(length, available);
#else

        /* Block and get all required values */
        count = length;
#endif

		entropy_codasip_fpga_trng_read(trng, buffer, count);
		buffer += count;
		length -= count;
	}

	return 0;
}

static int entropy_codasip_fpga_trng_get_entropy_isr(const struct device *dev,
                                                           uint8_t       *buf,
                                                           uint16_t       len,
                                                           uint32_t       flags)
{
    bm_trng_t *trng = (bm_trng_t *) dev->config;

	if ((flags & ENTROPY_BUSYWAIT) == 0U) {

		/* No busy wait; return whatever data is available. */
		size_t count;
		size_t available = trng->regs->RNDN * 4;

		if (available == 0) {
			return -ENODATA;
		}
		count = Z_MIN(len, available);
		entropy_codasip_fpga_trng_read(trng, buf, count);
		return count;

	} else {
		/* Allowed to busy-wait */
		int ret = entropy_codasip_fpga_trng_get_entropy(dev, buf, len);

		if (ret == 0) {
			/* Data retrieved successfully. */
			return len;
		}
		return ret;
	}

	return 0;
}

static int entropy_codasip_fpga_trng_init(const struct device *dev)
{
	const bm_trng_t *bm_trng = (bm_trng_t *) dev->config;
	
	/* Should call bm_trng_configure() instead of using a magic number: */
	bm_trng->regs->CONFIG = 0x00ff847f;	/* Magic Number from Lib BareMetal trng-demo.c */

	return 0;
}

static struct entropy_driver_api entropy_codasip_fpga_trng_api_funcs = {
	.get_entropy     = entropy_codasip_fpga_trng_get_entropy,
	.get_entropy_isr = entropy_codasip_fpga_trng_get_entropy_isr
};

#define ENTROPY_INIT(n)                                                        \
    static bm_trng_t entropy_codasip_fpga_trng_cfg_##n = {                     \
        .regs = (TRNG_Type *) DT_INST_REG_ADDR(n),                             \
    };                                                                         \
DEVICE_DT_INST_DEFINE(n,                                                       \
			entropy_codasip_fpga_trng_init,                                    \
			NULL,                                                              \
			NULL,                                                              \
			&entropy_codasip_fpga_trng_cfg_##n,                                \
			PRE_KERNEL_1,                                                      \
			CONFIG_ENTROPY_INIT_PRIORITY,                                      \
			&entropy_codasip_fpga_trng_api_funcs);

DT_INST_FOREACH_STATUS_OKAY(ENTROPY_INIT)
