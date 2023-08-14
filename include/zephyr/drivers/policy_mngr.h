/*
 * Copyright (c) 2023 Codasip s.r.o.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_DRIVERS_POLICY_MNGR_H_
#define ZEPHYR_INCLUDE_DRIVERS_POLICY_MNGR_H_

/**
 * @brief Watchdog Interface
 * @defgroup watchdog_interface Watchdog Interface
 * @ingroup io_interfaces
 * @{
 */

#include <zephyr/types.h>
#include <zephyr/sys/util.h>
#include <zephyr/device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Policy Manager callback.
 *
 * @param dev Policy Manager device instance.
 * @param alarm_id identifier.
 */
typedef void (*policy_mngr_callback_t)( const struct device *dev, int alarm_id );

/**
 * @brief Policy Manager Alarm Action.
 *
 * @param dev Watchdog device instance.
 * @param channel_id Channel identifier.
 */
typedef enum
{
    POLICY_MNGR_ALARM_ACTION_NONE,          /* Disable all actions on this alarm - default */
    POLICY_MNGR_ALARM_ACTION_INT_CALLBACK,  /* Enable the Interrupt Callback on this alarm */
    POLICY_MNGR_ALARM_ACTION_RESET          /* Reset the system on this alarm */
} policy_mngr_action_t;



/** @cond INTERNAL_HIDDEN */

/**
 * @brief API for setting up the policy manager instance.
 * @see policy_mngr_setup().
 */
typedef int (*policy_mngr_api_setup)( const struct device *dev, policy_mngr_callback_t callback );

/**
 * @brief API for setting a policy manager alarm.
 * @see policy_mngr_set_alarm().
 */
typedef int (*policy_mngr_api_set_alarm)( const struct device *dev, int alarm_id, policy_mngr_action_t action );


__subsystem struct policy_mngr_driver_api {
    policy_mngr_api_setup     setup;
    policy_mngr_api_set_alarm set_alarm;
};
/**
 * @endcond
 */

/**
 * @brief Set up Policy Manager instance.
 *
 * This function is used for configuring the policy manager
 * It should be called before setting an alarm.
 *
 * @param dev       Policy Manager device instance.
 * @param callback  Function to call on an active interrupt alarm or NULL for no-callback
 *
 * @retval 0 If successful.
 */
__syscall int policy_mngr_setup( const struct device *dev, policy_mngr_callback_t callback );

static inline int z_impl_policy_mngr_setup( const struct device *dev, policy_mngr_callback_t callback )
{
    const struct policy_mngr_driver_api *api =
        (const struct policy_mngr_driver_api *)dev->api;

    return api->setup( dev, callback );
}


/**
 * @brief Set up Policy Manager Alarm Set.
 *
 * This function is used for configuring a policy manager alarm
 * It must be called after setup.
 *
 * @param dev       Policy Manager device instance.
 * @param alarm_id  Alarm ID to be configured
 * @param action    Action to take when the alarm triggers
 *
 * @retval 0       If successful.
 * @retval -EINVAL If there is no alarm for that id.
 */
__syscall int policy_mngr_set_alarm( const struct device *dev, int alarm_id, policy_mngr_action_t action );

static inline int z_impl_policy_mngr_set_alarm( const struct device *dev, int alarm_id, policy_mngr_action_t action )
{
    const struct policy_mngr_driver_api *api =
        (const struct policy_mngr_driver_api *)dev->api;

    return api->set_alarm( dev, alarm_id, action );
}


#ifdef __cplusplus
}
#endif

/** @} */

#include <syscalls/policy_mngr.h>

#endif /* ZEPHYR_INCLUDE_DRIVERS_POLICY_MNGR_H_ */
