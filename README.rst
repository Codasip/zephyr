.. raw:: html

   <a href="https://www.zephyrproject.org">
     <p align="center">
       <picture>
         <source media="(prefers-color-scheme: dark)" srcset="doc/_static/images/logo-readme-dark.svg">
         <source media="(prefers-color-scheme: light)" srcset="doc/_static/images/logo-readme-light.svg">
         <img src="doc/_static/images/logo-readme-light.svg">
       </picture>
     </p>
   </a>

   <a href="https://bestpractices.coreinfrastructure.org/projects/74"><img
   src="https://bestpractices.coreinfrastructure.org/projects/74/badge"></a>
   <a
   href="https://github.com/zephyrproject-rtos/zephyr/actions/workflows/twister.yaml?query=branch%3Amain">
   <img
   src="https://github.com/zephyrproject-rtos/zephyr/actions/workflows/twister.yaml/badge.svg?event=push"></a>


The Zephyr Project is a scalable real-time operating system (RTOS) supporting
multiple hardware architectures, optimized for resource constrained devices,
and built with security in mind.

The Zephyr OS is based on a small-footprint kernel designed for use on
resource-constrained systems: from simple embedded environmental sensors and
LED wearables to sophisticated smart watches and IoT wireless gateways.

The Zephyr kernel supports multiple architectures, including ARM (Cortex-A,
Cortex-R, Cortex-M), Intel x86, ARC, Nios II, Tensilica Xtensa, and RISC-V,
SPARC, MIPS, and a large number of `supported boards`_.

.. below included in doc/introduction/introduction.rst


Getting Started
***************

Welcome to Zephyr! See the `Introduction to Zephyr`_ for a high-level overview,
and the documentation's `Getting Started Guide`_ to start developing.

.. start_include_here

Community Support
*****************

Community support is provided via mailing lists and Discord; see the Resources
below for details.

.. _project-resources:

Resources
*********

Here's a quick summary of resources to help you find your way around:

Codasip Port
------------

This repository is a port of Zephyr v3.6.0 to Codasip's FPGA platforms:

- Inferno (Genesys 2 FPGA Board)

    * L31 Carbon
    * L31 Helium
 
- IIOT-DoomBar (Genesys 2 FPGA Board)

    * L31 Fluorine

- Hobgoblin (Genesys 2 FPGA Board)

    * H730
    * A730

The Codasip L31 core is a low-power embedded core, part of the Codasip RISC-V Processors 3 Series.

The Codasip A730 core is a dual-issue, in-order core compliant with RVA22 profile, and a part of the 
Codasip RISC-V Processors 700 processor family.

To get going, setup your Zephyr environment as detailed below, then run the script `make-all.sh` to 
build all the Codasip sample applications for various platforms (the binaries are put in
the respective `builds-codasip_*` directories).

The instructions on how to run the applications come with the FPGA Platform bitstreams. Basically,
for all non-secure platforms, put the sample application `.bin` files in the root directory of an
SD Card along with the bitstream `.bit` file and create a file called `config.txt` containing 
the payload information, for example:

.. code-block::

    # CODASIP FIRST STAGE BOOTLOADER CONFIGURATION FILE
    #
    # Specify each payload loaded from microSD card using format:
    # [<GPIO SWITCH>] <PAYLOAD PATH> <LOAD ADDRESS> ["BOOT"]
    #
    # Where:
    # <GPIO SWITCH> - Allows to specify a switch used to enable (switch ON)
    #     and disable (switch OFF) payloads.
    # <PAYLOAD PATH> - Payload location on the microSD card file system.
    # <LOAD ADDRESS> - Memory address the payload will be stored at.
    # "BOOT" - After FSBL, execution will continue at this payload,
    #     only the first entry with the specification is considered.
    #
    # Specify your payloads below:

    0 /oob-demo.bin                     0x20000000 BOOT
    1 /zephyr-blinkers.bin              0x20000000 BOOT
    2 /zephyr-echo_bot-rx_interrupt.bin 0x20000000 BOOT
    3 /zephyr-fat_fs.bin                0x20000000 BOOT
    4 /zephyr-getchar.bin               0x20000000 BOOT
    5 /zephyr-hello_world_user.bin      0x20000000 BOOT
    6 /zephyr-philosophers.bin          0x20000000 BOOT
    7 /zephyr-synchronization.bin       0x20000000 BOOT


Put the SD Card in the Genesys 2 FPGA board.

Connect the board's `UART` port to your PC and run a terminal emulator at 115200 BAUD.
Then switch one switch (SW0 to SW7) on and the rest off, power the board and the
respective application should boot after the bitstream has loaded. Try a different switch
to run a different application, then press the `RESET` button to boot the new application.

Getting Started
---------------

  | 📖 `Zephyr Documentation`_
  | 🚀 `Getting Started Guide`_
  | 🙋🏽 `Tips when asking for help`_
  | 💻 `Code samples`_

Code and Development
--------------------

  | 🌐 `Source Code Repository`_
  | 📦 `Releases`_
  | 🤝 `Contribution Guide`_

Community and Support
---------------------

  | 💬 `Discord Server`_ for real-time community discussions
  | 📧 `User mailing list (users@lists.zephyrproject.org)`_
  | 📧 `Developer mailing list (devel@lists.zephyrproject.org)`_
  | 📬 `Other project mailing lists`_
  | 📚 `Project Wiki`_

Issue Tracking and Security
---------------------------

  | 🐛 `GitHub Issues`_
  | 🔒 `Security documentation`_
  | 🛡️ `Security Advisories Repository`_
  | ⚠️ Report security vulnerabilities at vulnerabilities@zephyrproject.org

Additional Resources
--------------------
  | 🌐 `Zephyr Project Website`_
  | 📺 `Zephyr Tech Talks`_

.. _Zephyr Project Website: https://www.zephyrproject.org
.. _Discord Server: https://chat.zephyrproject.org
.. _supported boards: https://docs.zephyrproject.org/latest/boards/index.html
.. _Zephyr Documentation: https://docs.zephyrproject.org
.. _Introduction to Zephyr: https://docs.zephyrproject.org/latest/introduction/index.html
.. _Getting Started Guide: https://docs.zephyrproject.org/latest/develop/getting_started/index.html
.. _Contribution Guide: https://docs.zephyrproject.org/latest/contribute/index.html
.. _Source Code Repository: https://github.com/zephyrproject-rtos/zephyr
.. _GitHub Issues: https://github.com/zephyrproject-rtos/zephyr/issues
.. _Releases: https://github.com/zephyrproject-rtos/zephyr/releases
.. _Project Wiki: https://github.com/zephyrproject-rtos/zephyr/wiki
.. _User mailing list (users@lists.zephyrproject.org): https://lists.zephyrproject.org/g/users
.. _Developer mailing list (devel@lists.zephyrproject.org): https://lists.zephyrproject.org/g/devel
.. _Other project mailing lists: https://lists.zephyrproject.org/g/main/subgroups
.. _Code samples: https://docs.zephyrproject.org/latest/samples/index.html
.. _Security documentation: https://docs.zephyrproject.org/latest/security/index.html
.. _Security Advisories Repository: https://github.com/zephyrproject-rtos/zephyr/security
.. _Tips when asking for help: https://docs.zephyrproject.org/latest/develop/getting_started/index.html#asking-for-help
.. _Zephyr Tech Talks: https://www.zephyrproject.org/tech-talks
