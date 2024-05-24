/*
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT codasip_policy_mngr_0_02

#include <stdint.h>
#include <strings.h>

#include <soc.h>
#include <zephyr/drivers/policy_mngr.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/irq.h>
#include <zephyr/arch/common/ffs.h> /* For find_lsb_set() */

/* #define POLICY_MNGR_USE_SHADOW_REGS to use shadow registers - required for the first Policy
                                                                 Manager */
/* #define POLICY_MNGR_USE_SHADOW_REGS Not required with the new hardware */

LOG_MODULE_REGISTER(policy_mngr_codasip, CONFIG_POLICY_MNGR_LOG_LEVEL);

/* Codasip Policy Manager Revision 0.02 */
typedef struct {
#if 0
    /* Watchdog timer treated as a separate device as there is already a Zephyr Watchdog device */
    volatile uint32_t WDTIMER;    /* RW - Watchdog Timer - 32 bit value clocked by System Clock,
                                          decrements to zero. When it hits 1 then system reset
                                          occurs */
    volatile uint32_t FILL[3];    /* -- - 12 byte gap between WDTIMER and RST_MASK */
#endif

        volatile uint32_t RST_MASK; /*  W - Reset mask */
        volatile uint32_t INT_MASK; /*  W - Interrupt mask */
        volatile uint32_t INT_PEND; /* RW - READ:  Set bits indicate pending interrupts
                                            WRITE: Each high bit clears the associated pending
                                                   interrupt, if all are cleared then int_out will 
                                                   go low.*/
} POLICY_MANAGER_Type;

/* Device constant configuration parameters */
struct policy_mngr_codasip_cfg {
        POLICY_MANAGER_Type *policy_mgr; /* .WDTIMER 0 = off. Non-zero counts down at system clock
                                                        rate, when it reaches 1 the system resets */
        uint32_t clock;                  /* Clock frequency in Hz */
        uint32_t port_map;               /* A bitmap of valid output pins */
        void (*irq_cfg_func)(void);      /* Function pointer of IRQ config function */
};

struct policy_mngr_codasip_data {
        policy_mngr_callback_t callback;
#ifdef POLICY_MNGR_USE_SHADOW_REGS
        uint32_t rst_mask_shadow;
        uint32_t int_mask_shadow;
#endif /* POLICY_MNGR_USE_SHADOW_REGS */
};

static int policy_mngr_codasip_setup(const struct device *dev, policy_mngr_callback_t callback)
{ /* callback can be NULL for no-callback */
        struct policy_mngr_codasip_data *data = dev->data;

        data->callback = callback;

        LOG_INF("_setup(): callback set = 0x%x", (uint32_t)callback);

        return 0;
}

static int policy_mngr_codasip_set_alarm(const struct device *dev, int alarm_id,
                                         policy_mngr_action_t action)
{
        const struct policy_mngr_codasip_cfg *config = dev->config;
        POLICY_MANAGER_Type *pm = config->policy_mgr;
        uint32_t port_map =
                config->port_map; /* GPIO Valid IO bits, for each bit: 1 = GPIO, 0 = not GPIO */

        /* Check for pin availability */
        if (!sys_test_bit((uint32_t)&port_map, alarm_id)) {
                LOG_ERR("_set_alarm(): Invalid alarm id");
                return -EINVAL;
        }

        uint32_t bit_mask = BIT(alarm_id);

#ifdef POLICY_MNGR_USE_SHADOW_REGS
        struct policy_mngr_codasip_data *data = dev->data;

        switch (action) {
        case POLICY_MNGR_ALARM_ACTION_NONE:
                LOG_INF("_set_alarm(): POLICY_MNGR_ALARM_ACTION_NONE set for alarm_id = %d",
                        alarm_id);
                data->rst_mask_shadow &= ~bit_mask;
                data->int_mask_shadow &= ~bit_mask;
                break;

        case POLICY_MNGR_ALARM_ACTION_INT_CALLBACK:
                LOG_INF("_set_alarm(): POLICY_MNGR_ALARM_ACTION_INT_CALLBACK set for alarm_id = %d",
                        alarm_id);
                data->rst_mask_shadow &= ~bit_mask;
                data->int_mask_shadow |= bit_mask;
                break;

        case POLICY_MNGR_ALARM_ACTION_RESET:
                LOG_INF("_set_alarm(): POLICY_MNGR_ALARM_ACTION_RESET set for alarm_id = %d",
                        alarm_id);
                data->rst_mask_shadow |= bit_mask;
                data->int_mask_shadow &= ~bit_mask;
                break;
        }

        LOG_INF("_set_alarm(): int_pend = 0x%x", pm->INT_PEND);

        /* Registers are write only */
        pm->RST_MASK = data->rst_mask_shadow;
        LOG_INF("_set_alarm(): rst_mask = 0x%x", data->rst_mask_shadow);
        pm->INT_MASK = data->int_mask_shadow;
        LOG_INF("_set_alarm(): int_mask = 0x%x", data->int_mask_shadow);

#else  /* !POLICY_MNGR_USE_SHADOW_REGS */
        switch (action) {
        case POLICY_MNGR_ALARM_ACTION_NONE:
                LOG_INF("_set_alarm(): POLICY_MNGR_ALARM_ACTION_NONE set for alarm_id = %d",
                        alarm_id);
                pm->RST_MASK &= ~bit_mask;
                pm->INT_MASK &= ~bit_mask;
                break;

        case POLICY_MNGR_ALARM_ACTION_INT_CALLBACK:
                LOG_INF("_set_alarm(): POLICY_MNGR_ALARM_ACTION_INT_CALLBACK set for alarm_id = %d",
                        alarm_id);
                pm->RST_MASK &= ~bit_mask;
                pm->INT_MASK |= bit_mask;
                break;

        case POLICY_MNGR_ALARM_ACTION_RESET:
                LOG_INF("_set_alarm(): POLICY_MNGR_ALARM_ACTION_RESET set for alarm_id = %d",
                        alarm_id);
                pm->RST_MASK |= bit_mask;
                pm->INT_MASK &= ~bit_mask;
                break;
        }
#endif /* !POLICY_MNGR_USE_SHADOW_REGS */

        return 0;
}

