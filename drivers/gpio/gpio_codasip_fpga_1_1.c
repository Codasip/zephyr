/*
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT codasip_fpga_gpio_1_1

#include <zephyr/devicetree.h>
#include <errno.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/irq.h>
#include <soc.h>
#include <zephyr/sys/sys_io.h>
#include <zephyr/drivers/gpio/gpio_utils.h>

typedef void (*config_func_t)(const struct device *dev);

/** \brief General purpose input and output (GPIO) */
typedef struct
{
    volatile uint32_t       CLR; ///< (@ 0x00) Clear register
    volatile uint32_t       SET; ///< (@ 0x04) Set register
    const volatile uint32_t RD;  ///< (@ 0x08) Read register */
    volatile uint32_t       DIR; ///< (@ 0x0C) Direction register
} l31fpga_gpio_regs_t;

/** \brief Pin direction definitions */
typedef enum
{
    L31FPGA_GPIO_DIR_OUTPUT = 0,
    L31FPGA_GPIO_DIR_INPUT
} l31fpga_gpio_dir_t;

typedef struct gpio_l31fpga_config_s
{
	/* gpio_driver_config needs to be first */
	struct gpio_driver_config  common;
	l31fpga_gpio_regs_t       *base;        /* Pointer to the base of the GPIO registers */
	uint32_t                   port_map;    /* A bitmap of valid output pins */
	config_func_t              config_func;
} gpio_l31fpga_config_t;

typedef struct gpio_l31fpga_data_s {
	/* gpio_driver_data needs to be first */
	struct gpio_driver_data common;

	uint32_t                   shadow_out;	/* A copy of the current output pin state */

#ifdef CONFIG_CODASIP_L31FPGA_IRQ
	sys_slist_t cb;
#endif
} gpio_l31fpga_data_t;

#if 0
#define GPIO_REG_ADDR(base, offset) (base + offset)

#define GPIO_RW_ADDR(base, offset, p)			 \
	(GPIO_REG_ADDR(base, offset) | (1 << (p + 2)))

#define GPIO_RW_MASK_ADDR(base, offset, mask)		 \
	(GPIO_REG_ADDR(base, offset) | (mask << 2))

enum gpio_regs {
	GPIO_DATA_OFFSET = 0x000,
	GPIO_DIR_OFFSET  = 0x400,
	GPIO_DEN_OFFSET  = 0x51C,
	GPIO_IS_OFFSET   = 0x404,
	GPIO_IBE_OFFSET  = 0x408,
	GPIO_IEV_OFFSET  = 0x40C,
	GPIO_IM_OFFSET   = 0x410,
	GPIO_MIS_OFFSET  = 0x418,
	GPIO_ICR_OFFSET  = 0x41C,
};
#endif

#if 0
/* No L31 Helium GPIO IRQs */
static void gpio_l31fpga_isr(const struct device *dev)
{
	const gpio_l31fpga_config_t *const cfg = dev->config;
	gpio_l31fpga_data_t *context = dev->data;
	uint32_t base = cfg->base;
	uint32_t int_stat = sys_read32(GPIO_REG_ADDR(base, GPIO_MIS_OFFSET));

	gpio_fire_callbacks(&context->cb, dev, int_stat);

	sys_write32(int_stat, GPIO_REG_ADDR(base, GPIO_ICR_OFFSET));
}
#endif

static int gpio_l31fpga_configure(const struct device *dev,
				    gpio_pin_t pin, gpio_flags_t flags)
{
	const gpio_l31fpga_config_t  *cfg      = dev->config;
	gpio_l31fpga_data_t          *context  = dev->data;
	l31fpga_gpio_regs_t          *base     = cfg->base;		/* GPIO Base address */
	uint32_t                      port_map = cfg->port_map;	/* GPIO Valid IO bits, for each bit: 1 = GPIO, 0 = not GPIO */

	if ((flags & (GPIO_PULL_UP | GPIO_PULL_DOWN)) != 0)
	{
		return -ENOTSUP;
	}

	if ((flags & GPIO_SINGLE_ENDED) != 0)
	{
		return -ENOTSUP;
	}

	/* Check for pin availability */
	if (!sys_test_bit((uint32_t)&port_map, pin))
	{
		return -EINVAL;
	}

	if ((flags & GPIO_OUTPUT) != 0)
	{
		if ((flags & GPIO_OUTPUT_INIT_HIGH) != 0)
		{
			context->shadow_out |= BIT(pin);
			base->SET            = context->shadow_out;
		}
		else if ((flags & GPIO_OUTPUT_INIT_LOW) != 0)
		{
			context->shadow_out &= ~BIT(pin);
			base->CLR            = ~context->shadow_out;
		}

		base->DIR &= ~BIT(pin);	/* Clear the bit to change to output */
	}
	else if ((flags & GPIO_INPUT) != 0)
	{
		base->DIR |= BIT(pin);
	}
	else
	{
		/* Pin digital disable - change to input */
		base->DIR |= BIT(pin);
	}

	return 0;
}

static int gpio_l31fpga_port_get_raw(const struct device *dev,
				       uint32_t *value)
{
	const gpio_l31fpga_config_t  *cfg      = dev->config;
	l31fpga_gpio_regs_t          *base     = cfg->base;		/* GPIO Base address */

	*value = base->RD;

	return 0;
}

