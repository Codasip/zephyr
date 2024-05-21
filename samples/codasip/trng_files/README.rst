.. _trng_files:

TRNG and FAT Filesystem Sample Application
##########################################

Overview
********

This sample app demonstrates use of the HMAC DRBG using Codasip's TRNG (True Random Number
Generator) as a seed and also demonstrates the filesystem API 
which uses the FAT file system driver with SDHC card, SoC flash or external flash chip.

To access device the sample uses :ref:`disk_access_api`.

First the application reads and output the contents of the file ``zephyr-logo.txt``, if it exists.
Then 1073741824 random bytes of data are written to the file "/SD:/trng.dat". 

Note: This app generates HMAC DRBG random data (using Codasip TRNG Entropy data as a seed) 
and writes it to the SD Card. 
                 
Requirements for SD card support
********************************

This project requires SD card support and microSD card formatted with FAT filesystem.
See the :ref:`disk_access_api` documentation for Zephyr implementation details.
Boards that by default use SD card for storage:
``arduino_mkrzero``, ``esp_wrover_kit``, ``mimxrt1050_evk``, ``nrf52840_blip``, ``olimexino_stm32``
and various Codasip FPGA boards, e.g. ``codasip_iiot_doombar_l31fluorine``.

The sample should be able to run with any other board that has "zephyr,sdmmc-disk"
DT node enabled.

Building and Running
********************

This application can be built and executed on Codasip's IIOT-DoomBar FPGA Platform:

./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip  trng_files

To build for another board, change "codasip_iiot_doombar_l31fluorine" above to that board's name.

Sample output
=============

You should get a similar output as below:

.. code-block:: console

    *** Booting Zephyr OS build v3.6.0-108-gfb92002b9a83 ***
    spi_cfg->frequency = 400000 requested
    Switching SD Clock to slow clock
    spi_cfg->frequency = 20000000 requested
    Switching SD Clock to fast clock
    Sector size 512
    Memory Size(MB) 30436
    Disk mounted.
                                                    ╔¿»»»»»»»µµµµµ≡≡g╦╦╦╦qg∩
                                                   j╫╫N╥   »»»»»»»hh░░░░░╫╫H
                                                   ╫╫╫╫╫╫N╦»»»»h░░░░░░╦╫╫╫╫H
                                                  ╟╫╫╫╫╫╫╫╫╫N╦░░░░░░╫╫╫╫╫╫╫H
                                                 ╔╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫H
                                                 ╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╬▓╬╫╫╫╫╫╫╫╫H
                                                ╬╫╫╫╫╫╫╫╫╫╫╫╫╫╣╫▓▓▓╬╫╫╫╫╫╫╫H
                                               ]╫╫╫╫╫╫╫╫╫╫╫╬▓╫╫╫▓▓▓▓▓Ñ░╫╫╫╫H
                                              .╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫▓▓▓▓▓▓▓╬╫╫╫H
                                              ╬╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫▓▓▓▓▓▓▓▓▓╫╫H
                                             ]╫╫╫╫╫╫╬╫╫╫╫╫╫╫╫╫╫╫▓▓▓▓▓▓▓▓▓▓╬H
         .,,╥╥╥r                            j╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫╫▓▓▓▌▀╝╜^`
         *Ñ░░░░H                            ╫╫╫╫╫╫╫╫╫╫╫╫╫▌╝╝╜^``
           ╙Ñ░░             ,              ╟╫╫╫▌Å╝╜"``
             "╩            ]░N╥         ,╓∩`
               9N╥,      ╥Ñ░░░░Ñ╥  ,╓╗╬╫╫╫
                ╫╫╫╫╫DN═ⁿ""```     "╙╬╫╫╫M
                ╚╫Ñ╩^                  `╙                                               ,,
                                                    ╒▄▄▄▄▄▄▄▄▄⌐                        ▐██⌐
                                                    '▀▀▀▀▀████     ,;,          ,;,    ▐██⌐  ,,                     ,  J▄
                                                         ▄██▀   ╓███▀▀██▄  ▐████████▄  ▐█████████  ██▌   ,██▀ ███▄███  ¬~'
                                                       ▄███     ███,,,,██▌ ▐██┘   ▐██▌ ▐██▀   ║██  ╙██▄  ██▌  ███▀
                                                     ,███▀      ███▀▀▀▀▀▀▀ ▐██     ██▌ ▐██⌐   ▐██   ╙██▄██▌   ███
                                                    ▄███▄▄▄▄▄▄∩ ███▄  ,▄,  ▐██▄, ,███⌐ ▐██⌐   ▐██    ▀████    ███
                                                    ██████████L  ▀▀█████▀  ▐██▀████▀-  ▐██⌐   ▐██     ███     ███
                                                                           ▐██                    ▄▄▄███
                                                                           ╘▀▀                    ▀▀▀▀▀
    Get Codasip TRNG Entropy data and write to SD Card as /SD:/trng.dat
    Writing 1073741824 random bytes to file "/SD:/trng.dat"
    [00:00:00.055,000] <inf> sd: Maximum SD clock is under 25MHz, using clock of 20000000Hz
    [00:00:00.065,000] <inf> main: Block count 62333952
    Written 0x10000 to file /SD:/trng.dat
    Written 0x20000 to file /SD:/trng.dat
    Written 0x30000 to file /SD:/trng.dat
    Written 0x40000 to file /SD:/trng.dat
    Written 0x50000 to file /SD:/trng.dat
    Written 0x60000 to file /SD:/trng.dat
    Written 0x70000 to file /SD:/trng.dat
    Written 0x80000 to file /SD:/trng.dat
    Written 0x90000 to file /SD:/trng.dat
    Written 0xa0000 to file /SD:/trng.dat
    Written 0xb0000 to file /SD:/trng.dat
    Written 0xc0000 to file /SD:/trng.dat
    Written 0xd0000 to file /SD:/trng.dat
    Written 0xe0000 to file /SD:/trng.dat
    Written 0xf0000 to file /SD:/trng.dat
    Written 0x100000 to file /SD:/trng.dat
    Written 0x110000 to file /SD:/trng.dat
    Written 0x120000 to file /SD:/trng.dat
    Written 0x130000 to file /SD:/trng.dat

    Etc.
