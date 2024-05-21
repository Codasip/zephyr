.. _crypto_sample:

Crypto
######

Overview
********
An example to illustrate the usage of crypto APIs and Codasip's AEAD Adaptor with Fake Crypto Engine.

Building and Running
********************

This application can be built and executed on Codasip's IIOT-DoomBar FPGA Platform:

./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip  crypto

To build for another board, change "codasip_iiot_doombar_l31fluorine" above to that board's name.

Sample Output
=============

.. code-block:: console

    *** Booting Zephyr OS build v3.6.0-108-gfb92002b9a83 ***
    I: Cipher Sample
    I: ECB Mode
    E: AEAD Adaptor Algorithm/Mode/Key-Size not supported or Max sessions in progress
    I: CBC Mode
    E: AEAD Adaptor Algorithm/Mode/Key-Size not supported or Max sessions in progress
    I: CTR Mode
    E: AEAD Adaptor Algorithm/Mode/Key-Size not supported or Max sessions in progress
    I: CCM Mode
    E: AEAD Adaptor Algorithm/Mode/Key-Size not supported or Max sessions in progress
    I: GCM Mode
    E: AEAD Adaptor Algorithm/Mode/Key-Size not supported or Max sessions in progress
    I: FAKE Mode
    I: Output length (encryption): 64
    I: FAKE mode ENCRYPT - Match
    I: Output length (decryption): 64
    I: FAKE mode DECRYPT - Match
