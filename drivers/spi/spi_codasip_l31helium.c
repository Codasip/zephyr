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

#define DT_DRV_COMPAT codasip_l31helium_spi

#include <stdbool.h>
#include <stdint.h>
#include <zephyr/devicetree.h>

/* See:
 * include/zephyr/drivers/spi.h
 * drivers/sdhc/sdhc_spi.c
 */

/* Code from Codasip Lib BareMetal */

/* Register: SPI_CTRL */
/* Description: Control register */

/* Bit 4 : Interrupt Acknowledge */
#define BM_SPI_CTRL_IACK_Pos   (4UL)                           ///< Position of IACK field.
#define BM_SPI_CTRL_IACK_Msk   (0x1UL << BM_SPI_CTRL_IACK_Pos) ///< Bit mask of IACK field.
#define BM_SPI_CTRL_IACK_Clear (1UL)                           ///< Clear any pending interrupt

/* Bit 3 : Interrupt disable */
#define BM_SPI_CTRL_ID_Pos     (3UL)                         ///< Position of ID field.
#define BM_SPI_CTRL_ID_Msk     (0x1UL << BM_SPI_CTRL_ID_Pos) ///< Bit mask of ID field.
#define BM_SPI_CTRL_ID_Disable (1UL)                         ///< Disable interrupts

/* Bit 2 : Interrupt enable */
#define BM_SPI_CTRL_IE_Pos    (2UL)                         ///< Position of IE field.
#define BM_SPI_CTRL_IE_Msk    (0x1UL << BM_SPI_CTRL_IE_Pos) ///< Bit mask of IE field.
#define BM_SPI_CTRL_IE_Enable (1UL)                         ///< Enable interrupts

/* Bit 1 : Chip Select disable */
#define BM_SPI_CTRL_CSD_Pos     (1UL)                          ///< Position of CSD field.
#define BM_SPI_CTRL_CSD_Msk     (0x1UL << BM_SPI_CTRL_CSD_Pos) ///< Bit mask of CSD field.
#define BM_SPI_CTRL_CSD_Disable (1UL)                          ///< Disable Chip Select

/* Bit 0 : Chip Select enable */
#define BM_SPI_CTRL_CSE_Pos    (0UL)                          ///< Position of CSE field.
#define BM_SPI_CTRL_CSE_Msk    (0x1UL << BM_SPI_CTRL_CSE_Pos) ///< Bit mask of cSE field.
#define BM_SPI_CTRL_CSE_Enable (1UL)                          ///< Enable Chip Select

/* Register: SPI_STATUS */
/* Description: Status register */

/* Bit 2 : Chip Select state */
#define BM_SPI_STATUS_CS_Pos      (2UL)                           ///< Position of CS field.
#define BM_SPI_STATUS_CS_Msk      (0x1UL << BM_SPI_STATUS_CS_Pos) ///< Bit mask of CS field.
#define BM_SPI_STATUS_CS_Inactive (0UL) ///< Inactive (chip is not selected)
#define BM_SPI_STATUS_CS_Active   (1UL) ///< Active (chip is selected)

/* Bit 1 : Chip Select state */
#define BM_SPI_STATUS_TIP_Pos                (1UL) ///< Position of TIP field.
#define BM_SPI_STATUS_TIP_Msk                (0x1UL << BM_SPI_STATUS_TIP_Pos) ///< Bit mask of TIP field.
#define BM_SPI_STATUS_TIP_Idle               (0UL)                            ///< Idle
#define BM_SPI_STATUS_TIP_TransferInProgress (1UL) ///< A transfer in progress

/* Bit 0 : Interrupt flag */
#define BM_SPI_STATUS_IF_Pos        (0UL)                           ///< Position of IF field.
#define BM_SPI_STATUS_IF_Msk        (0x1UL << BM_SPI_STATUS_IF_Pos) ///< Bit mask of IF field.
#define BM_SPI_STATUS_IF_NotPresent (0UL)                           ///< Interrupt is not present
#define BM_SPI_STATUS_IF_Present    (1UL)                           ///< Interrupt is active

/* Register: SPI_COMMAND */
/* Description: Status register */

