.. _fat_fs:

FAT Filesystem Sample Application
#################################

Overview
********

This sample app demonstrates use of the filesystem API and uses the FAT file
system driver with SDHC card.

To access device the sample uses :ref:`disk_access_api`.

First the application reads and output the contents of the file ``zephyr-logo.txt``, if it exists.
Then reads the root directory listing and outputs that, followed by it writing counting data 
to the file "/SD:/counting.dat". 

Requirements for SD card support
********************************

This project requires SD card support and microSD card formatted with FAT filesystem.
See the :ref:`disk_access_api` documentation for Zephyr implementation details.
Boards that by default use SD card for storage:
``arduino_mkrzero``, ``esp_wrover_kit``, ``mimxrt1050_evk``, ``nrf52840_blip``, ``olimexino_stm32``
and various Codasip FPGA boards, e.g. ``codasip_inferno_l31carbon``.

The sample should be able to run with any other board that has "zephyr,sdmmc-disk"
DT node enabled.

Requirements for setting up FAT FS on SoC flash
***********************************************

For the FAT FS to work with internal flash, the device needs to support erase
pages of size <= 4096 bytes and have at least 64kiB of flash available for
FAT FS partition alone.
Currently the following boards are supported:
``nrf52840dk_nrf52840`` and various Codasip FPGA boards, e.g. ``codasip_inferno_l31carbon``.

Building and Running
********************

This application can be built and executed on Codasip's FPGA Platform:

./make-sample.sh codasip_inferno_l31carbon     samples/codasip          fat_fs

To build for another board, change "codasip_inferno_l31carbon" above to that board's name.

Sample Output
=============

.. code-block:: console

    *** Booting Zephyr OS build v3.6.0-108-gfb92002b9a83 ***
    disk_access_init(disk_pdrv)
    spi_cfg->frequency = 400000 requested
    Switching SD Clock to slow clock
    spi_cfg->frequency = 20000000 requested
    Switching SD Clock to fast clock
    disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_COUNT, &block_count)
    disk_access_ioctl(disk_pdrv, DISK_IOCTL_GET_SECTOR_SIZE, &block_size)
    Sector size 512
    Memory Size(MB) 30436
    fs_mount(&mp)
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

    Listing dir /SD: ...
    [FILE] zephyr-blinkers.bin (size = 24724)
    [FILE] config.txt (size = 1011)
    [DIR ] .Trash-529202565
    [FILE] counting.dat (size = 16777216)
    [FILE] oob-demo.bin (size = 41372)
    [DIR ] A
    [FILE] zephyr-fat_fs.bin (size = 82364)
    [FILE] zephyr-getchar.bin (size = 24832)
    [FILE] zephyr-hello_world_user.bin (size = 152156)
    [FILE] zephyr-philosophers.bin (size = 54800)
    [FILE] zephyr-synchronization.bin (size = 27252)
    [FILE] system_genesys2-iiot_doombar_l31fluorine.bit (size = 11443725)
    Writing 16777216 counting bytes to file "/SD:/counting.dat"
    [00:00:00.058,000] <inf> sd: Maximum SD clock is under 25MHz, using clock of 20000000Hz
    [00:00:00.074,000] <inf> main: Block count 62333952
    Written 0x10000 to file /SD:/counting.dat
    Written 0x20000 to file /SD:/counting.dat
    Written 0x30000 to file /SD:/counting.dat
    Written 0x40000 to file /SD:/counting.dat
    Written 0x50000 to file /SD:/counting.dat
    Written 0x60000 to file /SD:/counting.dat
    Written 0x70000 to file /SD:/counting.dat
    Written 0x80000 to file /SD:/counting.dat

    Etc.
