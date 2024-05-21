.. _blinkers:

Blinkers
########

Overview
********

The Blinky sample blinks the LEDs forever using the :ref:`GPIO API <gpio_api>`. The switches
control if the associated LED toggles or not on each 1 second tick.

The source code shows how to:

#. Get a pin specification from the :ref:`devicetree <dt-guide>` as a
   :c:struct:`gpio_dt_spec`
#. Configure the GPIO pin as an output
#. Toggle the pin forever

See :ref:`pwm-blinky-sample` for a similar sample that uses the PWM API instead.

.. _blinky-sample-requirements:

Requirements
************

Your board must:

#. Have LEDs connected via GPIO pins (these are called "User LEDs" on many of
   Zephyr's :ref:`boards`).
#. Have the LEDs configured using the ``led0`` to ``led7`` devicetree alias.

Building and Running
********************

This application can be built and executed on Codasip's FPGA Platform:

./make-sample.sh codasip_inferno_l31carbon     samples/codasip          blinkers

To build for another board, change "codasip_inferno_l31carbon" above to that board's name.

On execution the LED starts to blink. If a runtime error occurs, the sample
exits without printing to the console.