/* Bit 4 : SPI transaction type */
#define BM_SPI_COMMAND_RW_Pos   (4UL)                            ///< Position of RW field.
#define BM_SPI_COMMAND_RW_Msk   (0x1UL << BM_SPI_COMMAND_RW_Pos) ///< Bit mask of RW field.
#define BM_SPI_COMMAND_RW_Write (0UL)                            ///< Execute write transfer
#define BM_SPI_COMMAND_RW_Read  (1UL)                            ///< Execute read transfer

/* Bits 3:2 : SPI transaction size */
#define BM_SPI_COMMAND_SIZE_Pos   (2UL)                              ///< Position of SIZE field.
#define BM_SPI_COMMAND_SIZE_Msk   (0x3UL << BM_SPI_COMMAND_SIZE_Pos) ///< Bit mask of SIZE field.
#define BM_SPI_COMMAND_SIZE_8Bit  (0x0UL)                            ///< 8-bit
#define BM_SPI_COMMAND_SIZE_16Bit (0x1UL)                            ///< 16-bit
#define BM_SPI_COMMAND_SIZE_32Bit (0x2UL)                            ///< 32-bit
#define BM_SPI_COMMAND_SIZE_64Bit (0x3UL)                            ///< 64-bit

/* Bits 1:0 : SPI transaction mode */
#define BM_SPI_COMMAND_MODE_Pos    (0UL)                              ///< Position of MODE field.
#define BM_SPI_COMMAND_MODE_Msk    (0x3UL << BM_SPI_COMMAND_MODE_Pos) ///< Bit mask of MODE field.
#define BM_SPI_COMMAND_MODE_Serial (0x0UL)                            ///< Serial mode
#define BM_SPI_COMMAND_MODE_Dual   (0x1UL)                            ///< Dual mode
#define BM_SPI_COMMAND_MODE_Quad   (0x2UL)                            ///< Quad mode
#define BM_SPI_COMMAND_MODE_Dummy  (0x3UL)                            ///< "Dummy" mode

/** \brief QSPI Master controller */
typedef struct {
    union {
        volatile uint64_t       CTRL;   ///< (@ 0x0000) (WO) Control register
        const volatile uint64_t STATUS; ///< (@ 0x0000) (RO) Status register
    };
    volatile uint64_t SCK; ///< (@ 0x0008) (WLRL) Clock prescale register
    union {
        volatile uint64_t       DATA;     ///< (@ 0x0010) (WO) Output data register
        volatile const uint64_t DATAREAD; ///< (@ 0x0010) (RO) Input data register
    };
    volatile uint64_t COMMAND; ///< (@ 0x0018) (WO) Command register
} SPI_Type;

/** \brief Structure holding data neccessary to service the SPI peripheral. */
typedef struct {
    SPI_Type     *regs;       ///< Pointer to the peripheral registers
    unsigned      ext_irq_id; ///< External interrupt identifier
    volatile bool irq_flag;   ///< Flag signaling that interrupt happened
    bool          use_irq;    ///< Interrupt configuration
} bm_spi_t;

/** \brief SPI controller configuration */
typedef struct {
    uint32_t frequency; ///< SCL frequency in Hz
    bool     use_irq;   ///< Interrupt configuration
} bm_spi_config_t;

/**
 * \brief Assert chip select
 *
 * \param spi SPI device
 */
static void bm_spi_cs_assert(bm_spi_t *spi);

/**
 * \brief Deassert chip select
 *
 * \param spi SPI device
 */
static void bm_spi_cs_deassert(bm_spi_t *spi);

/**
 * \brief Wait until SPI controller completes operation
 *
 * \param spi SPI device
 */
static void bm_spi_wait(bm_spi_t *spi);

/**
 * \brief Returns true if a transfer is in progress
 *
 * \param spi SPI device
 */
static bool bm_spi_busy(bm_spi_t *spi);

#if 0 // Not used
/**
 * \brief Write command for SPI controller
 *
 * \param spi SPI device to write to
 * \param data Data to write
 * \param size Configure width of the write (use: BM_SPI_COMMAND_SIZE_*)
 * \param mode Configure mode (use: BM_SPI_COMMAND_MODE_*)
 */