void policy_mngr_codasip_isr(const struct device *dev)
{
        const struct policy_mngr_codasip_cfg *config = dev->config;
        struct policy_mngr_codasip_data *data = dev->data;
        POLICY_MANAGER_Type *pm = config->policy_mgr;

        uint32_t int_pend = pm->INT_PEND;

#ifdef POLICY_MNGR_USE_SHADOW_REGS
        uint32_t bit_mask = data->int_mask_shadow & int_pend;

        LOG_INF("_isr(): int_pend = 0x%x", int_pend);
        LOG_INF("_isr(): int_mask = 0x%x", data->int_mask_shadow);

#else
        uint32_t bit_mask = pm->INT_MASK & int_pend;

        LOG_INF("_isr(): int_pend = 0x%x", int_pend);
        LOG_INF("_isr(): int_mask = 0x%x", pm->INT_MASK);
#endif

        int alarm_id =
                find_lsb_set(bit_mask); /* Note: This function returns the the first set lsb bit pos
                                           + 1 (!). alternative: __builtin_ffs( pending ); */

        if (alarm_id == 0) {
                LOG_INF("_isr(): spurious interrupt - no alarms");
        } else {
                alarm_id--; /* Adjust to the correct bit position */

                LOG_INF("_isr(): alarm_id = %d", alarm_id);

                /* Call the callback, if set */
                if (data->callback != NULL) {
                        data->callback(dev, alarm_id);
                }

                /* Clear the interrupt */
                pm->INT_PEND = 1UL << alarm_id;
        }

#ifdef POLICY_MNGR_USE_SHADOW_REGS
        /* Clear all masked interrupts - there is a bug in the hardware that allows masked
         * interrupts to interrupt! */
        pm->INT_PEND = ~data->int_mask_shadow;
#endif
}

static int policy_mngr_codasip_init(const struct device *dev)
{
        const struct policy_mngr_codasip_cfg *config = dev->config;
        POLICY_MANAGER_Type *pm = config->policy_mgr;

#ifdef POLICY_MNGR_USE_SHADOW_REGS
        struct policy_mngr_codasip_data *data = dev->data;

        /* All alarms are disabled at reset */
        data->rst_mask_shadow = 0;
        data->int_mask_shadow = 0;

        /* Registers are write only */
        pm->RST_MASK = data->rst_mask_shadow;
        pm->INT_MASK = data->int_mask_shadow;

#else
        /* All alarms are disabled at reset */
        pm->RST_MASK = 0;
        pm->INT_MASK = 0;
#endif

        /* Enable IRQ */
        config->irq_cfg_func();

        LOG_INF("Device %s initialized", dev->name);

        return 0;
}

static const struct policy_mngr_driver_api policy_mngr_codasip_driver_api = {
        .setup = policy_mngr_codasip_setup, .set_alarm = policy_mngr_codasip_set_alarm};

#define CODASIP_POLICY_MNGR_INIT(index)                                                            \
                                                                                                   \
        static void policy_mngr_codasip_cfg_func_##index(void);                                    \
                                                                                                   \
        static const struct policy_mngr_codasip_cfg policy_mngr_codasip_cfg_##index = {            \
                .policy_mgr = (POLICY_MANAGER_Type *)DT_INST_REG_ADDR(index),                      \
                .clock = DT_INST_PROP(index, clock_frequency),                                     \
                .port_map = BIT_MASK(DT_INST_PROP(index, nalarms)),                                \
                .irq_cfg_func = policy_mngr_codasip_cfg_func_##index,                              \
        };                                                                                         \
        static struct policy_mngr_codasip_data policy_mngr_codasip_data_##index;                   \
                                                                                                   \
        DEVICE_DT_INST_DEFINE(index, &policy_mngr_codasip_init, NULL,                              \
                              &policy_mngr_codasip_data_##index, &policy_mngr_codasip_cfg_##index, \
                              POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                    \
                              &policy_mngr_codasip_driver_api);                                    \
                                                                                                   \
        static void policy_mngr_codasip_cfg_func_##index(void)                                     \
        {                                                                                          \
                IRQ_CONNECT(DT_INST_IRQN(index), DT_INST_IRQ(index, priority),                     \
                            policy_mngr_codasip_isr, DEVICE_DT_INST_GET(index), 0);                \
                irq_enable(DT_INST_IRQN(index));                                                   \
        }

DT_INST_FOREACH_STATUS_OKAY(CODASIP_POLICY_MNGR_INIT)
