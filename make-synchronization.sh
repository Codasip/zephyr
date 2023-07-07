rm build/zephyr/zephyr.bin
west build -p always -b codasip_l31helium samples/synchronization
cp build/zephyr/zephyr.bin builds/zephyr-synchronization.bin

