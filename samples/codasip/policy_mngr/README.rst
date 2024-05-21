.. _policy_mngr-sample:

Policy Manager Sample
#####################

Overview
********

This sample demonstrates how to use the Policy Manager driver API.

Two GPIO lines have been connected to the Policy Manager for testing purposes to simulate
errors in the system. When a system error occurs the Policy Manager can do one of three things:
nothing, generate an interrupt or reset the device.

This sample app demonstrates these three options.

Building and Running
********************

The Policy Manager peripheral is configured in the board's ``.dts`` file. 
Make sure that the Policy Manager is enabled using the configuration file in ``boards`` folder.

Building
========

This application can be built and executed on Codasip's IIOT-DoomBar FPGA Platform:

.. code-block:: console

    ./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          policy_mngr

Sample output
=============

You should get a similar output as below:

.. code-block:: console

    *** Booting Zephyr OS build v3.6.0-108-gfb92002b9a83 ***
    Policy Manager sample application
    NOTE: THIS TEST CODE ONLY WORKS IF AUX GPIO01 (Aux) 3 & 4 CONNECTIONS TO THE POLICY MANAGER ALARMS 1 & 2 HAVE BEEN CONNECTED IN THE RTL
    [00:00:00.029,000] <inf> policy_mngr_codasip: _setup(): callback set = 0x20000f78
    [00:00:00.037,000] <inf> policy_mngr_codasip: _set_alarm(): POLICY_MNGR_ALARM_ACTION_INT_CALLBACK set for alarm_id = 0
    alarm1 & alarm2 not configured in policy manager yet - should do nothing
    Firing alarm1 
    Firing alarm2 
    Setting Alarm1 for Int Callback
    [00:00:06.061,000] <inf> policy_mngr_codasip: _set_alarm(): POLICY_MNGR_ALARM_ACTION_INT_CALLBACK set for alarm_id = 1
    [00:00:06.072,000] <inf> policy_mngr_codasip: _isr(): int_pend = 0x2
    [00:00:06.079,000] <inf> policy_mngr_codasip: _isr(): int_mask = 0x3
    [00:00:06.086,000] <inf> policy_mngr_codasip: _isr(): alarm_id = 1
    Policy Manager Interrupt Callback for alarm_id = 1
    Firing alarm1 
    [00:00:09.099,000] <inf> policy_mngr_codasip: _isr(): int_pend = 0x2
    [00:00:09.106,000] <inf> policy_mngr_codasip: _isr(): int_mask = 0x3
    [00:00:09.113,000] <inf> policy_mngr_codasip: _isr(): alarm_id = 1
    Policy Manager Interrupt Callback for alarm_id = 1
    Setting Alarm2 for Int Callback
    [00:00:12.128,000] <inf> policy_mngr_codasip: _set_alarm(): POLICY_MNGR_ALARM_ACTION_INT_CALLBACK set for alarm_id = 2
    [00:00:12.139,000] <inf> policy_mngr_codasip: _isr(): int_pend = 0x4
    [00:00:12.146,000] <inf> policy_mngr_codasip: _isr(): int_mask = 0x7
    [00:00:12.153,000] <inf> policy_mngr_codasip: _isr(): alarm_id = 2
    Policy Manager Interrupt Callback for alarm_id = 2
    Firing alarm2 
    [00:00:15.166,000] <inf> policy_mngr_codasip: _isr(): int_pend = 0x4
    [00:00:15.173,000] <inf> policy_mngr_codasip: _isr(): int_mask = 0x7
    [00:00:15.180,000] <inf> policy_mngr_codasip: _isr(): alarm_id = 2
    Policy Manager Interrupt Callback for alarm_id = 2
    Setting Alarm2 for RESET
    [00:00:18.194,000] <inf> policy_mngr_codasip: _set_alarm(): POLICY_MNGR_ALARM_ACTION_RESET set for alarm_id = 2
    Firing alarm2

    Secure Boot ROM code
    main: Build version: 0.10.0
    main: Build ID:      ef8d9f81
    main: Build time:    2024/04/22 08:38
    ...

.. note:: After the last message, the board will reset and the sequence will start again