static void bm_spi_write(bm_spi_t *spi, uint64_t data, uint8_t size, uint8_t mode);

/**
 * \brief Read command for SPI controller
 *
 * \param spi SPI device to read from
 * \param size Configure width of the read (use: BM_SPI_COMMAND_SIZE_*)
 * \param mode Configure mode (use: BM_SPI_COMMAND_MODE_*)
 *
 * \return Data read
 */
static uint64_t bm_spi_read(bm_spi_t *spi, uint8_t size, uint8_t mode);
#endif

/**
 * \brief Initialize SPI controller device
 *
 * \param spi Pointer to the SPI structure to initialize
 * \param config SPI configuration
 */
static void bm_spi_init(bm_spi_t *spi, const bm_spi_config_t *config);

#if 0 // Not used
/**
 * \brief Handle SPI interrupt
 *
 * \param spi Pointer to SPI controller
 *
 * \return 0 if successful, non-zero value in case of error
 */
static int bm_spi_handle_irq(bm_spi_t *spi);
#endif

static void bm_spi_cs_assert(bm_spi_t *spi)
{
    spi->regs->CTRL = BM_SPI_CTRL_CSE_Enable << BM_SPI_CTRL_CSE_Pos;
}

static void bm_spi_cs_deassert(bm_spi_t *spi)
{
    spi->regs->CTRL = BM_SPI_CTRL_CSD_Disable << BM_SPI_CTRL_CSD_Pos;
}

static void bm_spi_wait(bm_spi_t *spi)
{
    if (spi->use_irq)
    {
        while (!spi->irq_flag)
            ;
        spi->irq_flag = false;
    }
    else
    {
        while (spi->regs->STATUS & BM_SPI_STATUS_TIP_Msk)
            ;
    }
}

static bool bm_spi_busy(bm_spi_t *spi)
{
    return (spi->regs->STATUS & BM_SPI_STATUS_TIP_Msk) != 0; /* return true if Transfer In Progress */
}

#if 0 // Not used
static void bm_spi_write(bm_spi_t *spi, uint64_t data, uint8_t size, uint8_t mode)
{
    spi->regs->DATA    = data;
    spi->regs->COMMAND = BM_SPI_COMMAND_RW_Write << BM_SPI_COMMAND_RW_Pos |
                         size << BM_SPI_COMMAND_SIZE_Pos | mode << BM_SPI_COMMAND_MODE_Pos;
    bm_spi_wait(spi);
}

static uint64_t bm_spi_read(bm_spi_t *spi, uint8_t size, uint8_t mode)
{
    spi->regs->COMMAND = BM_SPI_COMMAND_RW_Read << BM_SPI_COMMAND_RW_Pos |
                         size << BM_SPI_COMMAND_SIZE_Pos | mode << BM_SPI_COMMAND_MODE_Pos;
    bm_spi_wait(spi);
    return spi->regs->DATAREAD;
}
#endif

/* As the l31helium SPI block is only half-duplex in Serial mode, while the above layers of Zephyr 
   SD drivers assume full-duplex mode and output dummy bytes (0xff) to clock in input data, then 
   something has to be done to fix this for the l31helium. 

   The solution is to switch to data read whenever an 0xff (data/dummy) byte is to be output. 
   The l31hardware outputs 0xff while reading by default. So this solution works without having to 
   modify other Zephyr layers (phew)!
*/
static uint64_t bm_spi_transceive_serial(bm_spi_t *spi, uint64_t data, uint8_t num_bytes)
{
    uint64_t data_rx = 0ULL;

    /* Process byte by byte */
    for( ; num_bytes > 0; num_bytes--, data >>= 8 )
    {
        spi->regs->DATA = data;

	if ( ( data & 0xff ) == 0xff )
	{
            /* Receive instead of transmit */
            spi->regs->COMMAND =   BM_SPI_COMMAND_RW_Read     << BM_SPI_COMMAND_RW_Pos
                                 | BM_SPI_COMMAND_SIZE_8Bit   << BM_SPI_COMMAND_SIZE_Pos
                                 | BM_SPI_COMMAND_MODE_Serial << BM_SPI_COMMAND_MODE_Pos;
	}
	else
	{
            spi->regs->COMMAND =   BM_SPI_COMMAND_RW_Write    << BM_SPI_COMMAND_RW_Pos
                                 | BM_SPI_COMMAND_SIZE_8Bit   << BM_SPI_COMMAND_SIZE_Pos
                                 | BM_SPI_COMMAND_MODE_Serial << BM_SPI_COMMAND_MODE_Pos;
        }
        bm_spi_wait(spi);

        data_rx <<= 8;
        data_rx  |= (uint8_t) spi->regs->DATAREAD;
    }

    return data_rx;
}

