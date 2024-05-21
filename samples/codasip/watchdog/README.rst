.. _watchdog-sample:

Watchdog Sample
###############

Overview
********

This sample demonstrates how to use the watchdog driver API.

A typical use case for a watchdog is that the board is restarted in case some piece of code
is kept in an infinite loop.

Building and Running
********************

In this sample, a watchdog callback is used to handle a timeout event once. This functionality is used to request an action before the board
restarts due to a timeout event in the watchdog driver.

The watchdog peripheral is configured in the board's ``.dts`` file. Make sure that the watchdog is enabled
using the configuration file in ``boards`` folder.

This application can be built and executed on Codasip's IIOT-DoomBar FPGA Platform:

    ./make-sample.sh codasip_iiot_doombar_l31fluorine     samples/codasip          watchdog

To build for another board, change "codasip_iiot_doombar_l31fluorine" above to that board's name.

Sample output
=============

You should get a similar output as below:

.. code-block:: console

    [00:00:00.000,000] <inf> wdt_codasip: Device wdt@60520000 initialized
    *** Booting Zephyr OS build v3.6.0-108-gfb92002b9a83 ***
    Watchdog sample application
    Attempting to test pre-reset callback
    [00:00:00.018,000] <inf> wdt_codasip: Installed timeout value: 25000000
    [00:00:00.025,000] <err> wdt_codasip: Reset mode with callback not supported

    Callback support rejected, continuing anyway
    [00:00:00.037,000] <inf> wdt_codasip: Installed timeout value: 25000000
    [00:00:00.045,000] <dbg> wdt_codasip: wdt_codasip_install_timeout: Configuring reset CPU/SoC mode

    [00:00:00.054,000] <dbg> wdt_codasip: wdt_codasip_setup: Setup the watchdog
    Feeding watchdog 5 times
    Feeding watchdog...
    [00:00:00.066,000] <dbg> wdt_codasip: wdt_codasip_feed: Fed the watchdog
    Feeding watchdog...
    [00:00:01.025,000] <dbg> wdt_codasip: wdt_codasip_feed: Fed the watchdog
    Feeding watchdog...
    [00:00:01.985,000] <dbg> wdt_codasip: wdt_codasip_feed: Fed the watchdog
    Feeding watchdog...
    [00:00:02.944,000] <dbg> wdt_codasip: wdt_codasip_feed: Fed the watchdog
    Feeding watchdog...
    [00:00:03.903,000] <dbg> wdt_codasip: wdt_codasip_feed: Fed the watchdog
    Feeding watchdog - LAST TIME!
    [00:00:04.863,000] <dbg> wdt_codasip: wdt_codasip_feed: Fed the watchdog
    Waiting for reset...


.. note:: After the last message, the board will reset and the sequence will start again
