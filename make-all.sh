#!/bin/bash -xe

# Copyright (c) 2024 Codasip s.r.o.
# SPDX-License-Identifier: Apache-2.0

./make-sample.sh codasip_hobgoblin_h730     samples/codasip          blinkers
./make-sample.sh codasip_hobgoblin_h730     samples/subsys/console   getchar
./make-sample.sh codasip_hobgoblin_h730     samples                  synchronization
./make-sample.sh codasip_hobgoblin_h730     samples                  philosophers
./make-sample.sh codasip_hobgoblin_h730     samples/drivers/uart     echo_bot               echo_bot-rx_interrupt
#./make-sample.sh codasip_hobgoblin_h730     samples/codasip          hello_world_interrupt USER TEST INTERRUPTS HAVE BEEN REMOVED FROM THE RTL
./make-sample.sh codasip_hobgoblin_h730     samples/codasip          hello_world_user
./make-sample.sh codasip_hobgoblin_h730     samples/codasip          fat_fs

./make-sample.sh codasip_hobgoblin_a730     samples/codasip          blinkers
./make-sample.sh codasip_hobgoblin_a730     samples/subsys/console   getchar
./make-sample.sh codasip_hobgoblin_a730     samples                  synchronization
./make-sample.sh codasip_hobgoblin_a730     samples                  philosophers
./make-sample.sh codasip_hobgoblin_a730     samples/drivers/uart     echo_bot               echo_bot-rx_interrupt
#./make-sample.sh codasip_hobgoblin_a730     samples/codasip          hello_world_interrupt USER TEST INTERRUPTS HAVE BEEN REMOVED FROM THE RTL
#./make-sample.sh codasip_hobgoblin_a730     samples/codasip          hello_world_user      NO PMP ON A730 SO THIS DOES NOT WORK
./make-sample.sh codasip_hobgoblin_a730     samples/codasip          fat_fs

./make-sample.sh codasip_inferno_l31carbon     samples/codasip          blinkers
./make-sample.sh codasip_inferno_l31carbon     samples/subsys/console   getchar
./make-sample.sh codasip_inferno_l31carbon     samples                  synchronization
./make-sample.sh codasip_inferno_l31carbon     samples                  philosophers
./make-sample.sh codasip_inferno_l31carbon     samples/drivers/uart     echo_bot               echo_bot-rx_interrupt
#./make-sample.sh codasip_inferno_l31carbon     samples/codasip          hello_world_interrupt USER TEST INTERRUPTS HAVE BEEN REMOVED FROM THE RTL
./make-sample.sh codasip_inferno_l31carbon     samples/codasip          hello_world_user
./make-sample.sh codasip_inferno_l31carbon     samples/codasip          fat_fs

./make-sample.sh codasip_inferno_l31helium     samples/codasip          blinkers
./make-sample.sh codasip_inferno_l31helium     samples/subsys/console   getchar
./make-sample.sh codasip_inferno_l31helium     samples                  synchronization
./make-sample.sh codasip_inferno_l31helium     samples                  philosophers
./make-sample.sh codasip_inferno_l31helium     samples/drivers/uart     echo_bot               echo_bot-rx_interrupt
#./make-sample.sh codasip_inferno_l31helium     samples/codasip          hello_world_interrupt USER TEST INTERRUPTS HAVE BEEN REMOVED FROM THE RTL
./make-sample.sh codasip_inferno_l31helium     samples/codasip          hello_world_user
./make-sample.sh codasip_inferno_l31helium     samples/codasip          fat_fs

./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          blinkers              # GPIO demo - Flash some LEDs and get switch input
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/subsys/console   getchar               # UART demo - input and output over the UART
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples                  synchronization       # Task switch & synchronisation demo
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples                  philosophers          # Task switch & synchronisation demo
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/drivers/uart     echo_bot               echo_bot-rx_interrupt # UART and interrupt demo
#./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          hello_world_interrupt # UART and interrupt demo # USER TEST INTERRUPTS HAVE BEEN REMOVED FROM THE RTL
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          hello_world_user      # User mode PMP access violation demo
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          fat_fs                # SD card access demo

./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          entropy               # Codasip Entropy TRNG
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          mbedtls               # mbedtls test suite (not using Entropy TRNG, uses fixed seed)
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          hmac_drbg             # HMAC DRBG demo using Codasip Entropy TRNG as the seed
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          trng_files            # Write 1GB random TRNG and DRBG data to SD Card files trng.bin and drbg.bin
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          watchdog              # Codasip Watchdog (feeding and reset) example
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          policy_mngr           # Codasip Policy Manager example.
./make-sample.sh codasip_iiot_doombar_l31fluorine   samples/codasip          crypto                # Codasip Fake AEAD Adpator Crypto example.