// #define TARGET_CLK_FREQ   25000000UL
// #define TARGET_CLK_FREQ   SYS_CLOCK_HW_CYCLES_PER_SEC
#define TARGET_CLK_FREQ   DT_PROP(DT_PATH(clocks, core_clk), clock_frequency)

static void bm_spi_init(bm_spi_t *spi, const bm_spi_config_t *config)
{
    uint32_t frequency = config->frequency;

    // Maximal SPI frequency is SYSTEM_CLK_FREQ/2
    if (frequency > (TARGET_CLK_FREQ / 2))
        frequency = TARGET_CLK_FREQ / 2;

    // Set clock divider
    spi->regs->SCK = TARGET_CLK_FREQ / (2 * frequency) - 1;

    spi->use_irq = config->use_irq;

#if 0 // Not used
    if (spi->use_irq)
    {
        // Enable interrupts
        spi->regs->CTRL = BM_SPI_CTRL_IE_Enable << BM_SPI_CTRL_IE_Pos;
        bm_ext_irq_enable(spi->ext_irq_id);
    }
#endif
}

#if 0 // Not used
static int bm_spi_handle_irq(bm_spi_t *spi)
{
    // Check SPI_IF
    if (!(spi->regs->STATUS & BM_SPI_STATUS_IF_Msk))
    {
        return -1;
    }

    // SPI controller operation completed
    spi->irq_flag   = true;
    spi->regs->CTRL = BM_SPI_CTRL_IACK_Clear << BM_SPI_CTRL_IACK_Pos;

    return 0;
}
#endif

/*
 * Copyright (c) 2019 Antmicro <www.antmicro.com>
 * Changes Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* The following code was based on spi_litespi.c */

/* Structure Declarations */

#define LOG_LEVEL CONFIG_SPI_LOG_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(spi_l31helium);

#include <stdbool.h>

#include <zephyr/sys/sys_io.h>
#include <zephyr/device.h>
#include <zephyr/drivers/spi.h>

#include "spi_context.h"


#define SPI_MAX_CS_SIZE 1
#define SPI_WORD_SIZE   1

typedef struct spi_l31helium_data_s {
	struct spi_context	ctx;
	bm_spi_t		bm_spi;
	uint8_t			mode;       ///< BM_SPI_COMMAND_MODE_
	bool			cs_active_high;
} spi_l31helium_data_t;

typedef struct spi_l31helium_cfg_s {
	uint32_t base;
	uint32_t f_sys;
} spi_l31helium_cfg_t;

#define SPI_DATA(dev) ((spi_l31helium_data_t *) ((dev)->data))