static int gpio_l31fpga_port_set_masked_raw(const struct device *dev,
					      uint32_t mask,
					      uint32_t value)
{
	const gpio_l31fpga_config_t  *cfg      = dev->config;
	gpio_l31fpga_data_t          *context  = dev->data;
	l31fpga_gpio_regs_t          *base     = cfg->base;		/* GPIO Base address */

	context->shadow_out &=  ~mask;
	context->shadow_out |= ( mask & value );

	base->CLR = ~context->shadow_out;
	base->SET =  context->shadow_out;

	return 0;
}

static int gpio_l31fpga_port_set_bits_raw(const struct device *dev,
					    uint32_t mask)
{
	const gpio_l31fpga_config_t  *cfg      = dev->config;
	gpio_l31fpga_data_t          *context  = dev->data;
	l31fpga_gpio_regs_t          *base     = cfg->base;		/* GPIO Base address */

	context->shadow_out |= mask;

	base->SET = context->shadow_out;

	return 0;
}

static int gpio_l31fpga_port_clear_bits_raw(const struct device *dev,
					      uint32_t mask)
{
	const gpio_l31fpga_config_t  *cfg      = dev->config;
	gpio_l31fpga_data_t          *context  = dev->data;
	l31fpga_gpio_regs_t          *base     = cfg->base;		/* GPIO Base address */

	context->shadow_out &= ~mask;

	base->CLR = ~context->shadow_out;

	return 0;
}

static int gpio_l31fpga_port_toggle_bits(const struct device *dev,
					   uint32_t mask)
{
	const gpio_l31fpga_config_t  *cfg      = dev->config;
	gpio_l31fpga_data_t          *context  = dev->data;
	l31fpga_gpio_regs_t          *base     = cfg->base;		/* GPIO Base address */

	/* value = ~base->RD; can't read output port pins */

	context->shadow_out ^= mask;

	base->CLR = ~context->shadow_out;
	base->SET =  context->shadow_out;

	return 0;
}

static int gpio_l31fpga_pin_interrupt_configure(const struct device *dev,
						  gpio_pin_t pin,
						  enum gpio_int_mode mode,
						  enum gpio_int_trig trig)
{
	/* No L31 Helium GPIO IRQs */
	return -ENOTSUP;
}

static int gpio_l31fpga_init(const struct device *dev)
{
	const gpio_l31fpga_config_t  *cfg      = dev->config;

	cfg->config_func(dev);

	return 0;
}

static int gpio_l31fpga_manage_callback(const struct device *dev,
					  struct gpio_callback *callback,
					  bool set)
{
	/* No L31 Helium GPIO IRQs */
	return -ENOTSUP;
}

static uint32_t gpio_l31fpga_get_pending_int(const struct device *dev)
{
        return 0;
}

static const struct gpio_driver_api gpio_l31fpga_driver_api = {
	.pin_configure           = gpio_l31fpga_configure,
	.port_get_raw            = gpio_l31fpga_port_get_raw,
	.port_set_masked_raw     = gpio_l31fpga_port_set_masked_raw,
	.port_set_bits_raw       = gpio_l31fpga_port_set_bits_raw,
	.port_clear_bits_raw     = gpio_l31fpga_port_clear_bits_raw,
	.port_toggle_bits        = gpio_l31fpga_port_toggle_bits,
	.pin_interrupt_configure = gpio_l31fpga_pin_interrupt_configure,
	.manage_callback         = gpio_l31fpga_manage_callback,
        .get_pending_int     = gpio_l31fpga_get_pending_int
};

#define L31FPGA_GPIO_DEVICE(n)                                                 \
	static void port_## n ##_l31fpga_config_func(const struct device *dev);    \
                                                                               \
	static gpio_l31fpga_data_t port_## n ##_l31fpga_runtime = {                \
		.shadow_out  = 0, /* GPIO Output is set to zero on reset */            \
	};                                                                         \
                                                                               \
	static const gpio_l31fpga_config_t gpio_l31fpga_port_## n ##_config = {    \
		.common = {                                                            \
			.port_pin_mask = GPIO_PORT_PIN_MASK_FROM_DT_INST(n),               \
		},                                                                     \
		.base        = (l31fpga_gpio_regs_t *) DT_INST_REG_ADDR(n),            \
		.port_map    = BIT_MASK(DT_INST_PROP(n, ngpios)),                      \
		.config_func = port_## n ##_l31fpga_config_func,                       \
	};                                                                         \
                                                                               \
	DEVICE_DT_INST_DEFINE(n,                                                   \
			    gpio_l31fpga_init,                                             \
			    NULL,                                                          \
			    &port_## n ##_l31fpga_runtime,                                 \
			    &gpio_l31fpga_port_## n ##_config,                             \
			    POST_KERNEL, CONFIG_GPIO_INIT_PRIORITY,                        \
			    &gpio_l31fpga_driver_api);                                     \
                                                                               \
	static void port_## n ##_l31fpga_config_func(const struct device *dev)     \
	{                                                                          \
		/* No L31 FPGA GPIO IRQs */                                          \
	}

#if 0
	static void port_## n ##_l31fpga_config_func(const struct device *dev)     \
	{                                                                          \
		IRQ_CONNECT(DT_INST_IRQN(n),                                           \
			    DT_INST_IRQ(n, priority),                                      \
			    gpio_l31fpga_isr,                                              \
			    DEVICE_DT_INST_GET(n), 0);                                     \
                                                                               \
		irq_enable(DT_INST_IRQN(n));                                           \
	}
#endif

DT_INST_FOREACH_STATUS_OKAY(L31FPGA_GPIO_DEVICE)
