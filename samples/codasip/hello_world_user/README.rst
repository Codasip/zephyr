.. _hello_world_user:

Hello World
###########

Overview
********
A simple Hello World example that can be used with any supported board and
prints 'Hello World from UserSpace!' to the console.

It also reads from address 0 in User space to generate a PMP Load Access Fault (mcause 5).


Building and Running
********************

This project outputs 'Hello World from UserSpace!' to the console.

This application can be built and executed on Codasip's FPGA Platform:

./make-sample.sh codasip_inferno_l31carbon     samples/codasip          hello_world_user

To build for another board, change "codasip_inferno_l31carbon" above to that board's name.

Sample Output
=============

.. code-block:: console

    *** Booting Zephyr OS build v3.6.0-108-gfb92002b9a83 ***
    Hello World from privileged mode. (iiot-doombar_l31fluorine)
    Hello World from UserSpace! (iiot-doombar_l31fluorine)
    Reading data from address 0x00 - This should fail with 'E:  mcause: 5, Load access fault'
    E: 
    E:  mcause: 5, Load access fault
    E:   mtval: 0
    E:      a0: 00000000    t0: 00000052
    E:      a1: 00000000    t1: 0000004c
    E:      a2: 00000035    t2: ffffffff
    E:      a3: 00000073    t3: 0000002a
    E:      a4: 00000000    t4: 0000002e
    E:      a5: 200247f8    t5: 0000007f
    E:      a6: 00000068    t6: 00000010
    E:      a7: 0000006a
    E:      sp: 200247c0
    E:      ra: 200011c2
    E:    mepc: 200011d4
    E: mstatus: 00000080
    E: 
    E: >>> ZEPHYR FATAL ERROR 0: CPU exception on CPU 0
    E: Current thread: 0x200208a8 (unknown)
    E: Halting system