/* Helper Functions */
static int spi_l31helium_config( const struct device *dev, const struct spi_config *config )
{
//	const spi_l31helium_cfg_t      *cfg      =  dev->config;
	spi_l31helium_data_t           *context  =  dev->data;
	bm_spi_t                       *bm_spi   = &context->bm_spi;
//	SPI_Type                       *base     =  cfg->base;		/* SPI Base address */

	bm_spi_config_t bm_spi_config =
	{
		.frequency = config->frequency,
		.use_irq   = bm_spi->use_irq,
	};
	// LOG_INF("config->frequency = %d", config->frequency );


	uint8_t cs = 0x00;

	if (config->slave != 0) {
		if (config->slave > SPI_MAX_CS_SIZE) {
			LOG_ERR("More slaves than supported");
			return -ENOTSUP;
		}
		cs = (uint8_t)(config->slave);
	}

	// LOG_INF("config->operation = 0x%x", config->operation );

	if (config->operation & SPI_HALF_DUPLEX) {
		LOG_ERR("Half-duplex not supported");
		return -ENOTSUP;
	}

	if (SPI_WORD_SIZE_GET(config->operation) != 8) {
		LOG_ERR("Word size must be %d", SPI_WORD_SIZE);
		return -ENOTSUP;
	}

	context->cs_active_high = ( config->operation & SPI_CS_ACTIVE_HIGH ) ? true : false; 
	// LOG_INF("CS active high = %s", context->cs_active_high ? "true" : "false" );

#if 0 // ToDo
	if (config->operation & SPI_LOCK_ON) {
		LOG_ERR("Lock On not supported");
		return -ENOTSUP;
	}
#endif

	if (IS_ENABLED(CONFIG_SPI_EXTENDED_MODES) &&
	    (config->operation & SPI_LINES_MASK) >= SPI_LINES_OCTAL) {
		LOG_ERR("Only supports single, dual and quad SPI line modes");
		return -ENOTSUP;
	}

        switch ( config->operation & SPI_LINES_MASK )
	{
        case SPI_LINES_SINGLE:
                context->mode = BM_SPI_COMMAND_MODE_Serial;
		// LOG_INF("SPI_LINES_SINGLE selected");
                break;

#if 0  // Not supporting 2 or 4 lines (QSPI) at the moment
#if DT_INST_PROP(0, lines) > 1
        case SPI_LINES_DUAL:
                context->mode = BM_SPI_COMMAND_MODE_Dual;
		LOG_INF("SPI_LINES_DUAL selected");
                break;
#endif

#if DT_INST_PROP(0, lines) > 2
        case SPI_LINES_QUAD:
                context->mode = BM_SPI_COMMAND_MODE_Quad;
		LOG_INF("SPI_LINES_QUAD selected");
                break;
#endif
#endif

        case SPI_LINES_DUAL:
		LOG_ERR("SPI_LINES_DUAL not supported");
		return -ENOTSUP;

        case SPI_LINES_QUAD:
		LOG_ERR("SPI_LINES_QUAD not supported");
		return -ENOTSUP;

        case SPI_LINES_OCTAL:
		LOG_ERR("SPI_LINES_OCTAL not supported");
		return -ENOTSUP;
        }

	if (config->operation & SPI_TRANSFER_LSB) {
		LOG_ERR("LSB first not supported");
		return -ENOTSUP;
	}

	/* Actually the L31 Helium SPI module is fixed as CPOL=CPHA=1, which is compatible with CPOL=CPHA=0, apparently.
	   So, accept CPOL=CPHA=1 or CPOL=CPHA=0 */
	if (   ( ( config->operation & SPI_MODE_CPOL) != 0 )
	     ^ ( ( config->operation & SPI_MODE_CPHA) != 0 ) ) {
		LOG_ERR("Only supports CPOL=CPHA=0 or CPOL=CPHA=1");
		return -ENOTSUP;
	}

	if (config->operation & SPI_OP_MODE_SLAVE) {
		LOG_ERR("Slave mode not supported");
		return -ENOTSUP;
	}

	/* Set Loopback */
	if (config->operation & SPI_MODE_LOOP) {
		LOG_ERR("Loopback mode not supported");
		return -ENOTSUP;
	}

//	bm_spi->regs       = base;  ///< Pointer to the peripheral registers DONE BELOW IN SPI_INIT()
	bm_spi->ext_irq_id = 0;     ///< External interrupt identifier
	bm_spi->irq_flag   = 0;     ///< Flag signaling that interrupt happened
	bm_spi->use_irq    = false; ///< Interrupt configuration

	bm_spi_init( bm_spi, &bm_spi_config );

	return 0;
}

