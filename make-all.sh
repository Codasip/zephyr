#!/bin/bash -xe

./make-sample.sh codasip_l31platformiiot samples/codasip        entropy

./make-sample.sh codasip_l31platformiiot samples/codasip        blinkers
./make-sample.sh codasip_l31platformiiot samples/subsys/console getchar
./make-sample.sh codasip_l31platformiiot samples                synchronization
./make-sample.sh codasip_l31platformiiot samples                philosophers
./make-sample.sh codasip_l31platformiiot samples/drivers/uart   echo_bot               echo_bot-rx_interrupt
./make-sample.sh codasip_l31platformiiot samples/codasip        hello_world_interrupt
./make-sample.sh codasip_l31platformiiot samples/codasip        hello_world_user
./make-sample.sh codasip_l31platformiiot samples/codasip        fat_fs

./make-sample.sh codasip_l31carbon samples/codasip        blinkers
./make-sample.sh codasip_l31carbon samples/subsys/console getchar
./make-sample.sh codasip_l31carbon samples                synchronization
./make-sample.sh codasip_l31carbon samples                philosophers
./make-sample.sh codasip_l31carbon samples/drivers/uart   echo_bot               echo_bot-rx_interrupt
./make-sample.sh codasip_l31carbon samples/codasip        hello_world_interrupt
./make-sample.sh codasip_l31carbon samples/codasip        hello_world_user
./make-sample.sh codasip_l31carbon samples/codasip        fat_fs

./make-sample.sh codasip_l31helium samples/codasip        blinkers
./make-sample.sh codasip_l31helium samples/subsys/console getchar
./make-sample.sh codasip_l31helium samples                synchronization
./make-sample.sh codasip_l31helium samples                philosophers
./make-sample.sh codasip_l31helium samples/drivers/uart   echo_bot               echo_bot-rx_interrupt
./make-sample.sh codasip_l31helium samples/codasip        hello_world_interrupt
./make-sample.sh codasip_l31helium samples/codasip        hello_world_user
./make-sample.sh codasip_l31helium samples/codasip        fat_fs
