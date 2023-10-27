/*
 * Copyright (c) 2019 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/policy_mngr.h>
#include <zephyr/sys/printk.h>
#include <stdbool.h>

#define POLICY_MNGR_ALARM0_L31AS ( 0 )
#define POLICY_MNGR_ALARM1_GPIO8 ( 1 )
#define POLICY_MNGR_ALARM2_GPIO9 ( 2 )

#define IRQ_ALARM_NUM        DT_IRQ_BY_IDX( DT_NODELABEL( policy_mngr0 ), 0, irq )

/* The devicetree node identifiers for "alarm1" & "alarm2" alias. */
#define ALARM1_NODE DT_ALIAS(alarm1)
#define ALARM2_NODE DT_ALIAS(alarm2)

static const struct gpio_dt_spec alarm1 = GPIO_DT_SPEC_GET( ALARM1_NODE, gpios );
static const struct gpio_dt_spec alarm2 = GPIO_DT_SPEC_GET( ALARM2_NODE, gpios );


static void policy_mngr_callback( const struct device *dev, int alarm_id )
{
    printk( "Policy Manager Interrupt Callback for alarm_id = %d\n", alarm_id );

    /* Clear the alarm source */
    switch( alarm_id )
    {
    case 1:
        gpio_pin_set_dt( &alarm1, 0 );
        break;
        
    case 2:
        gpio_pin_set_dt( &alarm2, 0 );
        break;

    default:
        break;
    }
}

int main( void )
{
    const struct device *const policy_mngr_dev = DEVICE_DT_GET( DT_NODELABEL( policy_mngr0 ) );

    printk( "Policy Manager sample application\n" );
    printk( "NOTE: THIS TEST CODE ONLY WORKS IF AUX GPIO8 & 9 CONNECTIONS TO THE POLICY MANAGER HAVE BEEN CONNECTED IN THE RTL\n" );
    printk( "This was done in: https://gitlab.codasip.com/platforms/l31-fpga-platform/-/jobs/1166045/artifacts/file/work/l31-dcls-5.0.3-9.4.1-vivado-project-platform-0.10.0.zip\n" );

    if ( !device_is_ready(policy_mngr_dev) ) {
        printk( "%s: device not ready.\n", policy_mngr_dev->name );
        return 0;
    }

    if ( policy_mngr_setup( policy_mngr_dev, policy_mngr_callback ) != 0 )
    {
        printk( "%s: Error setting device up.\n", policy_mngr_dev->name );
        return 0;
    }

    if ( policy_mngr_set_alarm( policy_mngr_dev, POLICY_MNGR_ALARM0_L31AS, POLICY_MNGR_ALARM_ACTION_INT_CALLBACK ) != 0 )
    {
        printk( "%s: Error setting alarm id %d\n", policy_mngr_dev->name, POLICY_MNGR_ALARM0_L31AS );
        return 0;
    }

    printk( "alarm1 & alarm2 not configured in policy manager yet - should do nothing\n" );

    printk( "Firing alarm1 \n" );
    gpio_pin_set_dt( &alarm1, 1 );
    // gpio_pin_set_dt( &alarm1, 0 );

    k_sleep(K_MSEC(3000));

    printk( "Firing alarm2 \n" );
    gpio_pin_set_dt( &alarm2, 1 );
    // gpio_pin_set_dt( &alarm2, 0 );

    k_sleep(K_MSEC(3000));

    printk( "Setting Alarm1 for Int Callback\n" );
    
    if ( policy_mngr_set_alarm( policy_mngr_dev, POLICY_MNGR_ALARM1_GPIO8, POLICY_MNGR_ALARM_ACTION_INT_CALLBACK ) != 0 )
    {
        printk( "%s: Error setting alarm id %d\n", policy_mngr_dev->name, POLICY_MNGR_ALARM1_GPIO8 );
        return 0;
    }

    k_sleep(K_MSEC(3000));

    printk( "Firing alarm1 \n" );
    gpio_pin_set_dt( &alarm1, 1 );
    // gpio_pin_set_dt( &alarm1, 0 );

    k_sleep(K_MSEC(3000));

    printk( "Setting Alarm2 for Int Callback\n" );
    
    if ( policy_mngr_set_alarm( policy_mngr_dev, POLICY_MNGR_ALARM2_GPIO9, POLICY_MNGR_ALARM_ACTION_INT_CALLBACK ) != 0 )
    {
        printk( "%s: Error setting alarm id %d\n", policy_mngr_dev->name, POLICY_MNGR_ALARM2_GPIO9 );
        return 0;
    }

    k_sleep(K_MSEC(3000));

    printk( "Firing alarm2 \n" );
    gpio_pin_set_dt( &alarm2, 1 );
    // gpio_pin_set_dt( &alarm2, 0 );

    k_sleep(K_MSEC(3000));

    printk( "Setting Alarm2 for RESET\n" );

    if ( policy_mngr_set_alarm( policy_mngr_dev, POLICY_MNGR_ALARM2_GPIO9, POLICY_MNGR_ALARM_ACTION_RESET ) != 0 )
    {
        printk( "%s: Error setting alarm id %d\n", policy_mngr_dev->name, POLICY_MNGR_ALARM2_GPIO9 );
        return 0;
    }

    k_sleep(K_MSEC(3000));

    printk( "Firing alarm2\n" );

    k_sleep(K_MSEC(1000));

    gpio_pin_set_dt( &alarm2, 1 );
    // gpio_pin_set_dt( &alarm2, 0 );

    printk( "Alarm2 fired - you should not see this message\n" );

    /* Waiting for the SoC reset. */
    printk("Waiting for reset...\n");
    while (1) {
        k_yield();
    }

    return 0;
}