static void spi_l31helium_xfer(const struct device *dev,
			       const struct spi_config *config)
{
//	const spi_l31helium_cfg_t      *cfg      =  dev->config;
	spi_l31helium_data_t           *context  =  dev->data;
	bm_spi_t                       *bm_spi   = &context->bm_spi;

	struct spi_context *ctx = &SPI_DATA(dev)->ctx;
	uint32_t send_len = spi_context_longest_current_buf(ctx);
	uint8_t read_data, write_data;

	//LOG_INF("T%d R%d", ctx->tx_len, ctx->rx_len);

	if ( context->cs_active_high ) {
		bm_spi_cs_deassert( bm_spi );
	}
	else {
		bm_spi_cs_assert( bm_spi );
	}

	for (uint32_t i = 0; i < send_len; i++) {
		/* Send a frame */
		if ( i < ctx->tx_len ) {
			write_data = (uint8_t) (ctx->tx_buf)[i];
		} else {
			write_data = (uint8_t) 0xff;
		}

		read_data = (uint8_t) bm_spi_transceive_serial( bm_spi, (uint64_t) write_data, 1 );
		if ( i < ctx->rx_len ) {
			ctx->rx_buf[i] = read_data;
		}
	}

	if ( context->cs_active_high ) {
		bm_spi_cs_assert( bm_spi );
	}
	else {
		bm_spi_cs_deassert( bm_spi );
	}

	spi_context_complete(ctx, dev, 0);
}

#define SD_PWR_EN_N_NODE DT_ALIAS(sd_pwr_en_n_out)
static const struct gpio_dt_spec sd_pwr_en_n_out = GPIO_DT_SPEC_GET(SD_PWR_EN_N_NODE, gpios);

static int spi_l31helium_init(const struct device *dev)
{
//	const spi_l31helium_cfg_t      *cfg      =  dev->config;
//	spi_l31helium_data_t           *context  =  dev->data;
//	bm_spi_t                       *bm_spi   = &context->bm_spi;

	// LOG_INF("SPI address 0x%x", (uint32_t) bm_spi->regs);
	gpio_pin_set_dt(&sd_pwr_en_n_out, 0);

	return 0;
}

/* API Functions */

static int spi_l31helium_transceive(const struct device *dev,
                                    const struct spi_config *config,
                                    const struct spi_buf_set *tx_bufs,
                                    const struct spi_buf_set *rx_bufs)
{
	int ret_val;

	ret_val = spi_l31helium_config( dev, config );

	if ( ret_val == 0 )
	{
		spi_context_buffers_setup( &SPI_DATA(dev)->ctx, tx_bufs, rx_bufs, 1 );
		spi_l31helium_xfer( dev, config );
	}
	return ret_val;
}

#ifdef CONFIG_SPI_ASYNC
static int spi_l31helium_transceive_async(const struct device *dev,
					const struct spi_config *config,
					const struct spi_buf_set *tx_bufs,
					const struct spi_buf_set *rx_bufs,
					struct k_poll_signal *async)
{
	return -ENOTSUP;
}
#endif /* CONFIG_SPI_ASYNC */

static int spi_l31helium_release(const struct device *dev,
			       const struct spi_config *config)
{
	spi_l31helium_data_t           *context  =  dev->data;
	bm_spi_t                       *bm_spi   = &context->bm_spi;

	if ( bm_spi_busy( bm_spi ) ) {
		return -EBUSY;
	}

	return 0;
}

/* Device Instantiation */
static struct spi_driver_api spi_l31helium_api =
{
	.transceive = spi_l31helium_transceive,

#ifdef CONFIG_SPI_ASYNC
	.transceive_async = spi_l31helium_transceive_async,
#endif /* CONFIG_SPI_ASYNC */

	.release = spi_l31helium_release,
};

#define SPI_INIT(n)							\
	static spi_l31helium_data_t spi_l31helium_data_##n = {		\
		SPI_CONTEXT_INIT_LOCK(spi_l31helium_data_##n, ctx),	\
		SPI_CONTEXT_INIT_SYNC(spi_l31helium_data_##n, ctx),	\
		.bm_spi.regs = (SPI_Type *) DT_INST_REG_ADDR(n),	\
	};								\
	static spi_l31helium_cfg_t spi_l31helium_cfg_##n = {		\
		.base = DT_INST_REG_ADDR(n),				\
	};								\
	DEVICE_DT_INST_DEFINE(n,					\
			spi_l31helium_init,				\
			NULL,						\
			&spi_l31helium_data_##n,			\
			&spi_l31helium_cfg_##n,				\
			POST_KERNEL,					\
			CONFIG_SPI_INIT_PRIORITY,			\
			&spi_l31helium_api);

DT_INST_FOREACH_STATUS_OKAY(SPI_INIT)

