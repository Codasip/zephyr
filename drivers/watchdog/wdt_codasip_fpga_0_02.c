/*
 * Copyright (c) 2019 Interay Solutions B.V.
 * Copyright (c) 2019 Oane Kingma
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */


/* Watchdog process:
 *   wdt_codasip_init() is called on Zephyr start-up (see below)
 * 
 *   App (samples/drivers/watchdog) calls:
 *     wdt_channel_id = wdt_install_timeout(wdt, &wdt_config);
 *     err = wdt_setup(wdt, WDT_OPT_PAUSE_HALTED_BY_DBG);
 * 
 *   To reset the watchdog, periodically:
 *     wdt_feed(wdt, wdt_channel_id);
 */

#define DT_DRV_COMPAT codasip_fpga_wdt_0_02

#include <stdint.h>

#include <soc.h>
#include <zephyr/drivers/watchdog.h>

#include <zephyr/logging/log.h>
#include <zephyr/irq.h>
LOG_MODULE_REGISTER(wdt_codasip, CONFIG_WDT_LOG_LEVEL);

/* Codasip Policy Manager Revision 0.02 */
typedef struct
{
    volatile uint32_t WDTIMER;    /* RW - Watchdog Timer - 32 bit value clocked by System Clock, decrements to zero. When it hits 1 then system reset occurs */
    volatile uint32_t FILL[ 3 ];  /* -- - 12 byte gap between WDTIMER and RST_MASK */
    volatile uint32_t RST_MASK;   /* RW - Reset mask */
    volatile uint32_t INT_MASK;   /* RW - Interrupt mask */
    volatile uint32_t INT_PEND;   /* RW - READ: Set bits indicate pending interrupts
                                          WRITE: Each high bit clears the associated pending interrupt, if all are
                                                 cleared then int_out will go low.*/
} POLICY_MANAGER_Type;

/* Device constant configuration parameters */
struct wdt_codasip_cfg
{
    POLICY_MANAGER_Type *policy_mgr;  /* .WDTIMER 0 = off. Non-zero counts down at system clock rate, when it reaches 1 the system resets */
    uint32_t             clock;       /* Clock frequency of the WDT in Hz */
};

struct wdt_codasip_data
{
/*  wdt_callback_t callback; no interrupt, so no callback */
    uint32_t wdtimer_feed_value;
    bool     timeout_installed;
};

static int wdt_codasip_setup(const struct device *dev, uint8_t options) /* options are ignored */
{
    const struct wdt_codasip_cfg *config = dev->config;
    struct wdt_codasip_data      *data   = dev->data;

    if (!data->timeout_installed) {
        LOG_ERR("No valid timeouts installed");
        return -EINVAL;
    }

    /* Watchdog is started after initialization */
    config->policy_mgr->WDTIMER = data->wdtimer_feed_value; /* Writing a non-zero value starts the watchdog */
    LOG_DBG("Setup the watchdog");

    return 0;
}

static int wdt_codasip_disable(const struct device *dev)
{
    const struct wdt_codasip_cfg *config = dev->config;
    struct wdt_codasip_data      *data   = dev->data;

    config->policy_mgr->WDTIMER = 0;    /* 0 turns the watchdog off */
    data->timeout_installed = false;
    LOG_DBG("Disabled the watchdog");

    return 0;
}

static int wdt_codasip_install_timeout(const struct device *dev,
                                       const struct wdt_timeout_cfg *cfg)
{
    const struct wdt_codasip_cfg *config = dev->config;
    struct wdt_codasip_data      *data   = dev->data;
    uint64_t installed_timeout;

    if (data->timeout_installed) {
        LOG_ERR("No more timeouts can be installed");
        return -ENOMEM;
    }

    /* cfg->window.max is upper limit of watchdog feed timeout in milliseconds. */
    installed_timeout = cfg->window.max * (uint64_t) config->clock / 1000; /* WDT clock frequency (in Hz) from DT */

    if ( installed_timeout > UINT32_MAX ) {
        LOG_ERR("Upper limit timeout out of range");
        return -EINVAL;
    }

    LOG_INF("Installed timeout value: %u", (uint32_t) installed_timeout);

    if (cfg->window.min > 0) {
        LOG_ERR("Lower limit timeout not supported");
        return -EINVAL;
    }

    /* Set mode of watchdog and callback */
    switch (cfg->flags) {
    case WDT_FLAG_RESET_SOC:
    case WDT_FLAG_RESET_CPU_CORE:
        if (cfg->callback != NULL) {
            LOG_ERR("Reset mode with callback not supported\n");
            return -ENOTSUP;
        }
        LOG_DBG("Configuring reset CPU/SoC mode\n");
        break;

    case WDT_FLAG_RESET_NONE:
        LOG_DBG("Non-reset mode not supported\n");
        return -ENOTSUP;

    default:
        LOG_ERR("Unsupported watchdog config flag\n");
        return -EINVAL;
    }
    
    data->wdtimer_feed_value = (uint32_t) installed_timeout;
    data->timeout_installed = true;

    return 0;
}

static int wdt_codasip_feed(const struct device *dev, int channel_id)
{
    const struct wdt_codasip_cfg *config = dev->config;
    struct wdt_codasip_data      *data   = dev->data;

    if (channel_id != 0) {
        LOG_ERR("Invalid channel id");
        return -EINVAL;
    }

    config->policy_mgr->WDTIMER = data->wdtimer_feed_value; /* Writing a non-zero value starts the watchdog */
    LOG_DBG("Fed the watchdog");

    return 0;
}

#if 0
/* No ISR */
static void wdt_codasip_isr(const struct device *dev)
{
    const struct wdt_codasip_cfg *config = dev->config;
    struct wdt_codasip_data      *data   = dev->data;
    WDOG_TypeDef *wdog = config->base;
    uint32_t flags;

    /* Clear IRQ flags */
    flags = WDOGn_IntGet(wdog);
    WDOGn_IntClear(wdog, flags);

    if (data->callback != NULL) {
        data->callback(dev, 0);
    }
}
#endif

static int wdt_codasip_init(const struct device *dev)
{
#ifdef CONFIG_WDT_DISABLE_AT_BOOT
    const struct wdt_codasip_cfg *config = dev->config;

    config->policy_mgr->WDTIMER = 0;
#endif

    LOG_INF("Device %s initialized", dev->name);

    return 0;
}

static const struct wdt_driver_api wdt_codasip_driver_api = {
    .setup           = wdt_codasip_setup,
    .disable         = wdt_codasip_disable,
    .install_timeout = wdt_codasip_install_timeout,
    .feed            = wdt_codasip_feed,
};

#define CODASIP_WDT_INIT(index)                                        \
                                                                       \
    static const struct wdt_codasip_cfg wdt_codasip_cfg_##index = {    \
        .policy_mgr = (POLICY_MANAGER_Type *)                          \
            DT_INST_REG_ADDR(index),                                   \
        .clock = DT_INST_PROP(index, clock_frequency),                 \
        /* .irq_cfg_func = wdt_codasip_cfg_func_##index, */            \
    };                                                                 \
    static struct wdt_codasip_data wdt_codasip_data_##index;           \
                                                                       \
    DEVICE_DT_INST_DEFINE(index,                                       \
                &wdt_codasip_init, NULL,                               \
                &wdt_codasip_data_##index,                             \
                &wdt_codasip_cfg_##index, POST_KERNEL,                 \
                CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                   \
                &wdt_codasip_driver_api);

DT_INST_FOREACH_STATUS_OKAY(CODASIP_WDT_INIT)
